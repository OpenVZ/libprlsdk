/*
 * IOSSLInterface.cpp: SSL service functions
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#if defined(_WIN_)
#include <QSslSocket>
#endif

#include <Libraries/OpenSSL/OpenSSL.h>
#include "IOSSLInterface.h"
#undef OPENSSL_NO_DEPRECATED
#include "Socket/SslHelper.h"

namespace IOService
{


bool IOCertificate::isNull() const
{
	return m_certificate.isEmpty();
}

QString IOCertificate::errorString() const
{
	return m_errorString;
}

void IOCertificate::setErrorString(const QString& errorString) const
{
	unsigned long errorCode = ERR_get_error();
	m_errorString = errorString;
	if (errorCode != 0)
	{
		const size_t ErrBuffSize = 256;
		char errBuff[ ErrBuffSize ];
		ERR_error_string_n( errorCode, errBuff, ErrBuffSize );
		WRITE_TRACE(DBG_FATAL, "%s. %s", qPrintable(errorString),  errBuff);
		m_errorString.append(":");
		m_errorString.append(errBuff);
	}
	else
	{
		WRITE_TRACE(DBG_FATAL,"%s", qPrintable(errorString));
	}
}

bool IOCertificate::isExpired(const QDateTime& timeToCompare) const
{
	(void)timeToCompare;

	SmartPtr< X509> openssl_cert (SSLHelper::QByteArrayToX509(m_certificate), X509_free);

	if (!openssl_cert.isValid())
		return false;

	int ret = X509_cmp_current_time(X509_get_notAfter(openssl_cert.get()));
	// 0 - error
	// < 0 - expired
	if (ret == 0)
	{
		setErrorString( "Certificate has wrong estimate date format");
		return false;
	}
	return ret < 0;
}

bool IOCertificate::isValid() const
{
	return (!m_certificate.isEmpty() && verifyCertificateFormat(m_certificate));
}

bool IOCredentials::isValid() const
{
	if (certificate.isNull() || privKey.isEmpty())
	{
		return false;
	}

	return certificate.isValid() && verifyPrivateKeyFormat(privKey);
}

bool IOCredentials::isSigned(const QByteArray& CACertificate) const
{
	return SSLHelper::verifyCertificate(*this, CACertificate);
}

bool IOCredentials::isNull() const
{
	return (certificate.isNull() || privKey.isNull());
}

IOCredentials IOCredentials::fromPrivateKeyAndCertificatesChain(const char *privateKey, const char *certificatesChain)
{
	IOCredentials credentials;
	if (certificatesChain && privateKey)
	{
		credentials.privKey = privateKey;
		IOService::parseCertificateSignResponse(certificatesChain, credentials);
	}
	return credentials;
}

bool verifyPrivateKeyFormat(const QByteArray& pkey)
{
    SmartPtr< RSA> openssl_rsa_key (SSLHelper::QByteArrayToRSA(pkey), RSA_free);
	return openssl_rsa_key.isValid();
}

bool verifyCertificateFormat(const QByteArray& pubKey)
{
	SmartPtr< X509> openssl_cert (SSLHelper::QByteArrayToX509(pubKey), X509_free);
	return openssl_cert.isValid();
}

bool generateCredentials(const QString& userId,
                         const QString& certificateRole,
                         IOCredentials& credentials,
                         int bits,
                         int serial,
                         int days)
{
	SmartPtr< EVP_PKEY> privateKey(EVP_PKEY_new(), EVP_PKEY_free);

	if ( !privateKey )
		return false;

	RSA *rsaKey = RSA_generate_key(bits, RSA_3, NULL, NULL);

	if( !rsaKey )
		return false;

	if (!EVP_PKEY_assign_RSA(privateKey.get(), rsaKey))
	{
		RSA_free(rsaKey);
		return false;
	}

	SmartPtr< X509> x509cert (X509_new(), X509_free);

	if (!x509cert.isValid())
		return false;

	X509_set_version(x509cert.get(), 3);

	ASN1_INTEGER_set(X509_get_serialNumber(x509cert.get()), serial);

	X509_gmtime_adj(X509_get_notBefore(x509cert.get()), (long) 60 * 60 * 24  *(-2));

	X509_gmtime_adj(X509_get_notAfter(x509cert.get()), (long) 60 * 60 * 24 * days);

	X509_set_pubkey(x509cert.get(), privateKey.get());

	if (!userId.isEmpty())
	{
		X509_NAME *name = X509_get_subject_name(x509cert.get());

		if (!name)
		{
			return false;
		}

		if (!userId.isEmpty())
		{
			X509_NAME_add_entry_by_NID(name, NID_commonName , MBSTRING_ASC,
			                           (unsigned char*) userId.toLatin1().constData(), -1, -1, 0);
		}

		if (!certificateRole.isEmpty())
		{
			X509_NAME_add_entry_by_NID(name, NID_role , MBSTRING_ASC,
			                           (unsigned char*) certificateRole.toLatin1().constData(), -1, -1, 0);
		}

		X509_NAME_add_entry_by_NID(name, NID_organizationName, MBSTRING_ASC,
								   (unsigned char*)"Parallels", -1, -1, 0);

		X509_NAME_add_entry_by_NID(name, NID_organizationalUnitName, MBSTRING_ASC,
								   (unsigned char*)"Mobile", -1, -1, 0);

	}

	credentials.certificate.fromByteArray(SSLHelper::X509ToQByteArray(x509cert.get()));

	credentials.privKey = SSLHelper::RSAToQByteArray(rsaKey);

	return credentials.isValid();
}

bool generateCSR(const IOCredentials& credentials, QByteArray& signingRequest)
{
	SmartPtr< X509> userCert( SSLHelper::QByteArrayToX509(credentials.certificate.toByteArray()), X509_free);

	if (!userCert.isValid())
		return false;

	SmartPtr< EVP_PKEY> privateKey(EVP_PKEY_new(), EVP_PKEY_free);

	if ( !privateKey )
		return false;

	RSA *rsaKey = SSLHelper::QByteArrayToRSA(credentials.privKey);

	if( !rsaKey )
		return false;

	if (!EVP_PKEY_assign_RSA(privateKey.get(), rsaKey))
	{
		RSA_free(rsaKey);
		return false;
	}

	X509_REQ* req  = X509_to_X509_REQ(userCert.get(), privateKey.get(), EVP_sha1());

	if (!req )
		return false;

	signingRequest =  SSLHelper::X509_REQToQByteArray(req);

	return !signingRequest.isEmpty();
}

bool parseCertificateSignResponse(const QByteArray& signedCert, IOCredentials& credentials)
{
	int prevPos(-1);
	while(true)
	{
		int certPosition = signedCert.indexOf("-----BEGIN CERTIFICATE-----", prevPos + 1);
		if (certPosition == -1 )
		{
			if (prevPos != -1)
			{
				QByteArray secondCert = signedCert.right(signedCert.size()- prevPos);
				credentials.certificate.fromByteArray(secondCert);
			}
			break;
		}
		else if(prevPos != -1)
		{
			QByteArray certificate = signedCert.mid(prevPos, certPosition - prevPos);
			credentials.certificatesChain.push_back(IOCertificate());
			credentials.certificatesChain.back().fromByteArray(certificate);
		}

		prevPos = certPosition;
	}

	return credentials.certificate.isValid();
}

QString IOCertificate::getRole() const
{
	SmartPtr< X509> pnt ( SSLHelper::QByteArrayToX509(m_certificate), X509_free);

	if (!pnt.isValid()) {
		setErrorString("Unable to get peer certifiate");
		return QString();
	}

	X509_NAME* localSubjectName = X509_get_subject_name(pnt.get());

	int roleIndex(X509_NAME_get_index_by_NID(localSubjectName , NID_role, -1));

	if  (roleIndex == -1) {
		setErrorString( "Missing role attribute");
		return QString();
	}

	X509_NAME_ENTRY * peerRoleEntry = X509_NAME_get_entry(localSubjectName, roleIndex);

	if (!peerRoleEntry) {
		setErrorString( "Cannot get role attribute");
		return QString();
	}

	ASN1_STRING * peerRoleAsn1String = X509_NAME_ENTRY_get_data(peerRoleEntry);
	if (!peerRoleAsn1String) {
		setErrorString( "Cannot get role attribute");
		return QString();
	}

	return QString(QByteArray((char*)ASN1_STRING_data(peerRoleAsn1String),
							  ASN1_STRING_length(peerRoleAsn1String)));
}

void deinitSSLLibrary()
{
	/*
	* Note: OpenSSL library helper DO NOT USE ANY QT! Thus decision what
	* exact function to call is done here.
	*/
	OpenSSL::Finalize();
}

bool initSSLLibrary()
{
	/*
	 * We share openssl lib with Qt on iPhone,
	 * because iPhone uses static openssl lib and we must
	 * do all our best to use this lib with care, which is not
	 * reenterable: not to deinit it, not use reenterable functions.
	 *
	 * Note: OpenSSL library helper DO NOT USE ANY QT! Thus decision what
	 * exact function to call is done here.
	 */
	return OpenSSL::Initialize();
}

}

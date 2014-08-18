/*
 * SslHelper.h
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

#ifndef SSLHELPER_H
#define SSLHELPER_H

#ifdef _WIN_
 #include <Windows.h>
#endif

// OpenSSL headers
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#ifndef OPENSSL_NO_RSA
#include <openssl/rsa.h>
#endif
#ifndef OPENSSL_NO_DSA
#include <openssl/dsa.h>
#endif

#include "../IOConnection.h"
#include <Libraries/Logging/Logging.h>
#include <Libraries/OpenSSL/OpenSSL.h>

namespace IOService {
// SSL helper
class SSLHelper
{
public:
	bool InitSSLContext(const IOCredentials& cert);

	void DeinitSSLContext();

	bool postHandshakeCheck(SSL* ssl, bool rehandshake = false);

	bool SetLocalCredentials(const IOCredentials& credentials);

	SSL_CTX* GetClientSSLContext ();

	SSL_CTX* GetServerSSLContext ();

	static void printCiphers(SSL* ssl);

	static QString GetConnectionChipher(SSL* ssl);

	static bool isNeedPostConnectionCheck(const QString& cipherName);

	static bool isValidCipher(const QString& cipherName);

	static bool isTrustedChannelCipher(const QString& cipherName);

	// Default SSL buffer read/write size
	// NOTE: we use 15kb (not 16kb) because of wrong SSL header check
	//       (#SocketClientPrivate::sslHeaderCheck). This function
	//       checks that SSL header must be <= SSLMaxDataLength,
	//       but SSL _can_ send 16kb + padding bytes, so old
	//       released versions of Parallels product can't accept
	//       such packages and will disconnect the peer. If we reduce
	//       the number of default SSL buffer SSL engine will never
	//       exceed this threshold.
	// ALSO: #SocketClientPrivate::sslHeaderCheck - useless length check !!!
	enum SSLConsts { SSLBuffSize = 15*1024 };

	static X509* QByteArrayToX509(const QByteArray& pem);

	static RSA* QByteArrayToRSA(const QByteArray& byteArray);

	static QByteArray RSAToQByteArray(RSA * rsa);

	static QByteArray X509ToQByteArray(X509 * x);

	static QByteArray X509_REQToQByteArray(X509_REQ * req);

	static bool verifyCertificate(const IOCredentials& credentials,
	                         const QByteArray& caCertificate = QByteArray());

	X509*	s_localCert;

private:
	static DH* DHCallback ( SSL*, int, int );
	bool SSL_CTX_set_credentials(SSL_CTX* ctx, const IOCredentials& cert);
private:
	SSL_CTX* s_clientSSLCtx;
	SSL_CTX* s_serverSSLCtx;
	static const char* s_serverSessionIdContext;
};

// SSL data length must be <= 2^14
enum SSLConsts { SSLMaxDataLength = 16384,
                 SSLCipherDescriptionSize = 5,
                 SSLKeyExchangePosition  = 2
               };

}
#endif // SSLHELPER_H

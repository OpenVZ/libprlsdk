                 How To Build OpenSSL for Parallels purposes


NOTE: Windows part of this manual is obsolete. Use the build_openssl_for_windows.cmd
	script instead. You'll need the following tools: tar.exe, patch.exe, perl.exe.
	Tar.exe and patch.exe can be used from a MSYS package. Perl from MSYS didn't work for
	me, so I've used ActiveState perl (http://www.perl.org/get.html). Make sure you have all
	these tools available via %PATH%. Also you'll need VS2013 installed and a qmake environment
	for building our system msvcrt.dll import library. Simply run the script and it will
	guide you.

1. Sources
-----------
  Untar openssl sources:

  mac, linux, win : openssl-1.0.1c

  example : tar -xzf openssl-1.0.1c.tar.gz


2. Patch
-----------
  Patch sources with corresponding patch.

	mac, linux, win : openssl-1.0.1c-configure.patch

  This patch includes gcc option "-fPIC -fvisibility=hidden" for Linux,
  "-fvisibility=hidden" for Mac OS and "/MT" for Windows.

  example : patch -p0 < openssl-1.0.1c-configure.patch


3. Build
-----------
  Change directory to openssl sources dir and read INSTALL<platform> manual.

  NOTE: To build openssl for MacOS X in 64bit mode need call
	$ ./Configure darwin64-x86_64-cc
	instead
	$ ./config

  NOTE: To build openssl for Linux used the following
	x86 :
	$ ./Configure linux-generic32 -m32
	x64 :
	$ ./Configure linux-x86_64

  NOTE: To build openssl for Windows used the following
	x86 :
        Run VS2005 x86 command prompt
	> perl Configure enable-static-engine VC-WIN32
	> ms\do_ms
	> nmake -f ms\ntdll.mak
	x64 :
        Run VS2005 x64 command prompt
	> perl Configure no-asm enable-static-engine VC-WIN64A
	> ms\do_win64a
	> nmake -f ms\ntdll.mak

4. Upgrade to new openssl version on mac:
------------------------------------------
o Build both i386 and x64 library version as described above.
o Make two platforms lib version and copy them to ssl/mac/lib
	$ lipo -create -output libprl_ssl.a openssl-1.0.1c-32/libssl.a openssl-1.0.1c-64/libssl.a
	$ lipo -create -output libprl_crypt.a openssl-1.0.1c-32/libcrypt.a openssl-1.0.1c-64/libcrypt.a
	$ ranlib libprl_crypt.a libprl_ssl.a
o Copy headers from ${build_dir}/include/openssl to ssl/mac/include/openssl, excluding file opensslconf.h
o Rename build_dir_32/include/openssl/opensslconf.h to ssl/mac/include/openssl/opensslconf-i386.h, and
  build_dir_64/include/openssl/opensslconf.h to ssl/mac/include/openssl/opensslconf-x86_64.h

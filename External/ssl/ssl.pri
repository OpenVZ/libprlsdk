win32 {
	INCLUDEPATH *= $$PWD/win32/include32
	x86_64 {
		QMAKE_LIBDIR *= $$PWD/win32/lib64
		LIBS += \
			-llibeay32 \
			-lssleay32
	} else {
		QMAKE_LIBDIR *= $$PWD/win32/lib
		LIBS += \
			-lprl_libeay32 \
			-lprl_ssleay32
	}
	LIBS += \
		-lgdi32 \
		-ladvapi32 \
		-luser32 \
		-lws2_32
}

# we use openssl >=0.9.8, so we cannot rely on system's openssl

mac: !ios {
	INCLUDEPATH *= $$PWD/mac/include
	QMAKE_LIBDIR *= $$PWD/mac/lib
	LIBS += -lprl_ssl -lprl_crypto
	!isEmpty(_linkable) {
		SOURCES *= $$PWD/prl_ssl_valgrind_wrappers.cpp
	}
}

ios: INCLUDEPATH *= $$PWD/ios/include

android {
	INCLUDEPATH	*=$$PWD/android/include
	LIBS		+= -lprl_ssl -lprl_crypto

	contains(ANDROID_TARGET_PLATFORM_ARCHITECTURE, ARM_V7) {
		QMAKE_LIBDIR	*=$$PWD/android/lib-armv7-a
	}

	contains(ANDROID_TARGET_PLATFORM_ARCHITECTURE, ARM_V5) {
		error(Add android arm 5 binaries)
	}
}

linux-* {
	linux-*-64 {
		QMAKE_LIBDIR *= $$PWD/linux/lib64
		INCLUDEPATH *= $$PWD/linux/include64
	}
	else {
		QMAKE_LIBDIR *= $$PWD/linux/lib
		INCLUDEPATH *= $$PWD/linux/include32
	}
	LIBS += -lprl_ssl -lprl_crypto -ldl
	!isEmpty(_linkable) {
		SOURCES *= $$PWD/prl_ssl_valgrind_wrappers.cpp
	}
}

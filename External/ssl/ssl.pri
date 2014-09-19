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

linux-* {
	QMAKE_LIBDIR *= $$PWD/linux/lib64
	INCLUDEPATH *= $$PWD/linux/include64
	LIBS += -lprl_ssl -lprl_crypto -ldl
	!isEmpty(_linkable) {
		SOURCES *= $$PWD/prl_ssl_valgrind_wrappers.cpp
	}
}

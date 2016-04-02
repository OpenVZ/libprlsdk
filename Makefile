all: Sources/SDK/Makefile
	$(MAKE) -C Sources/SDK

Sources/SDK/Makefile: Sources/Build/Build.pri Sources/Build/Current-gen.ver Sources/SDK/Handles/parallels-sdk.pc
	cd Sources/SDK && qmake-qt4 \
		ENABLE_LOCAL_DEPS=$$ENABLE_LOCAL_DEPS \
		LOCAL_DEPS_INCLUDE=$$LOCAL_DEPS_INCLUDE \
		LOCAL_DEPS_LIBS=$$LOCAL_DEPS_LIBS

Sources/Build/Build.pri: Makefile.version Sources/Build/Build.cfg
	cd Sources && ./Gen.py

Sources/Build/Current-gen.ver: Sources/Build/Build.pri

Sources/SDK/Handles/parallels-sdk.pc: Sources/Build/Build.pri

install: all
	cd Sources/SDK && make install \
		ENABLE_LOCAL_DEPS=$$ENABLE_LOCAL_DEPS \
		LOCAL_DEPS_INCLUDE=$$LOCAL_DEPS_INCLUDE \
		LOCAL_DEPS_LIBS=$$LOCAL_DEPS_LIBS

clean:
	$(MAKE) -C Sources/SDK clean

distclean:
	$(MAKE) -C Sources/SDK distclean

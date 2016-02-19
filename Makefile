all:
	cd Sources && ./Gen.py
	cd Sources/SDK && qmake-qt4 \
		ENABLE_LOCAL_DEPS=$$ENABLE_LOCAL_DEPS \
		LOCAL_DEPS_INCLUDE=$$LOCAL_DEPS_INCLUDE \
		LOCAL_DEPS_LIBS=$$LOCAL_DEPS_LIBS
	cd Sources/SDK && $(MAKE)
install: all
	cd Sources/SDK && make install \
		ENABLE_LOCAL_DEPS=$$ENABLE_LOCAL_DEPS \
		LOCAL_DEPS_INCLUDE=$$LOCAL_DEPS_INCLUDE \
		LOCAL_DEPS_LIBS=$$LOCAL_DEPS_LIBS

clean:
	cd Sources/SDK && $(MAKE) $@

distclean:
	cd Sources/SDK && $(MAKE) $@

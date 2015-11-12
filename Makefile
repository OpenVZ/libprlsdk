all:
	cd Sources && ./Gen.py
	cd Sources/SDK && qmake-qt4
	cd Sources/SDK && $(MAKE)
install: all
	cd Sources/SDK && make install

clean:
	cd Sources/SDK && $(MAKE) $@

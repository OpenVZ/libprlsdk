all:
	cd Sources && ./Gen.py
	cd Sources/SDK && qmake-qt4
	cd Sources/SDK && make
install: all
	cd Sources/SDK && make install

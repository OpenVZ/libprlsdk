[![Build Status](https://travis-ci.org/Parallels/parallels-sdk.png?branch=master)](https://travis-ci.org/Parallels/parallels-sdk)

Parallels SDK
=============

Parallels Virtualization SDK is a development kit used to create and integrate custom software solutions with Parallels Cloud Server. The SDK provides cross-platform ANSI C and
Python APIs.

Documetation for commercial version of SDK is available here:

- [Parallels_Virtualization_SDK_Programmers_Guide.pdf](http://download.parallels.com/doc/pcs/pdf/Parallels_Virtualization_SDK_Programmers_Guide.pdf)
- [Parallels_C_API_Reference.pdf](http://download.parallels.com/doc/pcs/pdf/Parallels_C_API_Reference.pdf)
- [Parallels_Python_API_Reference.pdf](http://download.parallels.com/doc/pcs/pdf/Parallels_Python_API_Reference.pdf)

Trial version of Paralles Cloud Server can be downloaded here [http://sp.parallels.com/en/products/pcs/](http://sp.parallels.com/en/products/pcs/).

How to install
--------------
To build the SDK run:

```bash
cd parallels-sdk/Sources
./Gen.py
cd SDK
qmake
make
make install
```


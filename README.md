[![Build Status](https://travis-ci.org/CloudServer/parallels-sdk.svg?branch=master)](https://travis-ci.org/CloudServer/parallels-sdk)

Virtuozzo SDK
=============

Virtuozzo SDK is a development kit used to create and integrate custom software
solutions with Virtuozzo. The SDK provides cross-platform ANSI C and Python
APIs.

Documentation for commercial version of SDK is available here:

- [Virtuozzo SDK Programmers Guide.pdf](http://download.cloudserver.parallels.com/doc/pcs/en_us/parallels/6/current/pdf/Parallels_Virtualization_SDK_Programmers_Guide.pdf)
- [Virtuozzo C API Reference.pdf](http://download.cloudserver.parallels.com/doc/pcs/en_us/parallels/6/current/pdf/Parallels_C_API_Reference.pdf)
- [Virtuozzo Python API Reference.pdf](http://download.cloudserver.parallels.com/doc/pcs/en_us/parallels/6/current/pdf/Parallels_Python_API_Reference.pdf)

Trial version of Virtuozzo can be downloaded here [http://www.odin.com/products/virtuozzo/](http://www.odin.com/products/virtuozzo/).

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


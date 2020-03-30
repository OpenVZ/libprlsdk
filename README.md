[![Build Status](https://travis-ci.org/CloudServer/virtuozzo-sdk.svg?branch=master)](https://travis-ci.org/CloudServer/virtuozzo-sdk)

Virtuozzo SDK
=============

Virtuozzo SDK is a development kit used to create and integrate custom software
solutions with Virtuozzo. The SDK provides cross-platform ANSI C and Python
APIs.

Documentation for commercial version of SDK is available here:

- [Virtuozzo SDK Programmers Guide.pdf](http://updates.virtuozzo.com/doc/pcs/en_us/virtuozzo/6/current/pdf/Virtuozzo_Virtualization_SDK_Programmers_Guide.pdf)
- [Virtuozzo C API Reference.pdf](http://updates.virtuozzo.com/doc/pcs/en_us/virtuozzo/6/current/pdf/Virtuozzo_C_API_Reference.pdf)
- [Virtuozzo Python API Reference.pdf](http://updates.virtuozzo.com/doc/pcs/en_us/virtuozzo/6/current/pdf/Virtuozzo_Python_API_Reference.pdf)

Trial version of Virtuozzo can be downloaded here [https://www.virtuozzo.com/products/virtuozzo/](https://www.virtuozzo.com/products/virtuozzo/).

How to install
--------------

SDK depends on [libprlcommon](https://src.openvz.org/scm/ovz/libprlcommon.git) and
[libprlxmlmodel](https://src.openvz.org/scm/ovz/libprlxmlmodel.git) libraries.
One needs to install them in order to build SDK.

To build the SDK run:

```bash
make
sudo make install
```

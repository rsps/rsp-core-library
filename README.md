# rsp-core-library

Open-source library for medical device software.

This library are being developed following processes adhering to IEC 62304 and IEC 60601-1 chapter 14.
At least one versioned release will be used in a TGM device by RSP Systems and submitted for FDA approval.

Developed by RSP Systems.

## License information

All source files shall include the following file header:

```
/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      <name>
 */
```
**Note:** To add \license tag to doxygen, please see: https://stackoverflow.com/questions/12353409/doxygen-and-license-copyright-informations

## Prerequisites

This library uses libraries for OpenSSL, Curl, WPA Supplicant, FreeType2 and Zlib when building to a Linux platform.
The libraries should be available for the linker to build the program.

To install those libraries on on Debian based systems, execute the following in a terminal:

```
sudo apt install libssl-dev libcurl4 libcurl4-openssl-dev libwpa-client-dev libfreetype-dev zlib1g-dev
```

To execute the unit tests a few other libraries are needed:
```
sudo apt install libpcre2-dev pkg-config
``` 

The code is using some C++20 features, so make sure to use a recent compiler to build the project.
To install GCC-11 on Ubuntu 20.04:

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt install gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
sudo update-alternatives --config gcc
```

Also cmake should be at least version 3.18.


## Build and tests

To build the library and accompanied test, simply go into the build directory and run `cmake ..`:

```
cd build
cmake ..
make
```
Tests can now be executed with `./rsp-core-lib-test` or simply `ctest`



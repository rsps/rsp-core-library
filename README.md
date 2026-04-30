# RSP Core Library

Open-source library for medical device software.

This library are being developed following processes adhering to IEC 62304 and IEC 60601-1 chapter 14.
At least one versioned release will be used in a TGM device by RSP Systems and submitted for FDA approval.

Developed by [RSP Systems A/S](https://rspsystems.com/).

## Prerequisites

This project has several development dependencies that must be installed, before you are able to build it.

### Debian / Ubuntu

In the root of the project, run the following script:

```sh
sudo ./install-dev-libraries.sh
```

### GCC

The code is using some C++ 20 features, so make sure to use a recent compiler to build the project.
To install GCC-11 on Ubuntu 20.04:

```sh
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt install gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
sudo update-alternatives --config gcc
```

## Build and tests

To build the library (and possibly accompanying tests), simply run `cmake` as follows:

```sh
cmake --preset debug|release|debug-with-tests|release-with-tests
cmake --build --preset debug|release|debug-with-tests|release-with-tests
```

Tests can now be executed with `./rsp-core-lib-test` or `ctest --preset debug-with-tests|release-with-tests`

## wpa_supplicant tests
To run the wpa_supplicant tests, the /etc/wpa_supplicant/wpa_supplicant.conf file must be present and contain the following:
```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
```
On newer Debian-based systems, systemd controls wpa_supplicant, which is started without reading the config file.
During the test, stop the service with `sudo systemctl stop wpa_supplicant`, and start wpa_suuplicant manually with
```sh
sudo wpa_supplicant -B -i <wireless device> -c /etc/wpa_supplicant/wpa_supplicant.conf
```

## Versioning

This package follows [Semantic Versioning 2.0.0](http://semver.org/)

## License

[MPL-2.0](https://spdx.org/licenses/MPL-2.0.html), please read the [`LICENSE`](./LICENSE) file included in this project.

### Development

For developers working on this project, please ensure that all source files include the following header:

```txt
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

**Note:** _To add `\license` tag to doxygen, please see [stackoverflow answer](https://stackoverflow.com/questions/12353409/doxygen-and-license-copyright-informations)._

﻿﻿Embedded Template Library (ETL)
-------------------------

[![Release date](https://img.shields.io/github/release-date/jwellbelove/etl?color=%231182c3)](https://img.shields.io/github/release-date/jwellbelove/etl?color=%231182c3)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/jwellbelove/etl)
[![Standard](https://img.shields.io/badge/c%2B%2B-98/03/11/14/17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Sponsors](https://img.shields.io/github/sponsors/ETLCPP)](https://img.shields.io/github/sponsors/ETLCPP)

![CI](https://github.com/ETLCPP/etl/workflows/vs2019/badge.svg)
![CI](https://github.com/ETLCPP/etl/workflows/gcc/badge.svg)
![CI](https://github.com/ETLCPP/etl/workflows/clang/badge.svg)
[![Build status](https://ci.appveyor.com/api/projects/status/b7jgecv7unqjw4u0/branch/master?svg=true)](https://ci.appveyor.com/project/jwellbelove/etl/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3c14cd918ccf40008d0bcd7b083d5946)](https://www.codacy.com/manual/jwellbelove/etl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ETLCPP/etl&amp;utm_campaign=Badge_Grade)

**This repository is an Arduino compatible clone of the Embedded Template Library (ETL)**

https://github.com/ETLCPP/etl

See (https://www.etlcpp.com/arduino.html) for up-to-date information.

If you already have the main ETL library from GitHub, you can generate the Arduino compatible variant by running a python script.

You must ensure that you have a directory named ```etl-arduino``` in the same parent directory as ```etl```

Change directory to ```etl/scripts``` and run the python script ```update_releases.py```

This will copy the ETL files into ```etl-arduino``` with a directory structure compatible with the Arduino library system.

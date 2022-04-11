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

To update this repository locally on your machine:

- start from a "master folder", for example in my case in the following example: ```~/Desktop/Jean/Git```.
- from this "master folder", have a clone of both the etl repository (https://github.com/ETLCPP/etl or a fork of it), and the etl-arduino repository (https://github.com/ETLCPP/etl-arduino or a fork of it).
- from the etl-arduino repository, run the etl repository ```arduino/create_arduino_library.py``` script.

For example, on my machine:

```
jj@MBX20:~/Desktop/Jean/Git$ ls -d etl*
etl  etl-arduino
jj@MBX20:~/Desktop/Jean/Git$ cd etl-arduino/
jj@MBX20:~/Desktop/Jean/Git/etl-arduino$ python3 ../etl/arduino/create_arduino_library.py 

Copy ETL files to the etl-arduino repository

arduino_dir         =  ../etl/arduino
etl_dir             =  ../etl
include_dir         =  ../etl/include
common_dir          =  ..
etl_arduino_dir     =  ../etl-arduino
etl_arduino_src_dir =  ../etl-arduino/src

Copy the library properties
  From : ../etl/library.properties
  To   : ../etl-arduino/library.properties

Copy the Arduino ETL header
  From : ../etl/arduino/Embedded_Template_Library.h
  To   : ../etl-arduino/src/Embedded_Template_Library.h

Copy the ETL headers
  From : ../etl/include
  To   : ../etl-arduino/src
```

At this point, you will then get the latest arduino-etl derived from the etl repo.


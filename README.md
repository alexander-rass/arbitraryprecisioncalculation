
# arbitraryprecisioncalculation<a name="arbitraryprecisioncalculation"></a>
Arbitrary precision calculation is a library containing trigonometric and other functions of floating point numbers with arbitrary precision using gmp library.

## Table of Contents<a name="table-of-contents"></a>
* [arbitraryprecisioncalculation](#arbitraryprecisioncalculation)
* [Table of Contents](#table-of-contents)
* [Features](#features)
* [Download](#download)
* [Installation Instructions](#installation-instructions)
  * [Installation on Windows](#installation-on-windows)
  * [Installation on a Linux operating system](#installation-on-a-linux-operating-system)
* [Examples](#examples)
* [Documentation](#documentation)
* [Feedback](#feedback)
* [License](#license)

## Features<a name="features"></a>

This library extends the gmp library to use many additional functions.

* Trigonometric functions (sin, cos, tan, arccos, ...)
* exponential functions (exp, log, ...)
* and more (pow, randomize, ...)

Additionally vector wise versions of some functions with two operands and some additional functions only working on vectors (orthogonal projection).

It is recommended to use only this library for calculations because it includes some kind of memory management.
Therefore using gmp functions directly on values received from the library or/and the other way around can cause unexpected behaviour!

## Download<a name="download"></a>

The source code and all further data can be downloaded from the
[github-repository](https://github.com/alexander-rass/arbitraryprecisioncalculation/).

## Installation Instructions<a name="installation-instructions"></a>

### Installation on Windows<a name="installation-on-windows"></a>

* Install Cygwin with components gcc, m4 and make
* Install the GMP library
  * Download and unzip the GMP sources and switch to that folder.
    * For example: download `gmp-x.x.x.tar.xz` from https://gmplib.org/
    * unzip the file with `tar -xJf gmp-x.x.x.tar.xz`
    * go to that folder with `cd gmp-x.x.x`
  * Execute `./configure`
  * Execute `make`
  * Execute `make install`
  * It might be necessary to copy the library `libgmp.a` and `libgmp.la` from
    `/usr/local/lib` to `/lib` to ensure that it is recognized.
* [Download](https://github.com/alexander-rass/arbitraryprecisioncalculation/) and unzip the High Precision PSO sources and switch to that folder.
* Execute `make` to generate the library file (`lib/libarbitraryprecisioncalculation.a`).
* The library file can be moved to any location.
* `.h` files can be found in the `src` folder.

### Installation on a Linux operating system<a name="installation-on-a-linux-operating-system"></a>

* Install g++ compiler, and make (if not already present).

  On systems like Ubuntu you can install them by the command
  `sudo apt-get install make g++`.
* Install the GMP library
  * On systems like Ubuntu you can install the GMP library by the command
    `sudo apt-get install libgmp-dev`.
  * Alternatively you can install it as described in the Windows section.
* [Download](https://github.com/alexander-rass/arbitraryprecisioncalculation/) and unzip the High Precision PSO sources and switch to that folder.
* Execute `make` to generate the library file (`lib/libarbitraryprecisioncalculation.a`).
* The library file can be moved to any location.
* `.h` files can be found in the `src` folder.


## Examples<a name="examples"></a>

In the [examples folder](https://github.com/alexander-rass/arbitraryprecisioncalculation/tree/master/examples)
some examples can be found.

#### Example 1

The following code of [example1.cpp](https://github.com/alexander-rass/arbitraryprecisioncalculation/blob/master/examples/example1.cpp) displays the limitations of double calculations compared to arbitrary precision calculation:

```cpp
#include <arbitrary_precision_calculation/arbitraryprecisioncalculation.h>
#include <iostream>
#include <string>

int main(){

    double d_1 = 1.0;
    double d_small = 1e-50;

    std::cout << "double calculation:\n";
    std::cout << "((" << d_1 << "+" << d_small << ")-" << d_1 <<")=" << ((d_1 + d_small) -d_1) << "\n";

    arbitraryprecisioncalculation::Configuration::setInitialPrecision(230);
    // 230 is approximately ld(10)*(50+20)
    // 50 decimal digits of needed precision and additional 20 digits to guarantee the precision of the output
    arbitraryprecisioncalculation::Configuration::setPrecisionSafetyMargin(50);
    arbitraryprecisioncalculation::Configuration::setOutputPrecision(20);

    mpf_t* v_1 = arbitraryprecisioncalculation::mpftoperations::ToMpft(d_1);
    mpf_t* v_small = arbitraryprecisioncalculation::mpftoperations::ToMpft(d_small);

    mpf_t* v_sum = arbitraryprecisioncalculation::mpftoperations::Add(v_1, v_small);
    mpf_t* v_result = arbitraryprecisioncalculation::mpftoperations::Subtract(v_sum, v_1);

    std::string s_1 = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_1);
    std::string s_small = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_small);
    std::string s_sum = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_sum);
    std::string s_result = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_result);

    std::cout << "mpf_t calculation:\n";
    std::cout << "(((" << s_1 << ")+(" << s_small << "))-(" << s_1 <<"))=(" << s_sum << ")-(" << s_1 << ")=" << s_result << "\n";
    std::cout << "The non precise value of v_small depends on the non precise conversion from 1e-50 to double not from double to mpf_t!!!\n";

    // After the last use of variable the value should be released.
    // This is actually not really necessary at the end of a program.
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_1);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_small);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_sum);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_result);
}
```
This file can be compiled by the following command if you are in the examples folder of the downloaded repository:

`g++ -I ../src/ -L./../lib example1.cpp -larbitraryprecisioncalculation -lgmp -o example1`

To compile your source file you need to need to specify the path to the header files (`-I ../src`)
and the location of the library file (`-L./../lib`) and specify the used libraries (`-larbitraryprecisioncalculation -lgmp`).
If you are in another current/working directory and/or if you move the library and/or header files to another location then paths may vary or may not be necessary.

The execution of it produces the following output:
```
$ ./example1 
double calculation:
((1+1e-50)-1)=0
mpf_t calculation:
(((1.00000000000000000000e+00)+(1.00000000000000000762e-50))-(1.00000000000000000000e+00))=(1.00000000000000000000e+00)-(1.00000000000000000000e+00)=1.00000000000000000762e-50
The non precise value of v_small depends on the non precise conversion from 1e-50 to double not from double to mpf_t!!!
```

## Documentation<a name="documentation"></a>

The documentation including an API can be found on
the related [github-pages](https://alexander-rass.github.io/arbitraryprecisioncalculation/) of this
repository. It is generated by doxygen. For customizing the output of doxygen
some files from the
[doxygen-bootstrapped](https://github.com/Velron/doxygen-bootstrapped)
repository of [Velron](https://github.com/Velron) are used. It is added as a
submodule and has an
[Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).

## Feedback<a name="feedback"></a>

Please feel free to contact Alexander Raß (Alexander.Rass@fau.de). Feedback or
a notice that you use this program is highly appreciated. We will also support
you in case you have problems during installation or usage of this program.

## License<a name="license"></a>

The MIT License (MIT)

Copyright (c) 2016 by Friedrich-Alexander-Universität Erlangen-Nürnberg and
Alexander Raß

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See also the
[license file](https://github.com/alexander-rass/arbitraryprecisioncalculation/blob/master/LICENSE).


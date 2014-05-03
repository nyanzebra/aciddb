AcidDB
======

AcidDB is an ACID-compliant client/server database in the form of an in-memory key/value store designed and built over a semester at Auburn University's for the 3700 Software Modeling and Design class.

AcidDB is released under the MIT license in hopes that the source code becomes useful to third parties.

Boost Build
===========

This project relies on the [Boost Build 2](http://www.boost.org/boost-build2/), or BJam, to build and so bjam or b2 must be present on your system. The remainder of the compilation process is self-contained.

Boost Build on Mac OS X
-----------------------

We recommend using a package manager such as [Homebrew](http://brew.sh/), however any installation of Boost Build that puts bjam or b2 into your path is fine.

If you're using homebrew, the installation of Boost Build is simple:

    brew install boost-build

Boost Build on Linux
--------------------

Use your favorite package manager to install boost build. On Ubuntu, it's found in APT under bjam:

    sudo apt-get install bjam

Building AcidDB
===============

Clone this repo *recursively* with submodules to a directory of your choosing, run the `build-boost.sh` script in the root directory to install the relevant boost libraries and headers to the local `deps\lib` and `deps\include` directories. In a nutshell, execute the following commands:

    git clone --recursive git@github.com:vmrob/aciddb.git
    cd aciddb
    ./build-boost.sh
    bjam
RIP
===

Encode WAV into MP3

Copyright &copy; 2016 Bernd Doser - All rights reserved.

Requirements
------------

- cmake (version >= 3.0, http://www.cmake.org)
- lame (tested version 3.99.5)

For Ubuntu 14.04 please install following packages:

    sudo apt-get install cmake libmp3lame-dev

Compilation for Linux
---------------------

- Compile rip

    cmake ..
    make -j <number of cores>

Cross compilation on Linux for Windows
--------------------------------------

- Compile lame for minwg

    wget http://downloads.sourceforge.net/project/lame/lame/3.99/lame-3.99.5.tar.gz
    tar xzf lame-3.99.5.tar.gz
    cd lame-3.99.5
    ./configure --host=x86_64-w64-mingw32 --prefix=<path> --disable-shared --enable-static --enable-nasm
    make -j <number of cores>

- Compile rip

    sudo apt-get install mingw-w64
    cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw64.cmake ..
    make -j <number of cores>

Usage
-----

    rip [path to wav files]


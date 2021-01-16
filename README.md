# Infinity

Infinity is a role-playing game for the Game Boy Color handheld game console, developed by Affinix Software primarily between the years 1999 and 2001. The game is incomplete and should be considered alpha/preview quality. See the [Infinity website](http://infinity-gbc.org) for the full announcement regarding the project release as well as a downloadable ROM file.

## License

This project is licensed under the [Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License](https://creativecommons.org/licenses/by-nc-nd/4.0/) and the [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-nc-sa/4.0/), with different parts covered by each license, and with certain exceptions to allow the parts to be combined. COMMERCIAL USE PROHIBITED. See the LICENSE file for details.

For commercial inquiries, please contact justin@karneges.com.

Additionally, any programming source files for the main game program, as well as for any utility programs, are licensed under the GNU General Public License, Version 3 or any later version. This includes files written in C and assembly language, but does not include event programming files. Such source files that are additionally covered by the GPL are marked with an appropriate license header.

## Building

Building the game requires a unix-like environment, GBDK, Yasm, as well as custom build utilities that you'll need to compile. On Windows, you can use [MSYS2](https://www.msys2.org/).

Clone our GBDK repository, which contains tweaks for it to build on modern systems. Build and install it somewhere. Below installs to `/home/user/gbdk/sdk`:

```sh
git clone git://github.com/infinity-gbc/gbdk.git
cd gbdk
GBDK_PATH=~/gbdk
make SDK_DIR=$GBDK_PATH
make SDK_DIR=$GBDK_PATH install
```

Patch and build the GBDK library:

```sh
cd $GBDK_PATH/sdk/gbz80-gb/2.1.5
patch -p0 < $INFINITY/tools/gbdk_infinity.diff
cd lib
make
```

Put the compiler in your `$PATH`:

```sh
export PATH=$PATH:$GBDK_PATH/sdk/gbz80-gb/2.1.5/bin
```

Clone the Infinity repository:

```sh
git clone https://github.com/infinity-gbc/infinity.git
cd infinity
INFINITY=`pwd`
```

Build the toolchain:

```sh
cd $INFINITY
cd tools/toolchain
make
cd ../../
```

Build the game!

```sh
make
```

If all goes well, the game will be built as `build/j.gb`.

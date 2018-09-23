# Infinity

Infinity is a role-playing game for the Game Boy Color handheld game console, developed by Affinix Software primarily between the years 1999 and 2001. The game is incomplete and should be considered alpha/preview quality. See the [Affinix website](http://affinix.com) for the full announcement regarding the project release as well as a downloadable ROM file.

## License

This project is licensed under the [Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License](https://creativecommons.org/licenses/by-nc-nd/4.0/) and the [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-nc-sa/4.0/), with different parts covered by each license, and with certain exceptions to allow the parts to be combined. COMMERCIAL USE PROHIBITED. See the LICENSE file for details.

For commercial inquiries, please contact support@affinix.com.

Additionally, any programming source files for the main game program, as well as for any utility programs, are licensed under the GNU General Public License, Version 3 or any later version. This includes files written in C and assembly language, but does not include event programming files. Such source files that are additionally covered by the GPL are marked with an appropriate license header.

## Building

Currently the game only builds on a Windows host. Building the game requires GBDK, TASM, as well as custom build utilities that you'll need to compile.

From within the source tree, run `setup.bat` from a command prompt:

```
cd \path\to\infinity\bin
setup
```

This will create a virtual `J:` drive pointing to the root of the project and change to it.

Download [GBDK 2.1.5](https://sourceforge.net/projects/gbdk/files/gbdk/2.1.5/) (the .zip file that's in there) and unpack into the root of the project as the directory `sdk`.

Download and install [MinGW](http://www.mingw.org/wiki/Getting_Started) if you don't already have it. From a MinGW shell (leave the original command prompt alone, we'll come back to it), patch the GBDK and build the custom build utiltiies:

```sh
cd /j/sdk/gbz80-gb/2-1-5/lib
patch -p0 < tools/gbdk_infinity.diff
cd tools/toolchain
make
```

You can close the MinGW shell now, we won't need it from here on.

Now from the command prompt, build the GBDK library using `make.bat`:

```
cd \sdk\gbz80-gb\2-1-5\lib
make.bat
```

Download [TASM 3.2](http://www.ticalc.org/archives/files/fileinfo/250/25051.html). Unpack and save `tasm.exe` into the project's `bin` directory.

You should now have all the tools ready to build the game. Now go run a bunch of commands:

```
PATH=%PATH%;c:\mingw\bin
cd \resource\eve
itemconv --defs items.ref
copy itemdefs.h ..\..\source\eve
e
d
c
build
cd \resource\ext
build             (press Y at the prompt)
cd \
do
```

If all goes well, the game will be built as `build\j.gb`.

## Building on Linux

**WORK IN PROGRESS**

Build GBDK and install it somewhere. Below installs to `/home/user/tmp/sdk`:

```sh
git clone git://github.com/infinity-gbc/gbdk.git
cd gbdk
make SDK_DIR=/home/user/tmp
make SDK_DIR=/home/user/tmp install
```

Patch the GBDK and build the library:

```sh
cd /home/user/tmp/sdk/gbz80-gb/2.1.5    # the dir with "sdk" in it
patch -p0 < /path/to/infinity/tools/gbdk_infinity.diff
cd sdk/gbz80-gb/2.1.5/lib
make
```

Build the toolchain:

```sh
# (from infinity dir)
cd tools/toolchain
make
```

Put the compiler in your `PATH`:

```sh
export PATH=$PATH:/home/user/tmp/sdk/gbz80-gb/2.1.5/bin
```

Copy all the `.pag` files from a Windows build into the `resource` dir of the infinity dir.

Build the game!

```sh
# (from infinity dir)
make
```

If all goes well, the game will be built as `build/j.gb`.

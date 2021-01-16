#!/bin/bash

gbdk=(2 1 5) # Current GBDK version

# Check if script is running under MinGW
unameOut="$(uname -s)"
case "${unameOut}" in
    MINGW*)     machine="win";;
    *)          machine="unix"
esac

# Pick version string style depending on host platform
if [ $unameOut == "win" ]; then
    version="${gbdk[0]}-${gbdk[1]}-${gbdk[2]}"
else
    version="${gbdk[0]}.${gbdk[1]}.${gbdk[2]}"
fi

# Patch GBDK libraries
cd sdk/gbz80-gb/$version/
patch -p0 < ../../../tools/gbdk_infinity.diff


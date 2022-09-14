# Overview
This document contains the instructions for compiling steam.hdll for various platform.

# Windows
For Windows, there are 2 ways to compile.


## Visual Studio
The first way is to install Visual Studio and using the solution file provided.

- Put your steam sdk in the hlsteam/sdk folder
- Define `HASHLINK_SRC` env var to point to your `hashlink` directory

## Makefile
The second way is to use the Makefile.

To use the makefile, you will need to install a couple of things.

1. Install [MSYS2](https://www.msys2.org/)
2. Via msys, install the toolchain for C++.
    - `pacman -S mingw-w64-x86_64-gcc`
    - `pacman -S mingw-w64-clang-x86_64_toolchain`
    - `pacman -S make`
3. You might also want to install git
    - pacman -S git
4. Download the steam sdk from Steam and copy it to hlsteam/sdk. Make sure that the path is roughly `sdk/redistributable_bin/...`.
5. Download hashlink from [hashlink github](https://github.com/HaxeFoundation/hashlink).
6. Copy `hashlink/include/` to `hlsteam/include/`
7. Make folder `hlsteam/lib` and copy `hashlink/lib/libhl.dll` to `hlsteam/lib/`
8. Run Mingw (installed via the toolchain) and go to the `hlsteam` folder and run `make`.

You will need distribute `libc++.dll` and `libunwind.dll` from `C:/msys64/clang64/bin` with your game.
The path might be different if you have installed msys to another directory.

# Mac
For Mac, we will assume that you have a locally compiled hashlink.

1. Download steam sdk and symlink/copy to `hlsteam/sdk`. Make sure that the path is roughly `sdk/redistributable_bin/...`.
2. Copy `hashlink/src/hl.h` to `hlsteam/include/`
3. Make folder `hlsteam/lib` and copy `hashlink/libhl.dylib` to `hlsteam/lib/`
4. Run `make`

# Linux
For Linux, we will assume that you have a locally compiled hashlink.

1. Download steam sdk and symlink/copy to `hlsteam/sdk`. Make sure that the path is roughly `sdk/redistributable_bin/...`.
2. Copy `hashlink/src/hl.h` to `hlsteam/include/`
3. Make folder `hlsteam/lib` and copy `hashlink/libhl.so` to `hlsteam/lib/`
4. Run `make`

# Common Traps

## Different hl.exe and libhl.dll/so for Steam and Distribution
Ensure that the hashlink version you use to compile steam is the same as the one you distribute with your game.
The following 2 files needs to come from the same source.

- hl.exe (hl for mac and linux)
- libhl.dll/so/dylib

and they need to be the same as the one that you compile steam.hdll with.

## Unable to load library
If you encounter `fail to load steam.hdll`, it means that some libraries that are required by steam is not distributed with the game.

To fix this, you will have to debug which library it is.

If the message is `fail to load ?steam.hdll` with the `?`, it is likely that one of the version of library that you included is not compatible, likely `libhl.hdll`.

### For Windows
Run mingw and go to the folder containing your game.

Rename steam.hdll to steam.dll and run `ldd steam.dll`.
This should tell you which dll is missing and you should be able to find the corresponding dll in `C:/msys64/clang64/bin`.

You should be able to ignore all `api-ms-XXXX` libraries as they are usually present in most windows and they seems to work even if they are not present.

### For Mac
Go to the folder containing your game and run `otool -L steam.hdll`.
This should tell you which shared library is missing.

### For Linux
See Windows, mostly similar except that you can run it in any terminal.

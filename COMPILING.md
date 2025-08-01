# Table of contents

* [Building Russian Doom on Windows](#building-russian-doom-on-windows)
  * [Step 1: Setting up build environment](#step-1-setting-up-build-environment)
    * [1.A: MSYS2 environment](#1a-msys2-environment)
      * [1.A.A: MSYS2 UCRT environment](#1aa-msys2-ucrt-environment)
      * [1.A.B: MSYS2 MinGW environment](#1ab-msys2-mingw-environment)
      * [1.A.C: MSYS2 Clang environment](#1ac-msys2-clang-environment)
    * [1.B: MSVC Visual Studio environment](#1b-msvc-visual-studio-environment)
      * [1.B.A: Providing required dependencies manually](#1ba-providing-required-dependencies-manually)
      * [1.B.B: Providing required dependencies using VCPKG](#1bb-providing-required-dependencies-using-vcpkg)
  * [Step 2: Compiling Russian Doom](#step-2-compiling-russian-doom)
  * [Step 3: Installing Russian Doom](#step-3-installing-russian-doom)
  * [Packaging Russian Doom](#packaging-russian-doom)
* [Building Russian Doom on Linux](#building-russian-doom-on-linux)
  * [Step 1: Setting up build environment](#step-1-setting-up-build-environment-1)
  * [Step 2: Compiling Russian Doom](#step-2-compiling-russian-doom-1)
  * [Step 3: Installing Russian Doom](#step-3-installing-russian-doom-1)
  * [Packaging Russian Doom](#packaging-russian-doom-1)

# Building Russian Doom on Windows

## Step 1: Setting up build environment

Download the source code archive of the latest release version and unpack it.
Or clone repository from Git.
The primary way of building Russian Doom on Windows is using an MSYS UCRT64 environment for x86_64
and using MSVC for x86.

### 1.A: MSYS2 environment

Download and install [MSYS2](https://www.msys2.org/).
For configuring and building Russian Doom,
you **must** use the appropriate MSYS2 terminal
(**ucrt64.exe**, **mingw64.exe**, **clang64.exe**)
so that the $PATH environment variable points to the proper toolchain.

### **1.A.A: MSYS2 UCRT environment**

For the **mingw-w64-ucrt-x86_64** toolchain (**64**-bit), use the **ucrt64.exe** terminal.

Open the terminal and install dependencies using the following command:
```shell
pacman -S mingw-w64-ucrt-x86_64-{pkgconf,gcc,make,cmake,SDL2{,_net,_mixer},libsamplerate} \
       git,python
```

If `python` version from MSYS2 is not working on your OS,
you can install a working version of [Python](https://www.python.org/downloads/) to the host system.
Python is completely optional if you will be using 'Selected DLL set'.

If you want to use `Ninja` or `Ninja Multi-Config` cmake generators, you need
to install `ninja` by the following command:
```shell
pacman -S mingw-w64-ucrt-x86_64-ninja
```

If you want to run CTest tests, you must also install `gdb` by the following command:
```shell
pacman -S mingw-w64-ucrt-x86_64-gdb
```

### **1.A.B: MSYS2 MinGW environment**

For the **mingw-w64-x86_64** toolchain, use the **mingw64.exe** terminal.

Open the terminal and install dependencies using the following command:
```shell
pacman -S mingw-w64-x86_64-{pkgconf,gcc,make,cmake,SDL2{,_net,_mixer},libsamplerate} \
       git,python
```

If `python` version from MSYS2 is not working on your OS,
you can install a working version of [Python](https://www.python.org/downloads/) to the host system.
Python is completely optional if you will be using 'Selected DLL set'.

If you want to use `Ninja` or `Ninja Multi-Config` cmake generators, you need
to install `ninja` by the following command:
```shell
pacman -S mingw-w64-x86_64-ninja
```

If you want to run CTest tests, you must also install `gdb` by the following command:
```shell
pacman -S mingw-w64-x86_64-gdb
```

### **1.A.C: MSYS2 Clang environment**

For the **mingw-w64-clang-x86_64** toolchain, use the **clang64.exe** terminal.

Open the terminal and install dependencies using the following command:
```shell
pacman -S mingw-w64-clang-x86_64-{pkgconf,clang,make,cmake,SDL2{,_net,_mixer},libsamplerate} \
       git,python
```

If `python` version from MSYS2 is not working on your OS,
you can install a working version of [Python](https://www.python.org/downloads/) to the host system.
Python is completely optional if you will be using 'Selected DLL set'.

If you want to use `Ninja` or `Ninja Multi-Config` cmake generators, you need
to install `ninja` by the following command:
```shell
pacman -S mingw-w64-clang-x86_64-ninja
```

If you want to run CTest tests, you must also install `gdb` by the following command:
```shell
pacman -S mingw-w64-clang-x86_64-gdb
```

### 1.B: MSVC Visual Studio environment

Download and install [Cmake](https://cmake.org/download/#latest) and
[Visual Studio](https://visualstudio.microsoft.com/downloads/) or Build tools.

If you want to build Windows XP compatible executables, you need to install `v141_xp` toolset.
In the GUI installer it can be easily found on the "Individual components" tab by typing "XP" in the search field.
Or you can run the following command after installing base components:
```shell
"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer" modify \
    --installPath "<path to install>" \
    --add Microsoft.VisualStudio.Component.WinXP \
    --nocache --norestart --quiet
```
**Note:** VS Installer can reboot your computer without asking,
and after reboot you have to run installer again and continue the installation process.

When using `Visual Studio *` cmake generators,
you need to specify target architecture by passing `-A <arch>` parameter to cmake:
`Win32` for **32**-bit and `x64` for **64**-bit.
For Windows XP compatible build you also need to pass `-T v141_xp` parameter.

You also need to provide development libraries, [manually](#1ba-providing-required-dependencies-manually)
or using [vcpkg](#1bb-providing-required-dependencies-using-vcpkg) (Windows 7+ host and Visual Studio 2015.3+).

You can use any terminal for the following steps.

### **1.B.A: Providing required dependencies manually**

Download and unpack development windows libraries for
[SDL2](https://github.com/libsdl-org/SDL/releases),
[SDL_mixer](https://github.com/libsdl-org/SDL_mixer/releases),
[SDL_net](https://github.com/libsdl-org/SDL_net/releases)
and optionally for
[libsamplerate](https://github.com/libsndfile/libsamplerate/releases) and
[miniz](https://github.com/richgel999/miniz/releases)
(can be ignored because there is a provided one).
Then pass the following parameters to cmake during configuration time:
```shell
-D SDL2_DIR="<path to SDL2>"
-D SDL2_MIXER_DIR="<path to SDL_mixer>"
-D SDL2_NET_DIR="<path to SDL_net>"
# Optionally:
-D SAMPLERATE_DIR="<path to libsamplerate>"
-D MINIZ_DIR="<path to miniz>"
```

Cmake option `CMAKE_FIND_PACKAGE_PREFER_CONFIG` should be `OFF` (default) in order for `*_DIR` variables to work.

**You also need to** install [Python](https://www.python.org/downloads/) (**Recommended**)
or use Cmake 3.16+ (Can't update Dlls),
**in order for automatic installation of DLLs to work.**

### **1.B.B: Providing required dependencies using VCPKG**

Install and bootstrap [vcpkg](https://github.com/microsoft/vcpkg#quick-start-windows).

By default, Russian Doom uses the manifest mode of vcpkg.
In this mode, all dependencies are obtained automatically during the project configuration stage.
If you want to use vcpkg in the classic mode, use the following commands to download and build dependencies:
```shell
vcpkg integrate install
vcpkg install --triplet=x86-windows sdl2 sdl2-mixer[libflac,libmodplug,mpg123,opusfile] sdl2-net libsamplerate
vcpkg install --triplet=x64-windows sdl2 sdl2-mixer[libflac,libmodplug,mpg123,opusfile] sdl2-net libsamplerate
```

Then, at the configuration step, add the following parameters to cmake:
```shell
-D CMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
# And for classic mode:
-D VCPKG_MANIFEST_MODE="OFF"
```

## Step 2: Compiling Russian Doom

There are several supported cmake generators: `MinGW Makefiles`, `MSYS Makefiles`, `Ninja`, `Ninja Multi-Config`,
`Visual Studio 16 2019`, `Visual Studio 17 2022`.

To configure the project, use the following command:
```shell
cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE="Release" -S . -B build
```
Available build types are `Release`, `Debug`, `RelWithDebInfo`, `MinSizeRel`.

For Visual Studio with vcpkg, you can use this command:
```shell
cmake -G "Visual Studio 17 2022" -A x64 \
-D CMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" \
-D CMAKE_BUILD_TYPE="Release" -S . -B build
```

To use configs provided by the installed libraries (for example, in the MSYS environment) instead of find-modules,
set the `CMAKE_FIND_PACKAGE_PREFER_CONFIG` cmake option to `ON`.

Russian Doom uses Link Time Optimization if it is available in the compiler.
If it causes problems with your compiler, set `RD_ENABLE_LTO` cmake option to `OFF`.

If you are using MSYS2 (or any other MinGW environment), by default DLLs from 'Selected DLL set'
will be used instead of DLLs provided by the build environment.
To use DLLs provided by the build environment, set the `RD_USE_SELECTED_DLL_SET` cmake option to `OFF`.
You will need `python` to be installed (**Recommended**) or use Cmake 3.16+ (Can't update Dlls).

To build the project, use the following command:
```shell
cmake --build build
```

After successful compilation, the resulting binaries can be found in the `build\src\` folder or,
if you used `Visual Studio *` or `Ninja Multi-Config` generators, in `build\src\<config>\` folder.

## Step 3: Installing Russian Doom

To install Russian Doom to `<install directory>` use the following command:
```shell
cmake --install build --prefix <install directory>
```
If you want to install only one game, you can use the following command:
```shell
cmake --install build --prefix <install directory> --component <game>
```
Available components are `doom`, `heretic`, `hexen`.

## Packaging Russian Doom

Packaging is performed using Cpack.

To package Russian Doom, run the following commands:
```shell
cd build
cpack -G ZIP
```
The resulting package of the previously compiled components and its checksum can be found in the `build` directory.

The only supported generators are [Archive Generators](https://cmake.org/cmake/help/latest/cpack_gen/archive.html).

# Building Russian Doom on Linux

Compiling on Linux is rather simple. 

## Step 1: Setting up build environment

First, make sure you have all the necessary dependencies for compilation.
On Ubuntu Linux, they can be installed by the following command:
```shell
sudo apt install gcc make python3 gzip cmake \
libsdl2-dev libsdl2-mixer-dev libsdl2-net-dev libsamplerate-dev
```
On Fedora Linux, they can be installed by the following command:
```shell
sudo dnf install gcc make python3 gzip cmake \
SDL2-devel SDL2_mixer-devel SDL2_net-devel libsamplerate-devel miniz-devel
```
On Manjaro Linux, they can be installed by the following command:
```shell
sudo pacman -S gcc make python3 gzip cmake \
sdl2 sdl2_mixer sdl2_net libsamplerate
```

The `gcc` compiler can de replaced with `clang`.
If you want to use `Ninja` or `Ninja Multi-Config` cmake generators, you need to install `ninja-build`.
The `python3` and `gzip` is not mandatory, but required for generation of man pages and bash completion.
If you want to run CTest tests, you must also install `gdb`.
If your Linux distribution provides a package for `miniz` library, you can use it as a dependency instead of the bundled one.

## Step 2: Compiling Russian Doom

Download the source code archive of the latest release version and unpack it.
Or clone repository from Git.

There are several supported cmake generators: `Unix Makefiles`, `Ninja`, `Ninja Multi-Config`.

To configure the project, use the following command:
```shell
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE="Release" -S . -B build
```
Available build types are `Release`, `Debug`, `RelWithDebInfo`, `MinSizeRel`.

To use configs provided by the installed libraries instead of find-modules,
set the `CMAKE_FIND_PACKAGE_PREFER_CONFIG` cmake option to `ON`.

Russian Doom uses Link Time Optimization if it is available in the compiler.
If it causes problems with your compiler, set `RD_ENABLE_LTO` cmake option to `OFF`.

To build the project, use the following command:
```shell
cmake --build build
```
After successful compilation, the resulting binaries can be found in the `build/src/` folder.

## Step 3: Installing Russian Doom

To install Russian Doom, use the following command:
```shell
sudo cmake --install build
```
If you want to install only one game, you can use the following command:
```shell
sudo cmake --install build --component <game>
```
Available components are `doom`, `heretic`, `hexen`. 

For the portable version, you probably want to use `--prefix <install directory>` parameter to set install directory.
Note that config files and savegames will be stored in that directory.

## Packaging Russian Doom

Packaging is performed using the standard process for your distribution.

If you are packaging a non-portable version, during configuration, 
you should set the value of the `CMAKE_INSTALL_PREFIX` cmake variable to match the installation path of your package.
By default, it is `/usr/local` and you probably want to change it to `/usr`.
You can then use `cmake --install build --prefix <path>` command to collect all distribution files in the `<path>`
location to prepare them for packaging.

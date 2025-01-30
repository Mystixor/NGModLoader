# NGModLoader
Mod Loader for the Ninja Gaiden series on PC. So far only Ninja Gaiden 2 Black (Steam, version 1.0.5.0) is supported.

# Disclaimer
The author of this repository has no affiliation with Team Ninja or Koei Tecmo and this software is intended purely for educational or entertainment purposes. The author does not claim ownership of the file formats that may be modifiable through use of the software, and no copyright infringement is intended. No game files will be distributed. All rights go to their respective owners. If you are the rights holder and would like us to remove this content, please contact us and we will comply with your request immediately.

# Usage
## Ninja Gaiden 2 Black
0. Download the required files from the [Releases](https://github.com/Mystixor/NGModLoader/releases) section here on Github. For the average user the `Release` version should be optimal, but if you want to see some potential debug information you can opt for the `Debug` version.
1. Put `d3d11.dll` in the directory `NINJAGAIDEN2BLACK\NINJAGAIDEN2BLACK\Binaries\Win64\`.
2. Create a folder `mods` in the directory `NINJAGAIDEN2BLACK\NINJAGAIDEN2BLACK\Binaries\Win64\`.
3. Put your modded files into the `mods` folder. Mods must follow the naming scheme of e.g. `00004.dat` or `07655.dat` (five-digit number, padded with zeros, `.dat` extension).
4. Create a folder `bin` inside the `mods` folder. This is where DLL-based mods can be put, which will then be injected on game startup. The directory layout is `mods\bin\[MOD NAME]\main.dll`.

The Mod Loader will now automatically activate when you launch the game, to disable it simply remove `d3d11.dll` from the game directory and restart the game.

# Building
For the average user I recommend downloading the prebuilt binaries available in the [Releases](https://github.com/Mystixor/NGModLoader/releases) section. If you still want to build the project yourself:
1. Open the solution `Detours\vc\Detours.sln` and build the library. I recommend the `x64` platform in the `ReleaseMD` configuration, but other options might work as well depending on your hardware (untested).
2. Open the solution `ModLoader.sln` and build the `ModLoader` project. The `d3d11.dll` will be created in the `x64` directory (`Debug` or `Release` subdirectory depending on your selected configuration).

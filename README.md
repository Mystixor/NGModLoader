# NGModLoader
Mod Loader for the Ninja Gaiden series on PC. So far only Ninja Gaiden 2 Black (Steam, version 1.0.5.0) is supported.

# Disclaimer
The author of this repository has no affiliation with Team Ninja or Koei Tecmo and this software is intended purely for educational or entertainment purposes. The author does not claim ownership of the file formats that may be modifiable through use of the software, and no copyright infringement is intended. No game files will be distributed. All rights go to their respective owners. If you are the rights holder and would like us to remove this content, please contact us and we will comply with your request immediately.

# Usage
## Ninja Gaiden 2 Black
0. Download the required files from the [Releases](https://github.com/Mystixor/NGModLoader/releases) section here on Github. For the average user the `Release` version should be optimal, but if you want to see some potential debug information you can opt for the `Debug` version.
1. Put `modloader.dll` in the directory `NINJAGAIDEN2BLACK\NINJAGAIDEN2BLACK\Binaries\Win64\`.
2. Create a folder `mods` in the directory `NINJAGAIDEN2BLACK\NINJAGAIDEN2BLACK\Binaries\Win64\`.
3. Put your modded files into the `mods` folder. Mods must follow the naming scheme of e.g. `00004.dat` or `07655.dat` (five-digit number, padded with zeros, `.dat` extension).
4. Boot up the game. As soon as you see the fullscreen game window appear, execute `Injector.exe`. The Mod Loader is now active until you close the game!

There is no rush on how quickly you execute the injector, you can do it any point while the game is running. Keep in mind though that some central configuration files are loaded in the first moments after the game launches, and the Mod Launcher is only able to mod these if you run it early enough.

# Building
For the average user I recommend downloading the prebuilt binaries available in the [Releases](https://github.com/Mystixor/NGModLoader/releases) section. If you still want to build the project yourself:
1. Open the solution `Detours\vc\Detours.sln` and build the library. I recommend the `x64` platform in the `ReleaseMD` configuration, but other options might work as well depending on your hardware (untested).
2. Open the solution `ModLoader.sln` and build both the `Injector` and the `ModLoader` projects. The `modloader.dll` and `Injector.exe` will be created in the `x64` directory (`Debug` or `Release` subdirectory depending on your selected configuration).

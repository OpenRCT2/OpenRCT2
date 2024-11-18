

<p align="center">
  <a href="https://openrct2.io">
    <img src="https://raw.githubusercontent.com/OpenRCT2/OpenRCT2/develop/resources/logo/icon_x128.png" style="width: 128px;" alt="OpenRCT2 logo"/>
  </a>
</p>

<h1 align="center">OpenRCT2</h1>

<h3 align="center">An open-source re-implementation of RollerCoaster Tycoon 2, a construction and management simulation video game that simulates amusement park management.</h3>

---

![OpenRCT2.org Group Park 5](https://i.imgur.com/e7CK5Sc.png)

---

### Download
| Latest release                                                                                                        | Latest development build |
|-----------------------------------------------------------------------------------------------------------------------|--------------------------|
| [![OpenRCT2.org](https://img.shields.io/badge/master-v0.4.16-green.svg)](https://openrct2.org/downloads/master/latest) | [![OpenRCT2.org](https://img.shields.io/github/last-commit/OpenRCT2/OpenRCT2/develop)](https://openrct2.org/downloads/develop/latest) |

---

### Chat
Chat takes place on Discord. You will need to create a Discord account if you don't yet have one.

If you want to help *make* the game, join the developer channel.

If you need help, want to talk to the developers, or just want to stay up to date then join the non-developer channel for your language.

If you want to help translate the game to your language, please stop by the Localisation channel.

| Language | Non Developer | Developer | Localisation | Asset Replacement |
| -------- | ------------- | --------- | ------------ | ----------------- |
| English | [![Discord](https://img.shields.io/badge/discord-%23openrct2--talk-blue.svg)](https://discord.gg/ZXZd8D8) </br> [![Discord](https://img.shields.io/badge/discord-%23help-blue.svg)](https://discord.gg/vJABqGGTEt) | [![Discord](https://img.shields.io/badge/discord-%23development-yellowgreen.svg)](https://discord.gg/fsEwSWs) | [![Discord](https://img.shields.io/badge/discord-%23localisation-green.svg)](https://discord.gg/sxnrvX9) | [![Discord](https://img.shields.io/badge/discord-%23open--graphics-b00b69.svg)](https://discord.gg/aM2Pchscnp) </br> [![Discord](https://img.shields.io/badge/discord-%23open--sound--and--music-b00b69.svg)](https://discord.gg/tuz3QBBWJf)
| Nederlands | [![Discord](https://img.shields.io/badge/discord-%23nederlands-orange.svg)](https://discord.gg/cQYSXzW) | | |

---

# Contents
- 1 - [Introduction](#1-introduction)
- 2 - [Downloading the game (pre-built)](#2-downloading-the-game-pre-built)
- 3 - [Building the game](#3-building-the-game)
  - 3.1 - [Building prerequisites](#31-building-prerequisites)
  - 3.2 - [Compiling and running](#32-compiling-and-running)
- 4 - [Contributing](#4-contributing)
  - 4.1 - [Bug fixes](#41-bug-fixes)
  - 4.2 - [New features](#42-new-features)
  - 4.3 - [Translation](#43-translation)
  - 4.4 - [Graphics](#44-graphics)
  - 4.5 - [Audio](#45-audio)
  - 4.6 - [Scenarios](#46-scenarios)
- 5 - [Policies](#5-policies)
  - 5.1 - [Code of conduct](#51-code-of-conduct)
  - 5.2 - [Code signing policy](#52-code-signing-policy)
  - 5.3 - [Privacy policy](#53-privacy-policy)
- 5 - [Licence](#6-licence)
- 6 - [More information](#7-more-information)
- 7 - [Sponsors](#8-sponsors)

---

# 1. Introduction

**OpenRCT2** is an open-source re-implementation of RollerCoaster Tycoon 2 (RCT2). The gameplay revolves around building and maintaining an amusement park containing attractions, shops and facilities. The player must try to make a profit and maintain a good park reputation whilst keeping the guests happy. OpenRCT2 allows for both scenario and sandbox play. Scenarios require the player to complete a certain objective in a set time limit whilst sandbox allows the player to build a more flexible park with optionally no restrictions or finance.

RollerCoaster Tycoon 2 was originally written by Chris Sawyer in x86 assembly and is the sequel to RollerCoaster Tycoon. The engine was based on Transport Tycoon, an older game which also has an equivalent open-source project, [OpenTTD](https://openttd.org). OpenRCT2 attempts to provide everything from RCT2 as well as many improvements and additional features, some of these include support for modern platforms, an improved interface, improved guest and staff AI, more editing tools, increased limits, and cooperative multiplayer. It also re-introduces mechanics from RollerCoaster Tycoon that were not present in RollerCoaster Tycoon 2. Some of those include; mountain tool in-game, the *"have fun"* objective, launched coasters (not passing-through the station) and several buttons on the toolbar.

---

# 2. Downloading the game (pre-built)

OpenRCT2 requires original files of RollerCoaster Tycoon 2 to play. It can be bought at either [Steam](https://store.steampowered.com/app/285330/RollerCoaster_Tycoon_2_Triple_Thrill_Pack/) or [GOG.com](https://www.gog.com/game/rollercoaster_tycoon_2). If you have the original RollerCoaster Tycoon and its expansion packs, you can [point OpenRCT2 to these](https://github.com/OpenRCT2/OpenRCT2/wiki/Loading-RCT1-scenarios-and-data) in order to play the original scenarios.

[Our website](https://openrct2.io/download) offers portable builds and installers with the latest versions of the `master` and `develop` branches. There is also a [launcher](https://openrct2.io/download/launcher) available for Windows and Linux that will automatically update your build of the game so that you always have the latest version.

Alternatively to using the launcher, for most Linux distributions, we recommend the [latest Flatpak release](https://flathub.org/apps/details/io.openrct2.OpenRCT2). When downloading from Flathub, you will always receive the latest updates regardless of which Linux distribution you use.

Some Linux distributions offer native packages:
* Arch Linux: [openrct2](https://archlinux.org/packages/extra/x86_64/openrct2/) latest release (`extra` repository) and, alternatively, [openrct2-git](https://aur.archlinux.org/packages/openrct2-git) (AUR)
* Gentoo (main portage tree): [games-simulation/openrct2](https://packages.gentoo.org/packages/games-simulation/openrct2)
* NixOS (`nixos-unstable` channel): [openrct2](https://github.com/NixOS/nixpkgs/blob/master/pkgs/games/openrct2/default.nix)
* openSUSE OBS: [games/openrct2](https://software.opensuse.org/download.html?project=games&package=openrct2)
* Ubuntu PPA (nightly builds): [`develop` branch](https://launchpad.net/~openrct2/+archive/ubuntu/nightly)

Some \*BSD operating systems offer native packages:
* FreeBSD: [games/openrct2](https://www.freshports.org/games/openrct2)
* OpenBSD: [games/openrct2](https://openports.se/games/openrct2)

---

# 3. Building the game

## 3.1 Building prerequisites

OpenRCT2 requires original files of RollerCoaster Tycoon 2 to play. It can be bought at either [Steam](https://store.steampowered.com/app/285330/) or [GOG.com](https://www.gog.com/game/rollercoaster_tycoon_2).

<details>
  <summary>Windows prerequisites</summary>

  There are two toolchain options for building the game on Windows:

  - Visual Studio 2022 (Enterprise / Professional / [Community (Free)](https://www.visualstudio.com/vs/community/))
    - Desktop development with C++
  - MSYS2 MinGW Toolchain
    - The toolchains with supported dependencies are `mingw-x86_64`, `mingw-xi686`, `ucrt-x86_64`, `clang-x86_64`, and `clang-xi686`. Each of these require the `$MINGW_PACKAGE_PREFIX` set.
    - Note that `msys` and `clangarm-64` are lacking packages for some dependencies.

  The projects depends on several libraries to be installed. These can be installed automatically using `msbuild` for Visual Studio builds. See section 3.2 for details.
  For MinGW builds, see the wiki for [the actual package names used in pacman](https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-MSYS2-MinGW).
</details>

<details>
  <summary>macOS prerequisites</summary>

  - Xcode Command Line Tools
  - [Homebrew](https://brew.sh)
  - CMake (available through Homebrew)

</details>

<details>
  <summary>Linux prerequisites</summary>

  - gcc (>= 8.0) or clang (>= 10.0) (for C++20 support)
  - sdl2 (only for UI client)
  - freetype (can be disabled)
  - fontconfig (can be disabled)
  - libzip (>= 1.0)
  - libpng (>= 1.2)
  - speexdsp (only for UI client)
  - curl (only if building with http support)
  - nlohmann-json (>= 3.6.0)
  - openssl (>= 1.0; only if building with multiplayer support)
  - icu (>= 59.0)
  - zlib
  - gl (commonly provided by Mesa or GPU vendors; only for UI client, can be disabled)
  - cmake
  - innoextract (optional runtime dependency; used for GOG installer extraction during setup)

  Refer to https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-Linux#required-packages-general for more information about installing the packages.

</details>

---

## 3.2 Compiling and running
### Windows:
<details>
  <summary>Show instructions</summary>

  1. Check out the repository, this can be done using [GitHub Desktop](https://desktop.github.com) or [other tools](https://help.github.com/articles/which-remote-url-should-i-use)
  2. Open a new Developer Command Prompt for VS 2022
  3. Navigate to the repository (e.g. `cd C:\GitHub\OpenRCT2`)
  4. To build the x64 version, use `msbuild openrct2.proj /t:build /p:platform=x64`
     To build the x86 version, use `msbuild openrct2.proj /t:build /p:platform=Win32`
     To build the Arm64 version, use `msbuild openrct2.proj /t:build /p:platform=arm64`
   
     **Note:** The file `g2.dat` may not be generated on cross-compilation (e.g. building for Arm64 on a x64 machine). In this case `g2.dat` must be copied from a x86/x64 build.
  5. Run the game, `bin\openrct2`

  Once you have ran msbuild once, further development can be done within Visual Studio by opening `openrct2.sln`. Make sure to select the correct target platform for which you ran the build in point #3 (`Win32` for the x86 version, `x64` for the x64 version, `arm64` for the Arm64 version), otherwise the build will fail in Visual Studio.

  Other examples:
  ```
  set platform=x64
  msbuild openrct2.proj /t:clean
  msbuild openrct2.proj /t:rebuild /p:configuration=release
  msbuild openrct2.proj /t:g2
  msbuild openrct2.proj /t:PublishPortable
  ```
</details>

### macOS:
<details>
  <summary>Show instructions</summary>

  CMake can build either a self-contained application bundle, which includes all the necessary game files and dependencies, or it can build a command line version that links against system installed dependencies. CMake will retrieve the dependencies from [Dependencies](https://github.com/OpenRCT2/Dependencies/) automatically. You can build the macOS app using CMake using the following commands:
  ```
  cmake -S . -B build
  cmake --build build --target install
  ```
  Then you can run the game by opening `OpenRCT2.app`

  To build the command line version, you'll need to disable the macOS app bundle:
  ```

  cmake -S . -B build -DMACOS_BUNDLE=off
  cmake --build build
  cmake --build build --target install
  ln -s ../data data
  ```
  Then you can run the game by running `./openrct2`.

  To link against system dependencies instead of letting CMake download the dependencies from [Dependencies](https://github.com/OpenRCT2/Dependencies), add `-DMACOS_USE_DEPENDENCIES=off` to your cmake args.

  Detailed instructions can be found on [Building OpenRCT2 on macOS using CMake](https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-macOS-using-CMake).
</details>

### Linux:
<details>
  <summary>Show instructions</summary>

  The standard CMake build procedure is to install the [required libraries](https://github.com/OpenRCT2/OpenRCT2#linux), then:
  ```
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX=. # set your standard cmake options, e.g. build type here - For example, -DCMAKE_BUILD_TYPE=RelWithDebInfo
  cmake --build build # you can parallelise your build job with e.g. -j 8 or consider using ninja
  cmake --build build --target install # the install target creates all the necessary files in places we expect them
  ```

  You can also use Ninja in place of Make using `-G Ninja` in the first command, if you prefer, see Wiki for details.

  Detailed instructions can be found on [Building OpenRCT2 on Linux](https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-Linux).
  
  Note: the `cmake -S . -B build` syntax is available for CMake >= 3.14. For older versions use:
  ```
  mkdir build
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=. # set your standard cmake options, e.g. build type here - For example, -DCMAKE_BUILD_TYPE=RelWithDebInfo
  cmake --build . --target install
  ```
</details>

---

# 4. Contributing
OpenRCT2 uses the [gitflow workflow](https://www.atlassian.com/git/tutorials/comparing-workflows#gitflow-workflow). If you are implementing a new feature or logic from the original game, please branch off and perform pull requests to ```develop```. If you are fixing a bug for the next release, please branch off and perform pull requests to the correct release branch. ```master``` only contains tagged releases, you should never branch off this.

Please read our [contributing guidelines](https://github.com/OpenRCT2/OpenRCT2/blob/develop/CONTRIBUTING.md) for information.

## 4.1 Bug fixes
A list of bugs can be found on the [issue tracker](https://github.com/OpenRCT2/OpenRCT2/issues). Feel free to work on any bug and submit a pull request to the develop branch with the fix. Mentioning that you intend to fix a bug on the issue will prevent other people from trying as well.

## 4.2 New features
Please talk to the OpenRCT2 team first before starting to develop a new feature. We may already have plans for or reasons against something that you'd like to work on. Therefore contacting us will allow us to help you or prevent you from wasting any time. You can talk to us via Discord, see links at the top of this page.

## 4.3 Translation
You can translate the game into other languages by editing the language files in ```data/language``` directory. Please join discussions in the [#localisation channel on Discord](https://discordapp.com/invite/sxnrvX9) and submit pull requests to [OpenRCT2/Localisation](https://github.com/OpenRCT2/Localisation).

## 4.4 Graphics
You can help create new graphics for the game by visiting the [OpenGraphics project](https://github.com/OpenRCT2/OpenGraphics). 3D modellers needed!

## 4.5 Audio
You can help create the music and sound effects for the game. Check out the OpenMusic repository and drop by our [#open-sound-and-music channel on Discord](https://discord.gg/9y8WbcX) to find out more.

## 4.6 Scenarios
We would also like to distribute additional scenarios with the game, when the time comes. For that, we need talented scenario makers! Check out the [OpenScenarios repository](https://github.com/PFCKrutonium/OpenRCT2-OpenScenarios).

---

# 5. Policies

## 5.1 Code of Conduct

We have a [Code of Conduct](CODE_OF_CONDUCT.md) that applies to all OpenRCT2 projects. Please read it.

## 5.2 Code signing policy

We sign our releases with a digital certificate provided by SignPath Foundation.

Free code signing provided by [SignPath.io](https://about.signpath.io/), certificate by [SignPath Foundation](https://signpath.org/).

Signed releases can only be done by member of the [development team](https://github.com/OpenRCT2/OpenRCT2/blob/develop/contributors.md#development-team).

## 5.3 Privacy policy

See [PRIVACY.md](PRIVACY.md) for more information.

---

# 6. Licence
**OpenRCT2** is licensed under the GNU General Public License version 3 or (at your option) any later version. See the [`licence.txt`](licence.txt) file for more details.

---

# 7. More information
- [GitHub](https://github.com/OpenRCT2/OpenRCT2)
- [OpenRCT2.org](https://openrct2.org)
- [Forums](https://openrct2.org/forums/)
- [Facebook](https://www.facebook.com/OpenRCT2)
- [RCT subreddit](https://www.reddit.com/r/rct/)
- [OpenRCT2 subreddit](https://www.reddit.com/r/openrct2/)

## Similar Projects

| [OpenLoco](https://github.com/OpenLoco/OpenLoco) | [OpenTTD](https://github.com/OpenTTD/OpenTTD) | [openage](https://github.com/SFTtech/openage) | [OpenRA](https://github.com/OpenRA/OpenRA) |
|:------------------------------------------------:|:----------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------:|
| [![icon_x128](https://user-images.githubusercontent.com/604665/53047651-2c533c00-3493-11e9-911a-1a3540fc1156.png)](https://github.com/OpenLoco/OpenLoco) | [![](https://github.com/OpenTTD/OpenTTD/raw/850d05d24d4768c81d97765204ef2a487dd4972c/media/openttd.128.png)](https://github.com/OpenTTD/OpenTTD) | [![](https://user-images.githubusercontent.com/550290/36507534-4693f354-175a-11e8-93a7-faa0481474fb.png)](https://github.com/SFTtech/openage) | [![](https://raw.githubusercontent.com/OpenRA/OpenRA/bleed/packaging/artwork/ra_128x128.png)](https://github.com/OpenRA/OpenRA) |
| Chris Sawyer's Locomotion | Transport Tycoon Deluxe | Age of Empires 2 | Red Alert |

# 8. Sponsors

Companies that kindly allow us to use their stuff:

| [DigitalOcean](https://www.digitalocean.com/)                                                                                                                     | [JetBrains](https://www.jetbrains.com/)                                                                                                        | [Backtrace](https://backtrace.io/)                                                                                                        | [SignPath](https://signpath.org/)                                                                                  |
|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------|
| [![do_logo_vertical_blue svg](https://user-images.githubusercontent.com/550290/36508276-8b572f0e-175c-11e8-8622-9febbce756b2.png)](https://www.digitalocean.com/) | [![jetbrains](https://user-images.githubusercontent.com/550290/36413299-0e0985ea-161e-11e8-8a01-3ef523b5905b.png)](https://www.jetbrains.com/) | [![backtrace](https://user-images.githubusercontent.com/550290/47113259-d0647680-d258-11e8-97c3-1a2c6bde6d11.png)](https://backtrace.io/) | [![Image](https://github.com/user-attachments/assets/2b5679e0-76a4-4ae7-bb37-a6a507a53466)](https://signpath.org/) |
| Hosting of various services                                                                                                                                       | CLion and other products                                                                                                                       | Minidump uploads and inspection                                                                                                           | Code signing                                                                                                       |

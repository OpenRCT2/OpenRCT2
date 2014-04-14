# OpenRCT2
An open source clone of Roller Coaster Tycoon 2 built by decompiling the original game one bit at a time.
- [Screenshot 1](http://dev.intelorca.co.uk/2014/openrct2/cheats.png)

# Contents
- 1 Introduction
  - 1.1 Background
  - 1.2 Decompiling the game
  - 1.3 Progress
- 2 Building the source code
  - 2.1 Prerequisites
  - 2.2 Compiling and running
- 3 Contributing
  - 3.1 Decompiling
  - 3.2 Naming of procedures and variables
  - 3.3 Cleaning and documenting the source code
  - 3.4 Implementing new featues / fixing bugs
- 4 Licence

# 1 Introduction

## 1.1 Background
**OpenRCT2** is an attempt to decompile RollerCoaster Tycoon 2 into C. RollerCoaster Tycoon 2 was originally written in MASM and Visual C++ where functions related to interfacing with the operating system were written in C (supposedly 1%) and the rest of the game in pure x86 assembly. OpenTTD went through the same treatment where Transport Tycoon Deluxe was decompiled into C and then have thousands of more features added to the game. RollerCoaster Tycoon 2 uses the third version of Chris Sawyer's engine. It still shares some code seen in Transport Tycoon and this is reflected in OpenTTD 0.1 such as the window system and graphics rendering. Chris Sawyer's Locomotion is a more up to date version, however OpenRCT2 will only use RollerCoaster Tycoon 2's engine code for now.

## 1.2 Decompiling the game
In order to decompile the game gradually and successfully, each procedure in RollerCoaster Tycoon 2 is to be re-written in C one by one. To help test the accuracy of the re-written procedures, the decompiled C procedures are compiled into a DLL (*openrct2.dll*) which exports an entry procedure mimicking the WinMain function in RollerCoaster Tycoon 2. The original executable *rct2.exe* has been patched so that *openrct2.dll* and WinMain are in the DLL import table and the WinMain export procedure in *openrct2.dll* is called at the start of the WinMain procedure in *rct2.exe* before returning. This way, starting rct2.exe now simply calls the new DLL which can then run all the decompiled code whilst still able to read / write to the *rct2.exe* memory model and run *rct2.exe* procedures.

The project therefore acts as a patch to RollerCoaster Tycoon 2 which can gradually implement each procedure whilst also adding new features where possible. Until all procedures of the original game are re-written in C, the project must remain a DLL which is called from the patched *rct2.exe*.

## 1.3 Progress
Currently the window system, graphics rendering and basic game loop are gradually being decompiled. Decompiling the all the game windows is a convenient way of identifying the game's memory structure. SDL2 has been used to replace the operating system calls so that game is cross-platform after the original game is no longer required.

## 1.4 Aim
The aim is to decompile RollerCoaster Tycoon 2 fully into C so that it can remain a cross platform game which runs on the latest operating systems with new features introduced and game play experience to be improved just like OpenTTD. Allowing the game to run in a resizeable window has already been accomplished using SDL2. Once the game has been fully decompiled, game logic such as peep path-finding, window / ride / object / map / construction limits increased, more sandbox friendly, editing available objects during a game, improved title sequence, mechanics only found in RCT1 (e.g. Shuttle Loop compatibility, pay for rides and park, have fun objective, finish five coasters objective, mountain tool during game) and lots more.

# 2 Building the source code

## 2.1 Prerequisites
- Windows XP / Vista / 7 / 8
- RollerCoaster Tycoon 2
- Visual Studio 2013 / Visual Studio Express 2013
- SDL2 development library for Visual C++.

## 2.2 Compiling and running
1. Checkout the repository.
2. Download SDL2 development library for Visual C++ and copy it to a directory called "sdl" in the repository. This directory should contain "include".
3. Open the solution in projects directory.
4. In *rct2.c*, ```GAME_PATH``` can be edited to reflect your RollerCoaster Tycoon 2 installation. (Registry keys and original executable are not required)
5. Select the 'Release' configuration and click Build -> Rebuild Solution.
6. Start debugging. If warned about *openrct2.exe* not having debug information, just continue.

# 3 Contributing

## 3.1 Decompiling
Experience with reverse engineering and x86 assembly is neccessary to decompile the original game. The game is currently being decompiled using IDA. Feel free to to take a procedure that hasn't currently been decompiled and decompile it. Contact IntelOrca for more information and for the lastest IDA database.

## 3.2 Naming of procedures and variables
Many variables and procedures are referenced in OpenRCT2 only by address. This may be because their function has not yet been identified. These can often be identified by removing their call and checking how they affect the game or reverse engineering the original game assembly to see where it is called / used.

## 3.3 Cleaning and documenting the source code
A lot of the source code is undocumented and messy. Whilst the structure of the code should be kept the same so that it closely resembles the original game. Various blocks of code can be moved into smaller functions and macros can be created for common operations.

## 3.4 Implementing new featues / fixing bugs
If enough of the game has been decompiled to implement a certain feature or fix a certain bug. This can be written. Comments should be added to clearly identify where code has been changed on purpose causing it to differ from the original game assembly.

# 4 License
**OpenRCT2** is licensed under the GNU General Public License version 3.
# Contents

- [Introduction](#1-introduction)
- [Contribution Philosophy](#2-contribution-philosophy)
- [Workflow](#3-workflow)
- [Contribution Types](#4-contribution-types)
    - [New Features](#41-new-features)
    - [Reporting Bugs](#42-reporting-bugs)
    - [Fixing Bugs](#43-fixing-bugs)
    - [Translation](#44-translation)
    - [Graphics](#45-graphics)
    - [Audio](#46-audio)
    - [Scenarios](#47-scenarios)
- [Best Practices](#5-best-practices)
    - [Adding New Strings](#51-adding-new-strings)
    - [Coding Style](#52-coding-style)
    - [Language](#53)


# 1. Introduction
The purpose of this page is to provide a high-level overview of some important concepts related to contributing. A more comprehensive, beginner-friendly guide can be be found at the following page on the wiki: [How To Contribute](https://github.com/OpenRCT2/OpenRCT2/wiki/How-To-Contribute)

# 2. Contribution Philosophy
Any contribution to OpenRCT2 is welcome and valued. Contributions can be in the form of bug reports, translation or code additions / changes. Please read this document and the [How To Contribute](https://github.com/OpenRCT2/OpenRCT2/wiki/How-To-Contribute) section of the wiki to learn how to contribute as effectively as possible. If you have any questions or concerns, please ask in the [Discord chat](https://discordapp.com/invite/fsEwSWs).

# 3. Workflow
OpenRCT2 uses the [gitflow workflow](https://www.atlassian.com/git/tutorials/comparing-workflows#gitflow-workflow). If you are implementing a new feature or logic from the original game, please branch off and perform pull requests to ```develop```. If you are fixing a bug for the next release, please branch off and perform pull requests to the correct release branch. ```master``` only contains tagged releases, you should never branch off this.
questions or concerns, please ask in the [Discord chat](https://discordapp.com/invite/fsEwSWs).

# 4. Contribution Types

## 4.1 New Features
Please talk to the OpenRCT2 team first before starting to develop a new feature. We may already have plans for or reasons against something that you'd like to work on. Therefore contacting us will allow us to help you or prevent you from wasting any time. You can talk to us via Discord, see links at the top of this page.

## 4.2 Reporting bugs
To report a bug, ensure you have a GitHub account. Search the issues page to see if the bug has already been reported. If not, create a new issue and write the steps to reproduce. Upload a saved game if possible as this is very helpful for users to replicate the bug. Please state which architecture and version of the game you are running, e.g.
```
OpenRCT2, v0.0.6-develop build 84ddd12 provided by AppVeyor
Windows (x86-64)
```

This can be found either at the bottom left of the title screen or
by running:
```
openrct2 --version
```

For Windows builds, OpenRCT2 will generate a memory dump and saved game when the game crashes unexpectedly. The game will open explorer to these files automatically for you. They are placed inside your configured user directory which by default is `%HOMEPATH%\Documents\OpenRCT2`.


## 4.3 Fixing bugs
A list of bugs can be found on the [issue tracker](https://github.com/OpenRCT2/OpenRCT2/issues). Feel free to work on any bug and submit a pull request to the develop branch with the fix. Mentioning that you intend to fix a bug on the issue will prevent other people from trying as well.

## 4.4 Translation
You can translate the game into other languages by editing the language files in ```data/language``` directory. Please join discussions in the [#localisation channel on Discord](https://discordapp.com/invite/sxnrvX9) and submit pull requests to [OpenRCT2/Localisation](https://github.com/OpenRCT2/Localisation).

## 4.5 Graphics
You can help create new graphics for the game by visiting the [OpenGraphics project](https://github.com/OpenRCT2/OpenGraphics). 3D modellers needed!

## 4.6 Audio
You can help create the music and sound effects for the game. Check out the OpenMusic repository and drop by our [#open-sound-and-music channel on Discord](https://discord.gg/9y8WbcX) to find out more.

## 4.7 Scenarios
We would also like to distribute additional scenarios with the game, when the time comes. For that, we need talented scenario makers! Check out the [OpenScenarios repository](https://github.com/PFCKrutonium/OpenRCT2-OpenScenarios).

# 5. Best Practices

## 5.1 Adding new strings
If you need to add a new localisable string to OpenRCT2, please add your new string entry to ```./data/language/en-GB.txt```.
It is important that you only edit en-GB in the OpenRCT2 repository as this is the base language that is used for
translation to other languages. A separate repository OpenRCT2/Localisation is used for translation pull requests, and changes
to that repository are merged with the OpenRCT2 main repository every night. When your pull request is merged, it is helpful
to create a new issue in the OpenRCT2/Localisation repository about the new strings you have added. This notifies translators
so that they can translate the new strings as quick as possible. Similarly if you change any existing string, it is more
important that you create an issue as this can be more easily overlooked.

When coding, please also add a string constant for your strings to ```./src/localisation/StringIds.h```.

## 5.2 Coding style
Use [this](https://github.com/OpenRCT2/OpenRCT2/wiki/Coding-Style) code style as a reference for new or changed code.

## 5.3 Language
For now, it is recommended that you only write C++ files as the majority of the game is currently in
C++. Exceptions are to modules that have direct relationship to original code.

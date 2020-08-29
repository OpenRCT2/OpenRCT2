# Contributing to OpenRCT2
Any contribution to OpenRCT2 is welcome and valued. Contributions can be in the form of bug reports, translation or code
additions / changes. Please read this document to learn how to contribute as effectively as possible. If you have any
questions or concerns, please ask in the [Discord chat](https://discordapp.com/invite/fsEwSWs).

# Reporting bugs
To report a bug, ensure you have a GitHub account. Search the issues page to see if the bug has already been reported.
If not, create a new issue and write the steps to reproduce. Upload a saved game if possible as this is very helpful
for users to replicate the bug. Please state which architecture and version of the game you are running, e.g.
```
OpenRCT2, v0.0.6-develop build 84ddd12 provided by AppVeyor
Windows (x86-64)
```

This can be found either at the bottom left of the title screen or
by running:
```
openrct2 --version
```

For Windows builds, OpenRCT2 will generate a memory dump and saved game when the game crashes unexpectedly. The game
will open explorer to these files automatically for you. They are placed inside your configured user directory which
by default is `%HOMEPATH%\Documents\OpenRCT2`.

# Translation
Translation is managed in a separate repository, [OpenRCT2/Localisation](https://github.com/OpenRCT2/Localisation).
You will find more information there.

# Contributing code
Please read [How To Contribute](https://github.com/OpenRCT2/OpenRCT2/wiki/How-To-Contribute)

## Code hints
### Adding new strings
If you need to add a new localisable string to OpenRCT2, please add your new string entry to ```./data/language/en-GB.txt```.
It is important that you only edit en-GB in the OpenRCT2 repository as this is the base language that is used for
translation to other languages. A separate repository OpenRCT2/Localisation is used for translation pull requests, and changes
to that repository are merged with the OpenRCT2 main repository every night. When your pull request is merged, it is helpful
to create a new issue in the OpenRCT2/Localisation repository about the new strings you have added. This notifies translators
so that they can translate the new strings as quick as possible. Similarly if you change any existing string, it is more
important that you create an issue as this can be more easily overlooked.

When coding, please also add a string constant for your strings to ```./src/localisation/StringIds.h```.

### Coding style
Use [this](https://github.com/OpenRCT2/OpenRCT2/wiki/Coding-Style) code style as a reference for new or changed code.

### Language
For now, it is recommended that you only write C++ files as the majority of the game is currently in
C++. Exceptions are to modules that have direct relationship to original code.

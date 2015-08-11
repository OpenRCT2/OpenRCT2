# Contributing to OpenRCT2
Any contribution to OpenRCT2 is welcome and valued. Contributions can be in the form of bug reports, translation or code
additions / changes. Please read this document to learn how to contribute as effectively as possible. If you have any
questions or concerns, please ask in the [gitter](https://gitter.im/OpenRCT2/OpenRCT2) chat room.

# Reporting bugs
To report a bug, ensure you have a GitHub account. Search the issues page to see if the bug has already been reported.
If not, create a new issue and write the steps to reproduce. Upload a saved game if possible and this is very helpful
for users to replicate the bug.

# Translation
Translation is managed in a separate repository, [OpenRCT2/Localisation](https://github.com/OpenRCT2/Localisation).
You will find more information there.

# Contributing code
## Steps
1. First, ensure you have a GitHub account and [fork](https://help.github.com/articles/fork-a-repo/) the OpenRCT2 repository.
2. Create a new branch from develop (unless you are contributing to another) and commit your changes to that.
3. Submit a new [pull request](https://help.github.com/articles/using-pull-requests/).
4. Wait for other users to test and review your changes.

## Credits
If you are contributing to OpenRCT2, please add your name to ```./contributors.md``` so that you can be credited for your
work outside and inside the game.

## Code hints
### Adding new strings
If you need to add a new localisable string to OpenRCT2, please add your new string entry to ```./data/language/english_uk.txt```.
It is important that you only edit English (UK) in the OpenRCT2 repository as this is the base language that is used for
translation to other languages. A separate repository OpenRCT2/Localisation is used for translation pull requests, and changes
to that repository are merged with the OpenRCT2 main repository every night. When your pull request is merged, it is helpful
to create a new issue in the OpenRCT2/Localisation repository about the new strings you have added. This notifies translators
so that they can translate the new strings as quick as possible. Similarly if you change any existing string, it is more
important that you create an issue as this can be more easily overlooked.

When coding, please also add a string constant for your strings to ```./src/localisation/string_ids.h```.

### Coding style / language
OpenRCT2 currently does not have a strict coding style. This will be implemented after the entire game is implemented and
undergoes heavy refactoring. For now, it is recommended that you only write C files as the majority of the game is currenlty in
C. Exceptions are to modules that have no direct relationship to original code such as the new software audio mixer, the
HTTP integration and the Twitch integration.

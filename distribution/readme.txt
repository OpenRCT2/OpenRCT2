Last updated:    2022-12-14
Release version: 0.4.3
------------------------------------------------------------------------


Table of contents
-----------------
1.0) About
2.0) Contacting
 * 2.1) Reporting bugs
3.0) Supported platforms
4.0) Installing and running OpenRCT2
5.0) Development
6.0) Translation
7.0) Troubleshooting
8.0) Licensing
9.0) Credits

1.0) About
---- -----
OpenRCT2 is an amusement park simulation game based upon the popular game
RollerCoaster Tycoon 2, written by Chris Sawyer. It attempts to mimic the
original game as closely as possible while extending it with new features.

OpenRCT2 is licensed under the GNU General Public License version 3.0, but
includes some 3rd party software under different licenses. See the section
"Licensing" below for details.

2.0) Contacting
---- ----------
The easiest way to contact the OpenRCT2 team is by submitting issues on GitHub
(https://github.com/OpenRCT2/OpenRCT2) in the form of questions or bug reports.
You can also chat with us on Discord (https://discordapp.com/invite/fsEwSWs).

2.1) Reporting bugs
---- --------------
GitHub is used for tracking bugs in OpenRCT2. Please check if the bug has
already been reported using the search functionality before submitting.

When you are sure it is not already reported you should:
 * Make sure you are running a recent version, i.e. run the latest stable or
   nightly based on where you found the bug.
 * Make sure you are not running a non-official binary, like a fork.
   When you are playing with a fork you should report any bugs to the bug
   tracker for that fork which is most likely another GitHub repository.
 * Make it reproducible for the developers. In other words, create a savegame
   in which you can reproduce the issue once loaded. It is very useful to give
   us the dump file as well, and a screenshot.
 * Check whether the bug is already reported on our bug tracker. This includes
   searching for recently closed bug reports as the bug might already be fixed.

After you have done all that you can report the bug. Please include the
following information in your bug report:
 * OpenRCT2 version (PLEASE test the latest git develop build)
 * Bug details, including instructions how to reproduce it
 * Platform (Windows, Linux, FreeBSD, ...) and compiler (including version) if
   you compiled OpenRCT2 yourself.
 * The processor architecture of your OS (x86 Windows, x86-64 Windows,
   Android on an ARM, Linux on a PowerPC, ...)
 * The language and culture your operating system is using.
 * Attach a saved game *and* a screenshot if possible
 * If this bug only occurred recently please note the last version without
   the bug and the first version including the bug. That way we can fix it
   quicker by looking at the changes made.
 * Attach a crash dump (if your game crashed or froze).

3.0) Supported platforms
---- -------------------
OpenRCT2 is currently supported on Windows Vista and above, many distributions of
Linux, macOS 10.13 or higher, Android, FreeBSD and OpenBSD. OpenRCT2 will only work on
little-endian architectures.
Further instructions can be found on GitHub.

4.0) Installing and running OpenRCT2
---- ------------------------------
Installing OpenRCT2 is fairly straightforward. Either you have downloaded an
archive which you have to extract to a directory where you want OpenRCT2 to
be installed, or you have downloaded an installer, which will automatically
extract OpenRCT2 in the given directory.

OpenRCT2 requires an installation of RollerCoaster Tycoon 2 (RCT2) to run. You
must have either installed the original RCT2 disc, the GOG version or the Steam
version. Alternatively you can manually specify the location of where your RCT2
data files are. These may be directly copied off of the original disc and placed in
a directory of your choice.

When you start OpenRCT2 for the first time, it will look for the RCT2 data files
in the following locations:
  - C:\Program Files\Infogrames\RollerCoaster Tycoon 2,
  - C:\Program Files (x86)\Infogrames\RollerCoaster Tycoon 2,
  - C:\Program Files\Infogrames Interactive\RollerCoaster Tycoon 2,
  - C:\Program Files (x86)\Infogrames Interactive\RollerCoaster Tycoon 2,
  - C:\Program Files\Atari\RollerCoaster Tycoon 2,
  - C:\Program Files (x86)\Atari\RollerCoaster Tycoon 2,
  - C:\GOG Games\RollerCoaster Tycoon 2 Triple Thrill Pack,
  - C:\Program Files\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack,
  - C:\Program Files (x86)\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack,
  - C:\Program Files\Steam\steamapps\common\Rollercoaster Tycoon 2,
  - C:\Program Files (x86)\Steam\steamapps\common\Rollercoaster Tycoon 2,
  - The location of the openrct2.exe (where OpenRCT2 was extracted/installed)

If none of these locations are found, OpenRCT2 will ask you to manually specify
the directory. Alternatively after running OpenRCT2 for the first time, you can
edit config.ini in the OpenRCT2 sub directory of your documents folder to set
the RCT2 install path.

OpenRCT2 supports setting a DPI scale in increments of .25. To set it, open the Options menu.

If you wish to use Steam Overlay or capture game for video recording or
streaming, you might have to enable hardware display in the options window.
This will still render the graphics on the CPU but the pixel data will be
displayed using either DirectX or OpenGL. This may also enable vertical
synchronisation if supported by your graphics card and you have enabled
"Uncap FPS".

5.0) Development
---- -----------
OpenRCT2 is an open-source collaborative project. It is developed voluntarily
and hosted on GitHub. If you would like to contribute to the development of
OpenRCT2, please read the readme file in the OpenRCT2 repository or on the main
GitHub page at (https://github.com/OpenRCT2/OpenRCT2). This gives more detailed
information about the project, its roadmap and how to compile the source code.

6.0) Translation
---- -----------
For more information about the game's translation and how to contribute, please
visit the GitHub page and wiki at (https://github.com/OpenRCT2/Localisation).

7.0) Troubleshooting
---- ---------------
If you are having problems running OpenRCT2, you can run OpenRCT2 in verbose
mode to view a detailed diagnostic log. This can be activated by running the
game via Command Prompt or PowerShell with the command line switch --verbose.

This log can help pinpoint where a problem exists and is a useful resource if
you wish to ask online for help.

8.0) Licensing
---- ---------
OpenRCT2 is licensed under the GNU General Public Licence version 3.0. For
the complete licence text, see the file 'licence.txt'. This licence applies
to all files in this distribution, except as noted below.

dukglue          | MIT licence.
duktape          | MIT licence.
libcURL          | MIT (or Modified BSD-style) licence.
libicu           | Unicode licence.
libpng           | libpng licence.
libspeex         | BSD-style licence.
libzip           | BSD 3 clause licence.
nlohmann-json    | MIT licence.
OpenSSL          | OpenSSL Licence
SDL2             | zlib licence.
zlib             | zlib licence.
Google Test      | BSD 3 clause licence.
Google Benchmark | Apache 2.0 licence.

Licences for sub-libraries used by the above may vary. For more information, visit the libraries' respective official websites.

9.0) Credits
---- -------
For the full list of contributors to OpenRCT2, see the file 'contributors.md'.

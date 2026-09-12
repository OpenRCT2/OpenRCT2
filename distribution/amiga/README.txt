OpenRCT2 for AmigaOS 3.2 (68k) -- tester build 3
================================================

This is an early test build of OpenRCT2, the open-source re-implementation of
RollerCoaster Tycoon 2, running natively on a 68k Amiga. It is a big-endian port
of the upstream C++ engine with an Intuition/RTG display backend. It is NOT
finished; you are testing it. Thank you.

WHAT YOU NEED
-------------
 * AmigaOS 3.2 (tested on 3.2.3). Other 3.x versions may work but are untested.
 * An RTG graphics card with a Picasso96 or CyberGraphX driver that offers an
   8-bit (256-colour) 640x480 screen mode. AGA/ECS chipset-only machines are
   NOT supported.
 * A fast CPU: 68040 or 68060, PiStorm (Emu68), or Vampire/Apollo. Tested on
   an emulated 68040. A plain 68020/030 will run it but very slowly.
 * About 300 MB of FREE Fast RAM (the game uses ~256 MB). 512 MB recommended.
 * About 80 MB of disk space for this archive, plus your RCT2 data.
 * The data files of the ORIGINAL RollerCoaster Tycoon 2 (GOG, Steam or CD).
   They are not included and cannot be. You need the folders
      Data   ObjData   Scenarios   Tracks
   copied to the Amiga. Sound effects come from Data/css1.dat (5 MB) and the
   title music from Data/css17.dat; the other css*.dat files are the ride
   music (~480 MB together) -- copy the ones you want, or all of them.

INSTALL
-------
 1. Extract this archive somewhere with enough room, e.g. Work:Games/.
    You get a drawer  OpenRCT2  with an icon.
 2. Copy your RCT2 data folders to the Amiga, e.g. Work:Games/RCT2/.
 3. Tell the game where they are with an assign. In a Shell:
        Assign RCT2: Work:Games/RCT2
    Add that line to S:User-Startup so it survives a reboot.
 4. Double-click the OpenRCT2 icon (or in a Shell: CD to the drawer and type
    Execute OpenRCT2). The first start takes a while (see below).

FIRST START
-----------
The first start builds an index of all game objects and scenarios and writes it
to the  user  drawer; this takes 1-2 minutes on a 68040 and is much faster on
later starts. Loading the title screen takes 40-60 s on an emulated 68040.
Expect ~25-30 frames per second at 640x480 on a fast machine.

Cash, date and temperature appear bottom-left/right; the toolbar is at the top.
NEW GAME lists the RollerCoaster Tycoon 1 and 2 scenarios found in your
Scenarios folder (plus Wacky Worlds and Time Twister if you have them). Click a
scenario to start it.

KNOWN LIMITATIONS OF THIS BUILD
-------------------------------
 * Sound and music play through AHI (ahi.device unit 0). Install AHI and set
   up unit 0 in Prefs/AHI (Paula on a plain Amiga; the Vampire/PiStorm drivers
   where available). Without AHI the game runs silently. Sound was verified by capturing the mixed stream on an emulator; real-hardware AHI playback is what this build needs testers for -- please report whether you hear the title music and clicks. Music needs the
   css*.dat files from your RCT2 Data folder; the optional OpenRCT2 music packs
   (OGG) are not supported yet.
 * RollerCoaster Tycoon 1 scenarios (.SC4) load and play (their simulation is
   bit-exact with the PC build). Rides that exist only in RCT1 use fallback
   RCT2 graphics unless you also own RCT1 and set  rct1_path  in
   user/config.ini to its folder ("Park has objects which require RCT1 linked"
   in the log is that, not an error).
 * Only the standard Intuition mouse pointer (no custom cursors), no clipboard.
 * Resolution: 640x480 by default. You can change window_width/window_height
   in  user/config.ini  but the frame rate drops with the pixel count
   (1280x720 runs at ~9 fps on an emulated 68040).
 * Saving games has had little testing. Save often, expect surprises.
 * Multiplayer, plugins and the scenario editor are untested.

REPORTING PROBLEMS
------------------
Please tell us: your machine (CPU, RAM, graphics card, OS version), what you
did, and what happened. A screenshot helps. For crashes or hangs, enable the
trace log BEFORE starting the game:
        SetEnv OPENRCT2_TRACE T:openrct2-trace.txt
then reproduce the problem and send us T:openrct2-trace.txt. Delete the
environment variable again afterwards (it slows loading down):
        UnSetEnv OPENRCT2_TRACE

OpenRCT2 is GPLv3. Source of this port: the "amiga" branch of the project's
repository (ask the person who gave you this build).

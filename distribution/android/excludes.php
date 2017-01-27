<?php

$include = [
    '/src/*.c',
    '/src/atomic/SDL_atomic.c',
    '/src/atomic/SDL_spinlock.c.arm',
    '/src/audio/*.c',
    '/src/audio/android/*.c',
    '/src/audio/dummy/*.c',
    '/src/core/android/*.c',
    '/src/cpuinfo/*.c',
    '/src/dynapi/*.c',
    '/src/events/*.c',
    '/src/file/*.c',
    '/src/filesystem/android/*.c',
    '/src/haptic/*.c',
    '/src/haptic/dummy/*.c',
    '/src/joystick/*.c',
    '/src/joystick/android/*.c',
    '/src/loadso/dlopen/*.c',
    '/src/power/*.c',
    '/src/power/android/*.c',
    '/src/render/*.c',
    '/src/render/*/*.c',
    '/src/stdlib/*.c',
    '/src/test/*.c',
    '/src/thread/*.c',
    '/src/thread/pthread/*.c',
    '/src/timer/*.c',
    '/src/timer/unix/*.c',
    '/src/video/*.c',
    '/src/video/android/*.c'
];

$basedir = realpath(__DIR__ . '/sdl/external');
$srcdir = $basedir . '/src/';

$excludesFile = '[' . PHP_EOL;


$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($srcdir, FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS));
foreach ($it as $fileinfo) {
    if (fnmatch('*.h', $fileinfo->getFilename(), FNM_CASEFOLD)) {
        continue;
    }

    $relativePath = substr($fileinfo, strlen($basedir));

    $matches = false;
    foreach ($include as $pattern) {
        if (fnmatch($pattern, $relativePath, FNM_CASEFOLD | FNM_PATHNAME)) {
            $matches = true;
        }
    }

    if ($matches === true) {
        continue;
    }

    $excludesFile .= '    "' . substr($fileinfo, strlen($srcdir)) . '",' . PHP_EOL;
}

$excludesFile .= ']' . PHP_EOL;
echo $excludesFile;


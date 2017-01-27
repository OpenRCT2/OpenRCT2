<?php

function log_d()
{
    $args = func_get_args();

    if (count($args) === 0) {
        return;
    }

    if (count($args) === 1) {
        echo $args[0] . PHP_EOL;
    }

    if (count($args) === 1) {
        echo call_user_func_array('printf', $args) . PHP_EOL;
    }
}

function rmrecursive($path)
{
    if (!file_exists($path)) {
        return;
    }

    $files = scandir($path);
    foreach ($files as $file) {
        if (in_array($file, ['.', '..'])) {
            continue;
        }

        if (is_file($path . '/' . $file)) {
            unlink($path . '/' . $file);
            continue;
        }

        if (is_dir($path . '/' . $file)) {
            rmrecursive($path . '/' . $file);
            continue;
        }
    }

    rmdir($path);
}

function download_library($name, $url, $folder, $srcPatterns = [], $move = [])
{
    $archive = $folder . '/' . $name . '-' . substr(sha1($url), 0, 7) . '.tar.gz';
    if (!file_exists($archive)) {
        log_d('Downloading %s', $url);

        if (!file_exists($folder)) {
            mkdir($folder, 0777, true);
        }
        file_put_contents($archive, fopen($url, 'r'));
        log_d('done!');
    }

    try {
        $extension = pathinfo($archive, PATHINFO_EXTENSION);
        if (strtolower($extension) === 'gz') {
            $unGzipped = dirname($archive) . '/' . basename($archive, '.gz');

            if (file_exists($unGzipped)) {
                unlink($unGzipped);
            }
        }
        $phar = new PharData($archive);

        if ($phar->isCompressed()) {
            $phar->decompress();
        }

        $basePath = $phar->getPathname();

        $files = [];

        $archiveRoot = $phar->getFilename();
        foreach (new RecursiveIteratorIterator($phar) as $file) {
            /** @var PharFileInfo $file */
            if (!$file->isFile()) {
                continue;
            }

            $filename = substr($file->getPathname(), strlen($basePath));
            foreach ($srcPatterns as $srcPattern) {
                if (fnmatch($srcPattern, $filename, FNM_CASEFOLD)) {
                    $files[] = $archiveRoot . $filename;
                    break;
                }
            }
        }


        $dest = __DIR__ . '/' . $name . '/';
        if (!file_exists($dest)) {
            mkdir($dest, 0777, true);
        }

        if (empty($archiveRoot)) {
            rmrecursive($dest . 'external');
            if (count($files) === 0) {
                return;
            }
            mkdir($dest . 'external');
            $phar->extractTo($dest . 'external', $files);
        } else {
            rmrecursive($dest . $archiveRoot);
            rmrecursive($dest . 'external');
            if (count($files) === 0) {
                return;
            }
            $phar->extractTo($dest, $files);
            rename($dest . $archiveRoot, $dest . 'external');
        }

        foreach ($move as $mvSrc => $mvDest) {
            if (!file_exists(dirname($dest . 'external' . $mvDest))) {
                mkdir(dirname($dest . 'external' . $mvDest), 0777, true);
            }
            rename($dest . 'external' . $mvSrc, $dest . 'external' . $mvDest);
        }

    } catch (Exception $e) {
        var_dump($e);
    }

}


$libDir = __DIR__ . '/tmp';

download_library('freetype', 'http://download.savannah.gnu.org/releases/freetype/freetype-2.4.12.tar.gz', $libDir, ['/src/**', '/src/**/*', '/include/**', '/include/**/*']);
download_library('sdl-ttf', 'https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.tar.gz', $libDir, ['/SDL_ttf.[hc]']);
download_library('sdl', 'https://libsdl.org/release/SDL2-2.0.4.tar.gz', $libDir, ['/src/**', '/src/**/*', '/include/**', '/include/**/*']);
download_library('libspeexdsp', 'https://github.com/xiph/speexdsp/archive/SpeexDSP-1.2rc3.tar.gz', $libDir, ['/include/**', '/libspeexdsp/**']);
download_library('libpng', 'http://downloads.sourceforge.net/project/libpng/libpng14/1.4.19/libpng-1.4.19.tar.gz', $libDir, ['/*.[ch]']);
download_library('libjansson', 'https://github.com/akheron/jansson/archive/v2.7.tar.gz', $libDir, ['/src/**', '/android/**'], ['/src/jansson.h' => '/include/jansson.h']);

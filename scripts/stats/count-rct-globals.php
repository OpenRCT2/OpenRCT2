#!/usr/bin/env php

<?php

exec('grep -E "(RCT2_ADDRESS|RCT2_GLOBAL)[\(\s]+(0x[0-9a-zA-Z]+|[A-Z0-9_]+)" -r -o .', $lines);

$addressFiles = [];
$lastFile = null;
$count = [];
$totalCount = 0;

foreach ($lines as $line) {
    $fileName = $lastFile;
    if (strpos($line, ':') !== false) {
        $data = explode(':', $line);
        $fileName = $data[0];
        $line = $data[1];
        $lastFile = $fileName;
    }

    $line = preg_replace_callback('/0x0+([0-9A-Fa-f])/', function ($matches) {
        return '0x' . strtoupper($matches[1]);
    }, $line);
    $line = preg_replace('/RCT2_(GLOBAL|ADDRESS)[\(\s]+/', '', $line);

    if (!isset($addressFiles[$line])) {
        $addressFiles[$line] = [];
    }
    $addressFiles[$line][] = $fileName;

    if (!isset($count[$line])) {
        $count[$line] = 0;
    }

    $count[$line]++;
    $totalCount++;
}

if (isset($argv[1]) && $argv[1] === '-v') {
    foreach ($count as $name => $c) {
        printf("%s\t%d\t%s\n", $name, $c, implode(", ", array_unique($addressFiles[$name])));
    }

    echo "\n";
}


printf("Found %d usages of RCT2_ADDRESS and RCT2_GLOBAL (%d distinct)\n", $totalCount, count($count));

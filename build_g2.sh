#!/bin/bash

if [[ $TARGET == "windows" ]]; then
	wine openrct2.exe sprite build data/g2.dat resources/g2/
else
	./openrct2 sprite build data/g2.dat resources/g2/
fi

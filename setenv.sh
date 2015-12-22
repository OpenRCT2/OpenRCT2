#!/bin/bash

###########################################################
# Setups a bash environment for OpenRCT2 development
###########################################################
rootPath=`pwd`
scriptsPath="$rootPath/scripts/bash"

echo -e "\e[36mSetting up OpenRCT2 development environment for $os\e[39m"

# Add scripts to path
export PATH=$scriptsPath:$PATH

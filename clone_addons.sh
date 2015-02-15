#!/bin/bash

# Dependencies for vespers

# assuming we are in OF_ROOT/apps/vespers/vespers
# or something similar
cd ../../../addons/

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
else
    PREFIX="git clone git@github.com:"
fi


${PREFIX}kylemcdonald/ofxCv.git -b 0.8.4

${PREFIX}andreasmuller/ofxAutoReloadedShader.git

${PREFIX}andyinabox/ofxTimeline.git -b no-hotkeys

# use the ofxTimeline script for the rest
cd ofxTimeline/
./clone_addons.sh
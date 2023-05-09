versionnum="v1.0"

filenameredump="patch/redump_patch/Ginga Ojousama Densetsu Yuna - Final Edition EN [${versionnum}] Redump.xdelta"

mkdir -p patch
mkdir -p patch/redump_patch

./build.sh

rm -f "$filenameredump"
xdelta3 -e -B 610433376 -s "patch/exclude/yuna3f.bin" "yuna3f_build.bin" "$filenameredump"

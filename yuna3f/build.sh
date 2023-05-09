
echo "*******************************************************************************"
echo "Setting up environment..."
echo "*******************************************************************************"

set -o errexit

BASE_PWD=$PWD
PATH=".:$PATH"

DISCASTER="../discaster/discaster"
ARMIPS="./armips/build/armips"

mkdir -p out

function remapPalette() {
  oldFile=$1
  palFile=$2
  newFile=$3
  
  if [ "$newFile" == "" ]; then
    newFile=$oldFile
  fi
  
  convert "$oldFile" -dither None -remap "$palFile" PNG32:$newFile
}

# function remapPaletteDithered() {
#   oldFile=$1
#   palFile=$2
#   newFile=$3
#   
#   convert "$oldFile" -dither Riemersma -remap "$palFile" PNG32:$newFile
# }

echo "*******************************************************************************"
echo "Copying game files..."
echo "*******************************************************************************"

#rm -r -f out/files
#if [ ! -e out/files ]; then
  mkdir -p out/files
  cp -r disc/files/* out/files
#fi

echo "*******************************************************************************"
echo "Building tools..."
echo "*******************************************************************************"

make

if [ ! -e $ARMIPS ]; then
  
  echo "********************************************************************************"
  echo "Building armips..."
  echo "********************************************************************************"
  
  cd armips
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
  cd $BASE_PWD
  
fi

echo "*******************************************************************************"
echo "Refreshing archive directories..."
echo "*******************************************************************************"

cp -r rsrc_raw out
cp -r rsrc out

echo "*******************************************************************************"
echo "Building font..."
echo "*******************************************************************************"

mkdir -p out/font
mkdir -p out/font/default
#yuna3f_fontbuild "font/nimbus-sans-l-bold-11/" "out/font/default/font.bin" "out/font/default/width.bin" "out/font/default/kerning_index.bin" "out/font/default/kerning_data.bin" "out/font/default/glyph_width.bin" "out/font/default/kerning_matrix.bin"
yuna3f_fontbuild "font/nimbus-sans-l-bold-11/" "out/font/default/" -w 12 -h 12 -k 0\
  --globalzkern -1\
  --globalnkkern -1\
  \
    --offsetkernpair 0x37 0x59 1\
    --offsetkernpair 0x54 0x59 1\
    --offsetkernpair 0x57 0x59 1\
    --offsetkernpair 0x58 0x59 1\
    --offsetkernpair 0x5E 0x59 1\
    --offsetkernpair 0x65 0x59 1\
  \
    --offsetkernpair 0x37 0x5A 1\
    --offsetkernpair 0x54 0x5A 1\
    --offsetkernpair 0x57 0x5A 1\
    --offsetkernpair 0x58 0x5A 1\
    --offsetkernpair 0x59 0x5A 1\
    --offsetkernpair 0x5E 0x5A 1\
    --offsetkernpair 0x65 0x5A 1\
  \
    --offsetkernpair 0x37 0x5C 1\
    --offsetkernpair 0x54 0x5C 1\
    --offsetkernpair 0x57 0x5C 1\
    --offsetkernpair 0x58 0x5C 1\
    --offsetkernpair 0x5E 0x5C 1\
    --offsetkernpair 0x65 0x5C 1\
  \
    --offsetkernpair 0x37 0x62 1\
    --offsetkernpair 0x54 0x62 1\
    --offsetkernpair 0x57 0x62 1\
    --offsetkernpair 0x58 0x62 1\
    --offsetkernpair 0x5E 0x62 1\
    --offsetkernpair 0x65 0x62 1
  
#    --offsetkernpair 0x5E 0x67 1

mkdir -p out/font/narrow
#yuna3f_fontbuild "font/nimbus-sans-l-bold-condensed-11/" "out/font/narrow/font.bin" "out/font/narrow/width.bin" "out/font/narrow/kerning_index.bin" "out/font/narrow/kerning_data.bin" "out/font/narrow/glyph_width.bin" "out/font/narrow/kerning_matrix.bin"
yuna3f_fontbuild "font/nimbus-sans-l-bold-condensed-11/" "out/font/narrow/" -w 12 -h 12

# HACK: the letters "i" and "l" specifically need to be pushed to the right to look okay.
# what i'm doing here is very much a hack, and relies on the fact that the letters are
# already pushed one pixel to the right in the source font sheet,
# meaning their kerning is guaranteed to be nonzero as long as kerning is not entirely
# disabled for a character.
mkdir -p out/font/subtitle
yuna3f_fontbuild "font/nimbus-sans-l-bold-12-outline/" "out/font/subtitle/" -w 16 -h 16 -k 2 -d 3 --offsetkern 0x59 1 --offsetkern 0x5C 1 --offsetkern 0x39 1

mkdir -p out/font/subtitle_8bpp
yuna3f_fontbuild "font/nimbus-sans-l-bold-12-outline/" "out/font/subtitle_8bpp/" -w 16 -h 16 -k 2 -d 3 --8bpp --offsetkern 0x59 1 --offsetkern 0x5C 1 --offsetkern 0x39 1

mkdir -p out/rsrc_raw/pal
#psx_genchrompal 0x00 0x00 0x00 0xFF 0xFF 0xFF "out/rsrc_raw/pal/monochrome.pal" "out/rsrc_raw/pal/monochrome.png" --trans --force 1 0x11 0x11 0x11
psx_genchrompal 0x00 0x00 0x00 0xFF 0xFF 0xFF "out/rsrc_raw/pal/subtitle.pal" "out/rsrc_raw/pal/subtitle.png" --force 0 0x11 0x11 0x11
datsnip "out/rsrc_raw/pal/subtitle.pal" 0 30 "out/rsrc_raw/pal/subtitle_clipped.pal"

echo "*******************************************************************************"
echo "Generating script..."
echo "*******************************************************************************"

mkdir -p out/scripttxt
mkdir -p out/scriptwrap
mkdir -p out/script

yuna3f_scriptimport

yuna3f_scriptwrap "out/scripttxt/spec_main.txt" "out/scriptwrap/spec_main.txt"
yuna3f_scriptwrap "out/scripttxt/spec_mapscript.txt" "out/scriptwrap/spec_mapscript.txt"
yuna3f_scriptwrap "out/scripttxt/spec_new.txt" "out/scriptwrap/spec_new.txt"
yuna3f_scriptwrap "out/scripttxt/spec_system.txt" "out/scriptwrap/spec_system.txt"
yuna3f_scriptwrap "out/scripttxt/spec_video.txt" "out/scriptwrap/spec_video.txt" "table/yuna3_en.tbl" "out/font/subtitle_8bpp/width.bin" "out/font/subtitle_8bpp/glyph_width.bin" "out/font/subtitle_8bpp/kerning_matrix.bin"
yuna3f_scriptwrap "out/scripttxt/spec_xa.txt" "out/scriptwrap/spec_xa.txt" "table/yuna3_en.tbl" "out/font/subtitle_8bpp/width.bin" "out/font/subtitle_8bpp/glyph_width.bin" "out/font/subtitle_8bpp/kerning_matrix.bin"

yuna3f_scriptbuild "out/scriptwrap/" "out/script/"

echo "*******************************************************************************"
echo "Building graphics..."
echo "*******************************************************************************"

function buildAceImgInPlace() {
  inPrefix=$1
  imgName=$2
  tcoName=$3
  gs8Name=$4
  
  yuna3f_aceimgbuild "${inPrefix}$imgName" "${inPrefix}$tcoName" "${inPrefix}$gs8Name" "${inPrefix}$gs8Name"
}

function buildAceImgInPlaceNoTrans() {
  inPrefix=$1
  imgName=$2
  tcoName=$3
  gs8Name=$4
  
  yuna3f_aceimgbuild "${inPrefix}$imgName" "${inPrefix}$tcoName" "${inPrefix}$gs8Name" "${inPrefix}$gs8Name" --notrans
}

#=====
# manually map needed animation file for number of lines in
# victory/defeat conditions
#=====

function remapObjectiveAnim() {
  namePrefix=$1
  numVic=$2
  numDef=$3
  
  targetTanFile="rsrc_raw/tan/msobjective_vic${numVic}def${numDef}.tan"
  cp "$targetTanFile" "out/rsrc/grp/BTLETC/$namePrefix/TAN"
}

remapObjectiveAnim "MST000.CSA" 1 1
remapObjectiveAnim "MST001.CSA" 1 1
remapObjectiveAnim "MST002.CSA" 1 2
remapObjectiveAnim "MST005.CSA" 2 1
remapObjectiveAnim "MST006.CSA" 1 2
remapObjectiveAnim "MST007.CSA" 2 1
remapObjectiveAnim "MST008.CSA" 2 1
remapObjectiveAnim "MST009.CSA" 2 1
remapObjectiveAnim "MST010.CSA" 2 2
remapObjectiveAnim "MST011.CSA" 1 2
remapObjectiveAnim "MST012.CSA" 2 1
remapObjectiveAnim "MST013.CSA" 1 1
remapObjectiveAnim "MST014.CSA" 1 2
remapObjectiveAnim "MST015.CSA" 1 1
remapObjectiveAnim "MST018.CSA" 2 1
remapObjectiveAnim "MST019.CSA" 2 1
remapObjectiveAnim "MST020.CSA" 2 2
remapObjectiveAnim "MST021.CSA" 2 1
remapObjectiveAnim "MST022.CSA" 2 2
remapObjectiveAnim "MST025.CSA" 2 1
remapObjectiveAnim "MST026.CSA" 2 1
remapObjectiveAnim "MST029.CSA" 2 1
remapObjectiveAnim "MST030.CSA" 1 1
remapObjectiveAnim "MST031.CSA" 2 1
remapObjectiveAnim "MST032.CSA" 1 1
remapObjectiveAnim "MST050.CSA" 1 1
remapObjectiveAnim "MST064.CSA" 1 1
remapObjectiveAnim "MST067.CSA" 1 1
remapObjectiveAnim "MST070.CSA" 2 1
# mst079 (the security device puzzle in chapter 4) has objective text,
# but it's not actually used (and the animation doesn't seem to have been
# properly implemented anyway)
remapObjectiveAnim "MST080.CSA" 1 1
remapObjectiveAnim "MST081.CSA" 1 1
remapObjectiveAnim "MST082.CSA" 1 1
remapObjectiveAnim "MST088.CSA" 2 1
remapObjectiveAnim "MST093.CSA" 2 1
# special tan for vs mode objective
cp "rsrc_raw/tan/msobjective_vs.tan" "out/rsrc/grp/BTLETC/MST094.CSA/TAN"

#=====
# prep objective images
#=====

for folder in out/rsrc/grp/BTLETC/MST*.CSA; do
  fileName=$(basename $folder)
  imgName=$folder/image.png
  remapPalette "$imgName" "rsrc/grp/orig/BTLETC/MST002.CSA/image.png" "$imgName"
  
  echo $imgName
  
  # composite objective labels
  # HACK: mission 94 is for the 2-player battle mode and has a dedicated
  # objective label baked into the image
  if [ "$fileName" != "MST094.CSA" ]; then
    convert $imgName\
      "out/rsrc/grp/objective_labels.png" -geometry +0+0 -compose copy -composite\
      PNG32:$imgName
  fi
  
  yuna3f_aceimgbuild "$folder/image.png" "$folder/TCO" "$folder/GS8" "$folder/GS8"
  yuna3f_arcbuild "$folder/" "index.txt" "out/rsrc_raw/gou/BTLETC/$fileName"
done

remapPalette "out/rsrc/grp/BTCOM.png" "out/rsrc/grp/orig/BTCOM.png"
#psx_img_decolorize "out/rsrc/grp/BTCOM.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/BTCOM_gray.png" 8 -paloffset 0x1200
psx_img_decolorize "out/rsrc/grp/BTCOM.png" "rsrc_raw/pal/battle_interface_mod.pal" "out/rsrc/grp/BTCOM_gray.png" 8
psx_rawimg_conv "out/rsrc/grp/BTCOM_gray.png" "out/rsrc/grp/BTCOM_gray.bin" 8
datpatch "out/rsrc_raw/gou/BTLETC/BTCOM.GS8" "out/rsrc_raw/gou/BTLETC/BTCOM.GS8" "out/rsrc/grp/BTCOM_gray.bin" 0x100
cp "out/rsrc_raw/gou/BTLETC/BTCOM.GS8" "out/files/DATA/BTCOM.GS8"

#=====
# 
#=====

remapPalette "out/rsrc/grp/STWIN.png" "out/rsrc/grp/orig/STWIN.png"
psx_img_decolorize "out/rsrc/grp/STWIN.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/STWIN_gray.png" 8 -paloffset 0
psx_rawimg_conv "out/rsrc/grp/STWIN_gray.png" "out/rsrc/grp/STWIN_gray.bin" 8
datpatch "out/rsrc_raw/gou/BTLETC/STWIN.GS8" "out/rsrc_raw/gou/BTLETC/STWIN.GS8" "out/rsrc/grp/STWIN_gray.bin" 0x100

remapPalette "out/rsrc/grp/IDO.png" "out/rsrc/grp/orig/IDO.png"
psx_img_decolorize "out/rsrc/grp/IDO.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/IDO_gray.png" 8 -paloffset 0
psx_rawimg_conv "out/rsrc/grp/IDO_gray.png" "out/rsrc/grp/IDO_gray.bin" 8
datpatch "out/files/MAP/IDO.GS8" "out/files/MAP/IDO.GS8" "out/rsrc/grp/IDO_gray.bin" 0x100

remapPalette "out/rsrc/grp/KAI.png" "out/rsrc/grp/orig/KAI.png"
psx_img_decolorize "out/rsrc/grp/KAI.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/KAI_gray.png" 8 -paloffset 0
psx_rawimg_conv "out/rsrc/grp/KAI_gray.png" "out/rsrc/grp/KAI_gray.bin" 8
datpatch "out/files/MAP/KAI.GS8" "out/files/MAP/KAI.GS8" "out/rsrc/grp/KAI_gray.bin" 0x100

remapPalette "out/rsrc/grp/DATA/OPT.CSA/image.png" "out/rsrc/grp/orig/DATA/OPT.CSA/image.png"
buildAceImgInPlace "out/rsrc/grp/DATA/OPT.CSA/" "image.png" "TCO" "GS8"
yuna3f_arcbuild "out/rsrc/grp/DATA/OPT.CSA/" "index.txt" "out/files/DATA/OPT.CSA"

remapPalette "out/rsrc/grp/DATA/TITSP.CSA/image.png" "out/rsrc/grp/orig/DATA/TITSP.CSA/image.png"
buildAceImgInPlace "out/rsrc/grp/DATA/TITSP.CSA/" "image.png" "TCO" "GS8"
yuna3f_arcbuild "out/rsrc/grp/DATA/TITSP.CSA/" "index.txt" "out/files/DATA/TITSP.CSA"

remapPalette "out/rsrc/grp/BTLETC/MPSEL.CSA/image.png" "out/rsrc/grp/orig/BTLETC/MPSEL.CSA/image.png"
buildAceImgInPlaceNoTrans "out/rsrc/grp/BTLETC/MPSEL.CSA/" "image.png" "TCO" "GS8"
yuna3f_arcbuild "out/rsrc/grp/BTLETC/MPSEL.CSA/" "index.txt" "out/rsrc_raw/gou/BTLETC/MPSEL.CSA"

yuna3f_ss1build "out/rsrc/grp/TITLE.png" "out/rsrc_raw/gou/PICTURE/TITLE.SS1"
yuna3f_ss1build "out/rsrc/grp/OMAKE.png" "out/rsrc_raw/gou/PICTURE/OMAKE.SS1"
yuna3f_ss1build "out/rsrc/grp/LOADING.png" "out/rsrc_raw/gou/PICTURE/LOADING.SS1"
yuna3f_ss1build "out/rsrc/grp/CHAP1.png" "out/rsrc_raw/gou/PICTURE/CHAP1.SS1"
yuna3f_ss1build "out/rsrc/grp/CHAP2.png" "out/rsrc_raw/gou/PICTURE/CHAP2.SS1"
yuna3f_ss1build "out/rsrc/grp/CHAP3.png" "out/rsrc_raw/gou/PICTURE/CHAP3.SS1"
yuna3f_ss1build "out/rsrc/grp/CHAP4.png" "out/rsrc_raw/gou/PICTURE/CHAP4.SS1"
yuna3f_ss1build "out/rsrc/grp/CHAP5.png" "out/rsrc_raw/gou/PICTURE/CHAP5.SS1"
yuna3f_ss1build "out/rsrc/grp/ITIRAN.png" "out/files/CARDS/ITIRAN.SS1"
# overwrite duplicate of LOADING in BTLETC
cp "out/rsrc_raw/gou/PICTURE/LOADING.SS1" "out/rsrc_raw/gou/BTLETC/LOADING.SS1"
yuna3f_ss1build "out/rsrc/grp/FUTA.png" "out/files/DATA/FUTA.SS1"

for file in out/rsrc/grp/CARDFILE/*.png; do
  name=$(basename $file .png)
  yuna3f_ss1build "$file" "out/rsrc_raw/gou/CARDFILE/$name.SS1"
done

for file in out/rsrc/grp/ENDING/*.png; do
  name=$(basename $file .png)
  yuna3f_ss1build "$file" "out/rsrc_raw/gou/ENDING/$name.SS1"
done

remapPalette "out/rsrc/grp/SENHYO.png" "out/rsrc/grp/orig/SENHYO.png"
yuna3f_aceimgbuild "out/rsrc/grp/SENHYO.png" "out/rsrc_raw/gou/BTLETC/SENHYO.TCO" "out/rsrc_raw/gou/BTLETC/SENHYO.GS8" "out/rsrc_raw/gou/BTLETC/SENHYO.GS8" --notrans

remapPalette "out/rsrc/grp/SYUKEI.png" "out/rsrc/grp/orig/SYUKEI.png"
yuna3f_aceimgbuild "out/rsrc/grp/SYUKEI.png" "out/rsrc_raw/gou/BTLETC/SYUKEI.TCO" "out/rsrc_raw/gou/BTLETC/SYUKEI.GS8" "out/rsrc_raw/gou/BTLETC/SYUKEI.GS8" --notrans

remapPalette "out/rsrc/grp/SENSEKI.png" "out/rsrc/grp/orig/SENSEKI.png"
yuna3f_aceimgbuild "out/rsrc/grp/SENSEKI.png" "out/rsrc_raw/gou/BTLETC/SENSEKI.TCO" "out/rsrc_raw/gou/BTLETC/SENSEKI.GS8" "out/rsrc_raw/gou/BTLETC/SENSEKI.GS8" --notrans

cp out/rsrc/grp/orig/vs_bg_map_* out/rsrc/grp
convert "out/rsrc/grp/vs_bg_map_1-img.png"\
  \( "out/rsrc/grp/vs_bg.png" -crop 128x128+0+0 +repage \) -geometry +0+0 -composite\
  \( "out/rsrc/grp/vs_bg.png" -crop 128x128+128+0 +repage \) -geometry +0+128 -composite\
  \( "out/rsrc/grp/vs_bg.png" -crop 64x128+256+0 +repage \) -geometry +128+0 -composite\
  \( "out/rsrc/grp/vs_bg.png" -crop 128x112+0+128 +repage \) -geometry +128+128 -composite\
  PNG32:out/rsrc/grp/vs_bg_map_1-img.png
convert "out/rsrc/grp/vs_bg_map_2-img.png"\
  \( "out/rsrc/grp/vs_bg.png" -crop 128x112+128+128 +repage \) -geometry +0+0 -composite\
  \( "out/rsrc/grp/vs_bg.png" -crop 64x112+256+128 +repage \) -geometry +0+128 -composite\
  PNG32:out/rsrc/grp/vs_bg_map_2-img.png
remapPalette "out/rsrc/grp/vs_bg_map_1-img.png" "out/rsrc/grp/orig/vs_bg_map_1-img.png"
remapPalette "out/rsrc/grp/vs_bg_map_2-img.png" "out/rsrc/grp/orig/vs_bg_map_2-img.png"
dandy_imggen "out/rsrc/grp/vs_bg_map_1" "out/rsrc/grp/vs_bg_map_1.tim"
dandy_imggen "out/rsrc/grp/vs_bg_map_2" "out/rsrc/grp/vs_bg_map_2.tim"
datpatch "out/rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "out/rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "out/rsrc/grp/vs_bg_map_1.tim" 0
datpatch "out/rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "out/rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "out/rsrc/grp/vs_bg_map_2.tim" 0x10220

remapPalette "out/rsrc/grp/ms/MS53/bg.png" "out/rsrc/grp/orig/ms/MS53/bg.png"
yuna3f_msbg_unview "out/rsrc_raw/arc/MSDATA/MS53/MS53.CEL" "out/rsrc_raw/arc/MSDATA/MS53/MS53.BGD" "out/rsrc/grp/ms/MS53/bg.png" --img1 "out/rsrc/grp/ms/MS53/image1-img.png" --boundw 40 --boundh 32
convert "out/rsrc/grp/ms/MS53/image1-img.png"\
  \( "out/rsrc/grp/orig/ms/MS53/image1-img.png" -crop 8x8+56+48 +repage \) -geometry +16+56 -composite\
  \( "out/rsrc/grp/orig/ms/MS53/image1-img.png" -crop 8x8+40+40 +repage \) -geometry +24+56 -composite\
  PNG32:out/rsrc/grp/ms/MS53/image1-img.png
dandy_imggen "out/rsrc/grp/ms/MS53/image1" "out/rsrc/grp/ms/MS53/ms53-bg1.tim"
cat "out/rsrc/grp/ms/MS53/ms53-bg1.tim" > "out/rsrc_raw/arc/MSDATA/MS53/MS53.TIM"
yuna3f_msbg53_patch "out/rsrc_raw/arc/MSDATA/MS53/MS53.BGD" "out/rsrc_raw/arc/MSDATA/MS53/MS53.BGD"
# copy graphic and repeat the above for MS78, the second team selection screen,
# which is identical
cp "out/rsrc/grp/ms/MS53/bg.png" "out/rsrc/grp/ms/MS78/bg.png"
cp "out/rsrc/grp/ms/MS53/image1-img.png" "out/rsrc/grp/ms/MS78/image1-img.png"
yuna3f_msbg_unview "out/rsrc_raw/arc/MSDATA/MS78/MS78.CEL" "out/rsrc_raw/arc/MSDATA/MS78/MS78.BGD" "out/rsrc/grp/ms/MS78/bg.png" --img1 "out/rsrc/grp/ms/MS78/image1-img.png" --boundw 40 --boundh 32
dandy_imggen "out/rsrc/grp/ms/MS78/image1" "out/rsrc/grp/ms/MS78/ms78-bg1.tim"
cat "out/rsrc/grp/ms/MS78/ms78-bg1.tim" > "out/rsrc_raw/arc/MSDATA/MS78/MS78.TIM"
yuna3f_msbg53_patch "out/rsrc_raw/arc/MSDATA/MS78/MS78.BGD" "out/rsrc_raw/arc/MSDATA/MS78/MS78.BGD"

remapPalette "out/rsrc/grp/ms/MS87/bg.png" "out/rsrc/grp/orig/ms/MS87/bg.png"
yuna3f_msbg_unview "out/rsrc_raw/arc/MSDATA/MS87/MS87.CEL" "out/rsrc_raw/arc/MSDATA/MS87/MS87.BGD" "out/rsrc/grp/ms/MS87/bg.png" --img1 "out/rsrc/grp/ms/MS87/image1-img.png" --boundw 50 --boundh 32
dandy_imggen "out/rsrc/grp/ms/MS87/image1" "out/rsrc/grp/ms/MS87/ms87-bg1.tim"
cat "out/rsrc/grp/ms/MS87/ms87-bg1.tim" > "out/rsrc_raw/arc/MSDATA/MS87/MS87.TIM"

psx_img_decolorize "out/rsrc/grp/BTLETC/EF14/patch.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/BTLETC/EF14/patch_gray.png" 8 -paloffset 0x600
psx_rawimg_conv "out/rsrc/grp/BTLETC/EF14/patch_gray.png" "out/rsrc/grp/BTLETC/EF14/patch_gray.bin" 8
datpatch "out/rsrc/grp/BTLETC/EF14/SPFONT" "out/rsrc/grp/BTLETC/EF14/SPFONT" "out/rsrc/grp/BTLETC/EF14/patch_gray.bin" 0x7C0
yuna3f_arcbuild "out/rsrc/grp/BTLETC/EF14/" "index.txt" "out/rsrc_raw/gou/BTLETC/EF14.A00"
cp "out/rsrc_raw/gou/BTLETC/EF14.A00" "out/files/BATMAP/EF14.A00"

psx_img_decolorize "out/rsrc/grp/BTLETC/EF17/patch.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "out/rsrc/grp/BTLETC/EF17/patch_gray.png" 8 -paloffset 0x600
psx_rawimg_conv "out/rsrc/grp/BTLETC/EF17/patch_gray.png" "out/rsrc/grp/BTLETC/EF17/patch_gray.bin" 8
datpatch "out/rsrc/grp/BTLETC/EF17/SPFONT" "out/rsrc/grp/BTLETC/EF17/SPFONT" "out/rsrc/grp/BTLETC/EF17/patch_gray.bin" 0x100
yuna3f_arcbuild "out/rsrc/grp/BTLETC/EF17/" "index.txt" "out/rsrc_raw/gou/BTLETC/EF17.A00"
# nope! the defense label that appears to be used is not, because
# this completely separate graphic from a completely separate sprite sheet is
# permanently cached during startup
convert "out/rsrc/grp/EFECT01.png"\
  "out/rsrc/grp/BTLETC/EF17/patch_gray.png" -geometry +144+96 -composite\
  PNG32:out/rsrc/grp/EFECT01.png
psx_rawimg_conv "out/rsrc/grp/EFECT01.png" "out/rsrc/grp/EFECT01.bin" 8
datpatch "out/rsrc_raw/gou/BTLETC/EFECT01.GS8" "out/rsrc_raw/gou/BTLETC/EFECT01.GS8" "out/rsrc/grp/EFECT01.bin" 0x100

echo "*******************************************************************************"
echo "Packing archives..."
echo "*******************************************************************************"

for file in out/rsrc_raw/arc/MSDATA/*; do
  if [ ! -f $file ]; then
    # filenames are limited to 8 characters
    baseFile=$(basename $file).BCT
    baseFile=${baseFile:0:8}
    
    baseDir=$(dirname $file)
    outArcName=$baseDir/$baseFile
    
#    echo "$file" "$baseFile" "$outArcName"
    yuna3f_arcbuild "$file/" "index.txt" "$outArcName"
  fi
done

# note that while this is named as if it were a GOU pack,
# it's actually a standard archive with enforced(?) sector alignment
yuna3f_arcbuild "out/rsrc_raw/arc/MSDATA/" "index.txt" "out/files/BATMAP/MSDATA.GOU" --sectoralign

yuna3f_goubuild "out/rsrc_raw/gou/BTLETC/" "index.txt" "out/files/BATMAP/BTLETC.GOU"
yuna3f_goubuild "out/rsrc_raw/gou/PICTURE/" "index.txt" "out/files/DATA/PICTURE.PIC"
yuna3f_goubuild "out/rsrc_raw/gou/CARDFILE/" "index.txt" "out/files/CARDS/CARDFILE.PIC"
yuna3f_goubuild "out/rsrc_raw/gou/ENDING/" "index.txt" "out/files/DATA/ENDING.PIC"

#exit

echo "*******************************************************************************"
echo "Replacing disused files..."
echo "*******************************************************************************"

# it's probably more trouble than it's worth to actually remove these
# entirely, so let's just do this
echo "dummy" > "out/files/DATA/YUNA3.FON"
echo "dummy" > "out/files/DATA/YUNA3.TBL"

echo "*******************************************************************************"
echo "Expanding EXE..."
echo "*******************************************************************************"

psx_exepad "out/files/SLPS_014.51" 0x100000 0 "out/files/SLPS_014.51"

echo "*******************************************************************************"
echo "Building ASM..."
echo "*******************************************************************************"

mkdir -p out/asm

# copy original executables
cp "out/files/SLPS_014.51" "out/asm"
cp "out/files/BIN/LONGMAP.BIN" "out/asm"
cp "out/files/BIN/OMAKE.BIN" "out/asm"
cp "out/files/BIN/BMS096.BIN" "out/asm"
cp "out/files/BIN/ALBUM.BIN" "out/asm"

# build  
$ARMIPS asm/SLPS_014.51.asm -temp out/asm/temp.txt -sym out/asm/symbols.sym -sym2 out/asm/symbols.sym2

# copy to output folder
cp "out/asm/SLPS_014.51" "out/files"
cp "out/asm/LONGMAP.BIN" "out/files/BIN"
cp "out/asm/OMAKE.BIN" "out/files/BIN"
cp "out/asm/BMS096.BIN" "out/files/BIN"
cp "out/asm/ALBUM.BIN" "out/files/BIN"

#exit

echo "*******************************************************************************"
echo "Generating disc layout report..."
echo "*******************************************************************************"

# "pre-build" disc to generate locations of output files
$DISCASTER "yuna3f.dsc" -f "out/discfiles.txt"

echo "*******************************************************************************"
echo "Generating file layout table..."
echo "*******************************************************************************"

yuna3f_makefiletable "out/discfiles.txt" "out/filetable.bin"

echo "*******************************************************************************"
echo "Building file table ASM..."
echo "*******************************************************************************"

mkdir -p out/asm

# copy original executables
cp "out/files/SLPS_014.51" "out/asm"

# build  
$ARMIPS asm/filetable.asm -temp out/asm/temp_filetable.txt -sym out/asm/symbols_filetable.sym -sym2 out/asm/symbols_filetable.sym2

# copy to output folder
cp "out/asm/SLPS_014.51" "out/files"

echo "*******************************************************************************"
echo "Building disc..."
echo "*******************************************************************************"

$DISCASTER "yuna3f.dsc"

echo "*******************************************************************************"
echo "Success!"
echo "*******************************************************************************"

set -o errexit

function unpackCsaGrp() {
  inName=$1
  outFolder=$2
  transOption=$3
  
  ./yuna3f_arcextr "$inName" "$outFolder/" "$outFolder/index.txt"
  
  tcoFileName=$outFolder/TCO
  gs8FileName=$outFolder/GS8
#  tanFileName="$outFolder/TAN"
  outFileName=$outFolder/image.png
  
  if [ ! -e "$tcoFileName" ]; then exit 1; fi
  if [ ! -e "$gs8FileName" ]; then exit 1; fi
#  if [ ! -e "$outFolder/TAN" ]; then exit 1; fi
  
  ./yuna3f_aceimgextr "$tcoFileName" "$gs8FileName" "$outFileName" $transOption
}

function dumpMissionBg() {
  inPrefix=$1
  celName=$2
  bgdName=$3
  timName=$4
  outPrefix=$5
  img1Offset=$6
  img2Offset=$7
  
  img1Name=
  if [ ! $img1Offset == "" ]; then
    ./dandy_imgextr "${inPrefix}$timName" "$outPrefix/image1" -offset "$img1Offset"
    img1Name=${outPrefix}image1-img.png
  fi;
  
  img2Name=
  if [ ! $img2Offset == "" ]; then
    ./dandy_imgextr "${inPrefix}$timName" "$outPrefix/image2" -offset "$img2Offset"
    img2Name=${outPrefix}image2-img.png
  fi;
  
  ./yuna3f_msbg_view "${inPrefix}$celName" "${inPrefix}$bgdName" "${outPrefix}bg.png" "$img1Name" "$img2Name"
}

make blackt
make libpsx
make

mkdir -p "rsrc/grp/orig"

# for file in rsrc_raw/gou/BTLETC/MST*.CSA; do
#   fName=$(basename $file)
# #  echo "$file"
#   unpackCsaGrp "$file" "rsrc/grp/orig/BTLETC/$fName"
# done

# ./psx_rawimg_extr "rsrc_raw/gou/BTLETC/BTCOM.GS8" "rsrc/grp/orig/BTCOM.png" 0x180 0x80 8 -i 0x100
# ./psx_img_colorize "rsrc/grp/orig/BTCOM.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTCOM.png" 8 -paloffset 0x1200

# #./yuna3f_gouextr "disc/files/BATMAP/EFFECT.TGO" "rsrc_raw/gou/EFFECT/" "rsrc_raw/gou/EFFECT/index.txt"

#./yuna3f_gouextr "disc/files/DATA/TALK.PIC" "rsrc_raw/gou/TALK/" "rsrc_raw/gou/TALK/index.txt"

# ./yuna3f_gouextr "disc/files/BATMAP/EFFECT.TGO" "rsrc_raw/gou/EFFECT/" "rsrc_raw/gou/EFFECT/index.txt"
# for file in rsrc_raw/gou/EFFECT/*.CSA; do
#   fName=$(basename $file)
#   unpackCsaGrp "$file" "rsrc/grp/orig/EFFECT/$fName"
# done

# ./yuna3f_aceimgextr "rsrc_raw/gou/BTLETC/SENHYO.TCO" "rsrc_raw/gou/BTLETC/SENHYO.GS8" "rsrc/grp/orig/SENHYO.png" --notrans
# ./yuna3f_aceimgextr "rsrc_raw/gou/BTLETC/SENSEKI.TCO" "rsrc_raw/gou/BTLETC/SENSEKI.GS8" "rsrc/grp/orig/SENSEKI.png" --notrans
# 
# ./psx_rawimg_extr "rsrc_raw/gou/BTLETC/STWIN.GS8" "rsrc/grp/orig/STWIN.png" 0x80 0xA0 8 -i 0x100
# ./psx_img_colorize "rsrc/grp/orig/STWIN.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/STWIN.png" 8 -paloffset 0x0

# ./psx_rawimg_extr "disc/files/MAP/IDO.GS8" "rsrc/grp/orig/IDO.png" 0x100 0x80 8 -i 0x100
# ./psx_img_colorize "rsrc/grp/orig/IDO.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/IDO.png" 8 -paloffset 0x0
# 
# ./psx_rawimg_extr "disc/files/MAP/KAI.GS8" "rsrc/grp/orig/KAI.png" 0x100 0x80 8 -i 0x100
# ./psx_img_colorize "rsrc/grp/orig/KAI.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/KAI.png" 8 -paloffset 0x0

#./psx_rawimg_extr "disc/files/CH02/KOCPBG.GS8" "test.png" 0x200 0x100 8 -i 0x100
#./psx_img_colorize "rsrc/grp/orig/KAI.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/KAI.png" 8 -paloffset 0x0

# unpackCsaGrp "disc/files/DATA/OPT.CSA" "rsrc/grp/orig/DATA/OPT.CSA"
# unpackCsaGrp "disc/files/DATA/TITSP.CSA" "rsrc/grp/orig/DATA/TITSP.CSA"

# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/TITLE.SS1" "rsrc/grp/orig/TITLE.png"
# ./yuna3f_ss1extr "rsrc_raw/saturn_files/TITLE.SS1" "rsrc/grp/orig/TITLE_saturn.png"

# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/CHAP1.SS1" "rsrc/grp/orig/CHAP1.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/CHAP2.SS1" "rsrc/grp/orig/CHAP2.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/CHAP3.SS1" "rsrc/grp/orig/CHAP3.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/CHAP4.SS1" "rsrc/grp/orig/CHAP4.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/CHAP5.SS1" "rsrc/grp/orig/CHAP5.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/LOADING.SS1" "rsrc/grp/orig/LOADING.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/PICTURE/OMAKE.SS1" "rsrc/grp/orig/OMAKE.png"
# ./yuna3f_ss1extr "rsrc_raw/gou/BTLETC/VS_SEL.SS1" "rsrc/grp/orig/VS_SEL.png"
# ./yuna3f_ss1extr "disc/files/DATA/FUTA.SS1" "rsrc/grp/orig/FUTA.png"

# unpackCsaGrp "rsrc_raw/gou/BTLETC/MPSEL.CSA" "rsrc/grp/orig/BTLETC/MPSEL.CSA" "--notrans"
#unpackCsaGrp "disc/files/DATA/ADVBG2.CS" "rsrc/grp/orig/ADVBG2.CS" "--notrans"

# ./yuna3f_aceimgextr "rsrc_raw/gou/BTLETC/SYUKEI.TCO" "rsrc_raw/gou/BTLETC/SYUKEI.GS8" "rsrc/grp/orig/SYUKEI.png" --notrans

# ./yuna3f_gouextr "disc/files/CARDS/CARDFILE.PIC" "rsrc_raw/gou/CARDFILE/" "rsrc_raw/gou/CARDFILE/index.txt"
# mkdir -p rsrc/grp/orig/CARDFILE
# for file in rsrc_raw/gou/CARDFILE/*.SS1; do
#   name=$(basename $file .SS1)
#   ./yuna3f_ss1extr "$file" "rsrc/grp/orig/CARDFILE/$name.png"
# done
# #./yuna3f_aceimgextr "disc/files/CARDS/SUUZI.TCO" "disc/files/CARDS/SUUZI.GS8" "rsrc/grp/orig/SUUZI.png"
# ./yuna3f_ss1extr "disc/files/CARDS/ITIRAN.SS1" "rsrc/grp/orig/ITIRAN.png"

#./yuna3f_gouextr "disc/files/BATMAP/DEMOCSA.GOU" "rsrc_raw/gou/DEMOCSA/" "rsrc_raw/gou/DEMOCSA/index.txt"
#unpackCsaGrp "rsrc_raw/gou/DEMOCSA/MS67A.CSA" "rsrc/grp/orig/test" "--notrans"

# # you have no idea how long it took me to work out that the REAL background for the
# # vs mode map select is randomly hidden in what should be the mission 41 map tile TIM
# ./dandy_imgextr "rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "rsrc/grp/orig/vs_bg_map_1" -offset 0
# ./dandy_imgextr "rsrc_raw/arc/MSDATA/MS41/ms41.TIM" "rsrc/grp/orig/vs_bg_map_2" -offset 0x10220
# convert -size 320x240 xc:transparent\
#   \( "rsrc/grp/orig/vs_bg_map_1-img.png" -crop 128x128+0+0 +repage \) -geometry +0+0 -composite\
#   \( "rsrc/grp/orig/vs_bg_map_1-img.png" -crop 128x128+0+128 +repage \) -geometry +128+0 -composite\
#   \( "rsrc/grp/orig/vs_bg_map_1-img.png" -crop 64x128+128+0 +repage \) -geometry +256+0 -composite\
#   \( "rsrc/grp/orig/vs_bg_map_1-img.png" -crop 128x112+128+128 +repage \) -geometry +0+128 -composite\
#   \( "rsrc/grp/orig/vs_bg_map_2-img.png" -crop 128x112+0+0 +repage \) -geometry +128+128 -composite\
#   \( "rsrc/grp/orig/vs_bg_map_2-img.png" -crop 64x112+0+128 +repage \) -geometry +256+128 -composite\
#   PNG32:rsrc/grp/orig/vs_bg.png

# ./dandy_imgextr "rsrc_raw/arc/MSDATA/MS53/MS53.TIM" "rsrc/grp/orig/teamsplit_bg" -offset 0

# mkdir -p rsrc/grp/orig/ms/MS53
# dumpMissionBg "rsrc_raw/arc/MSDATA/MS53/" "MS53.CEL" "MS53.BGD" "MS53.TIM" "rsrc/grp/orig/ms/MS53/" 0
# #mkdir -p rsrc/grp/orig/ms/MS41
# #dumpMissionBg "rsrc_raw/arc/MSDATA/MS41/" "ms41.CEL" "ms41.BGD" "ms41.TIM" "rsrc/grp/orig/ms/MS41/" 0 0x10220
# mkdir -p rsrc/grp/orig/ms/MS87
# dumpMissionBg "rsrc_raw/arc/MSDATA/MS87/" "MS87.CEL" "MS87.BGD" "MS87.TIM" "rsrc/grp/orig/ms/MS87/" 0
# mkdir -p rsrc/grp/orig/ms/MS78
# dumpMissionBg "rsrc_raw/arc/MSDATA/MS78/" "MS78.CEL" "MS78.BGD" "MS78.TIM" "rsrc/grp/orig/ms/MS78/" 0

# ./yuna3f_arcextr "rsrc_raw/gou/BTLETC/EF12.A00" "rsrc/grp/orig/BTLETC/EF12/" "rsrc/grp/orig/BTLETC/EF12/index.txt"
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF12/SPFONT" "rsrc/grp/orig/BTLETC/EF12/SPFONT_8.png" 8 1024 8
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF12/SPFONT" "rsrc/grp/orig/BTLETC/EF12/SPFONT_16.png" 16 512 8
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF12/SPFONT_8.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF12/SPFONT_8.png" 8 -paloffset 0x600
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF12/SPFONT_16.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF12/SPFONT_16.png" 8 -paloffset 0x600
# 
# ./yuna3f_arcextr "rsrc_raw/gou/BTLETC/EF14.A00" "rsrc/grp/orig/BTLETC/EF14/" "rsrc/grp/orig/BTLETC/EF14/index.txt"
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF14/SPFONT" "rsrc/grp/orig/BTLETC/EF14/SPFONT_8.png" 8 1024 8
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF14/SPFONT" "rsrc/grp/orig/BTLETC/EF14/SPFONT_16.png" 16 512 8
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF14/SPFONT_8.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF14/SPFONT_8.png" 8 -paloffset 0x600
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF14/SPFONT_16.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF14/SPFONT_16.png" 8 -paloffset 0x600
# 
# ./yuna3f_arcextr "rsrc_raw/gou/BTLETC/EF17.A00" "rsrc/grp/orig/BTLETC/EF17/" "rsrc/grp/orig/BTLETC/EF17/index.txt"
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF17/SPFONT" "rsrc/grp/orig/BTLETC/EF17/SPFONT_8.png" 8 1024 8
# ./psx_rawimg_extr "rsrc/grp/orig/BTLETC/EF17/SPFONT" "rsrc/grp/orig/BTLETC/EF17/SPFONT_16.png" 16 1024 8
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF17/SPFONT_8.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF17/SPFONT_8.png" 8 -paloffset 0x600
# ./psx_img_colorize "rsrc/grp/orig/BTLETC/EF17/SPFONT_16.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/BTLETC/EF17/SPFONT_16.png" 8 -paloffset 0x600

# ./psx_rawimg_extr "rsrc_raw/gou/BTLETC/EFECT01.GS8" "rsrc/grp/orig/EFECT01.png" 0x100 0x100 8 -i 0x100
#cp "rsrc/grp/orig/EFECT01.png" "rsrc/grp/orig/EFECT01_gray.png"
#./psx_img_colorize "rsrc/grp/orig/EFECT01.png" "rsrc_raw/gou/BTLETC/YUNA97.CLT" "rsrc/grp/orig/EFECT01.png" 8 -paloffset 0x600

./yuna3f_gouextr "disc/files/DATA/ENDING.PIC" "rsrc_raw/gou/ENDING/" "rsrc_raw/gou/ENDING/index.txt"
mkdir -p rsrc/grp/orig/ENDING
for file in rsrc_raw/gou/ENDING/*.SS1; do
  name=$(basename $file .SS1)
  ./yuna3f_ss1extr "$file" "rsrc/grp/orig/ENDING/$name.png"
done

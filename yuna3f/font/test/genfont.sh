
set -o errexit

function drawTextImg() {
  file=$1
  fontname=$2
  xoffset=$3
  yoffset=$4
  text=$5
  # fontsize is optional and defaults to 26
  fontsize=$6
  if [ "$fontsize" == "" ]; then
    fontsize=26
  fi
  
  echo "$file $fontname $xoffset $yoffset $text $fontsize"

  if [ ! -f $file ]; then
#    mkdir -p $(dirname $file)
#    convert -size 256x256 xc:black -alpha transparent "$file"
    convert -size 256x256 xc:black "$file"
#    convert -size 256x256 xc:#E8E0C8 "$file"
  fi
  
#  convert $file -font $fontname -pointsize $fontsize +antialias -fill white -gravity NorthWest -draw "text $xoffset,$yoffset '$text'" $file
  convert $file -font $fontname -pointsize $fontsize -antialias -fill white -gravity NorthWest -draw "text $xoffset,$yoffset '$text'" $file
#  convert $file -font $fontname -pointsize $fontsize -antialias -fill black -gravity NorthWest -draw "text $xoffset,$yoffset '$text'" $file
}

# generate black outline around non-transparent part of input image,
# for e.g. credits text
#
# $1 = input image
# $2 = output image
function outlineSolidPixels() {
  convert "$1" \( +clone -channel A -morphology EdgeOut Diamond -negate -threshold 0 -negate +channel +level-colors black \) -compose DstOver -composite "$2"
}

function renderFont() {
  outfile=$1
  fontname=$2
  fontsize=$3
  xBase=$4
  yBase=$5
  xSpacing=$6
  ySpacing=$7
  
  lineNum=0
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 0))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 1))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 2))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 3))) $(($yBase + ($ySpacing * $lineNum))) "" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 4))) $(($yBase + ($ySpacing * $lineNum))) "—" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 5))) $(($yBase + ($ySpacing * $lineNum))) "–" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 6))) $(($yBase + ($ySpacing * $lineNum))) "…" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 7))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 8))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 9))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  
  lineNum=1
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 0))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 1))) $(($yBase + ($ySpacing * $lineNum))) "!" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 2))) $(($yBase + ($ySpacing * $lineNum))) "\"" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 3))) $(($yBase + ($ySpacing * $lineNum))) "#" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 4))) $(($yBase + ($ySpacing * $lineNum))) "$" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 5))) $(($yBase + ($ySpacing * $lineNum))) "%" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 6))) $(($yBase + ($ySpacing * $lineNum))) "&" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 7))) $(($yBase + ($ySpacing * $lineNum))) "’" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 8))) $(($yBase + ($ySpacing * $lineNum))) "(" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 9))) $(($yBase + ($ySpacing * $lineNum))) ")" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) "*" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) "+" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "," $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "-" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) "." $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) "/" $fontsize
  
  lineNum=2
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 0))) $(($yBase + ($ySpacing * $lineNum))) "0" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 1))) $(($yBase + ($ySpacing * $lineNum))) "1" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 2))) $(($yBase + ($ySpacing * $lineNum))) "2" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 3))) $(($yBase + ($ySpacing * $lineNum))) "3" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 4))) $(($yBase + ($ySpacing * $lineNum))) "4" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 5))) $(($yBase + ($ySpacing * $lineNum))) "5" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 6))) $(($yBase + ($ySpacing * $lineNum))) "6" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 7))) $(($yBase + ($ySpacing * $lineNum))) "7" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 8))) $(($yBase + ($ySpacing * $lineNum))) "8" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 9))) $(($yBase + ($ySpacing * $lineNum))) "9" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) ":" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) ";" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "<" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "=" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) ">" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) "?" $fontsize
  
  lineNum=3
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  0))) $(($yBase + ($ySpacing * $lineNum))) "@" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  1))) $(($yBase + ($ySpacing * $lineNum))) "A" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  2))) $(($yBase + ($ySpacing * $lineNum))) "B" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  3))) $(($yBase + ($ySpacing * $lineNum))) "C" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  4))) $(($yBase + ($ySpacing * $lineNum))) "D" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  5))) $(($yBase + ($ySpacing * $lineNum))) "E" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  6))) $(($yBase + ($ySpacing * $lineNum))) "F" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  7))) $(($yBase + ($ySpacing * $lineNum))) "G" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  8))) $(($yBase + ($ySpacing * $lineNum))) "H" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  9))) $(($yBase + ($ySpacing * $lineNum))) "I" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) "J" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) "K" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "L" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "M" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) "N" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) "O" $fontsize
  
  lineNum=4
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  0))) $(($yBase + ($ySpacing * $lineNum))) "P" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  1))) $(($yBase + ($ySpacing * $lineNum))) "Q" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  2))) $(($yBase + ($ySpacing * $lineNum))) "R" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  3))) $(($yBase + ($ySpacing * $lineNum))) "S" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  4))) $(($yBase + ($ySpacing * $lineNum))) "T" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  5))) $(($yBase + ($ySpacing * $lineNum))) "U" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  6))) $(($yBase + ($ySpacing * $lineNum))) "V" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  7))) $(($yBase + ($ySpacing * $lineNum))) "W" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  8))) $(($yBase + ($ySpacing * $lineNum))) "X" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  9))) $(($yBase + ($ySpacing * $lineNum))) "Y" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) "Z" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) "[" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "\\" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "]" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) "‘" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) "_" $fontsize
  
  lineNum=5
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  0))) $(($yBase + ($ySpacing * $lineNum))) "\`" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  1))) $(($yBase + ($ySpacing * $lineNum))) "a" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  2))) $(($yBase + ($ySpacing * $lineNum))) "b" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  3))) $(($yBase + ($ySpacing * $lineNum))) "c'" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  4))) $(($yBase + ($ySpacing * $lineNum))) "d" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  5))) $(($yBase + ($ySpacing * $lineNum))) "e" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  6))) $(($yBase + ($ySpacing * $lineNum))) "f" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  7))) $(($yBase + ($ySpacing * $lineNum))) "g" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  8))) $(($yBase + ($ySpacing * $lineNum))) "h" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  9))) $(($yBase + ($ySpacing * $lineNum))) "i" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) "j" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) "k" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "l" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "m" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) "n" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) "o" $fontsize
  
  lineNum=6
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  0))) $(($yBase + ($ySpacing * $lineNum))) "p" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  1))) $(($yBase + ($ySpacing * $lineNum))) "q" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  2))) $(($yBase + ($ySpacing * $lineNum))) "r" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  3))) $(($yBase + ($ySpacing * $lineNum))) "s" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  4))) $(($yBase + ($ySpacing * $lineNum))) "t" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  5))) $(($yBase + ($ySpacing * $lineNum))) "u" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  6))) $(($yBase + ($ySpacing * $lineNum))) "v" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  7))) $(($yBase + ($ySpacing * $lineNum))) "w" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  8))) $(($yBase + ($ySpacing * $lineNum))) "x" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing *  9))) $(($yBase + ($ySpacing * $lineNum))) "y" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 10))) $(($yBase + ($ySpacing * $lineNum))) "z" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 11))) $(($yBase + ($ySpacing * $lineNum))) "“" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 12))) $(($yBase + ($ySpacing * $lineNum))) "|" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 13))) $(($yBase + ($ySpacing * $lineNum))) "”" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 14))) $(($yBase + ($ySpacing * $lineNum))) "’" $fontsize
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 15))) $(($yBase + ($ySpacing * $lineNum))) " " $fontsize
  
  lineNum=7
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 1))) $(($yBase + ($ySpacing * $lineNum))) "The quick brown fox jumps over the lazy dog" $fontsize
  
  lineNum=8
  drawTextImg $outfile $fontname $(($xBase + ($xSpacing * 1))) $(($yBase + ($ySpacing * $lineNum))) "Yuna Kagurazaka" $fontsize
  
#  convert $outfile -dither none -remap palette.png PNG32:$outfile
#  convert $outfile -dither Riemersma -remap palette.png PNG32:$outfile
#  convert $outfile -dither FloydSteinberg -remap palette.png PNG32:$outfile
}

#convert -size 256x192 xc: -alpha transparent -font Teko-SemiBold -pointsize 26 -fill white -gravity North -draw "text 0,0 'Test'" -draw "text 0,25 'Test2'" test_out3.png

#drawTextImg  0 Teko-SemiBold   0  38  "Producer"

rm -f test.png
#renderFont "test.png" "Smilecomix" 14 0 0 16 16
#renderFont "test.png" "Loosey-Sans" 18 0 -2 16 16
#renderFont "test.png" "Polsyh" 18 0 -2 16 16

#renderFont "test.png" "Stanberry-Regular" 18 0 0 16 16

#renderFont "test.png" "Coyotris-Comic" 22 0 -2 16 16
#renderFont "test.png" "HelvetiHand" 18 0 0 16 16
#renderFont "test.png" "Spork" 18 0 0 16 16
#renderFont "test.png" "DCC-Scisor" 20 0 0 16 16
#renderFont "test.png" "AmateurComic" 18 0 0 16 16
#renderFont "test.png" "ComickBook-Simple" 18 0 0 16 16

#renderFont "test.png" "Nimbus-Roman-No9-L" 16 0 0 16 16
#renderFont "test.png" "URW-Palladio-L-Roman" 16 0 0 16 16
#renderFont "test.png" "Linux-Libertine" 18 0 0 16 16
#renderFont "test.png" "Day-Roman" 16 0 0 16 16
#renderFont "test.png" "Nilland" 16 0 0 16 16

#renderFont "test.png" "BodoniXT" 16 0 0 16 16

#renderFont "test.png" "Timeless-Normal" 16 0 0 16 16
#renderFont "test.png" "GeosansLight" 16 0 0 16 16
#renderFont "test.png" "Aubrey" 16 0 0 16 16

#renderFont "test.png" "KenyanCoffeeRg-Regular" 16 0 0 16 20
#renderFont "test.png" "KenyanCoffeeRg-Regular" 15 0 0 16 20

#renderFont "test.png" "DreamOrphans-Regular" 14 0 0 16 16
#renderFont "test.png" "Walkway-Condensed-Bold" 16 0 0 16 16

#renderFont "test.png" "Borg" 18 0 0 16 16

#renderFont "test.png" "Folks-Normal" 16 0 0 16 16

#renderFont "test.png" "LibelSuitRg-Regular" 15 0 0 16 16

#renderFont "test.png" "SteelfishRoundedRg-Bold" 15 0 0 16 16

#renderFont "test.png" "Bitstream-Charter" 16 0 0 16 16
#renderFont "test.png" "OldSansBlack" 14 0 0 16 16
#renderFont "test.png" "Sansation-Regular" 14 0 0 16 16

#renderFont "test.png" "DejaVu-Serif-Bold-Italic" 14 0 0 16 16
#renderFont "test.png" "Sansation-Bold-Italic" 14 0 0 16 16
#renderFont "test.png" "Sansation-Bold" 14 0 0 16 16
#renderFont "test.png" "Cabin-SemiBold" 15 0 0 16 16
#renderFont "test.png" "Cabin-SemiBold-Italic" 15 0 0 16 16

#renderFont "test.png" "Cabin-SemiBold-Italic" 12 0 0 16 16
#renderFont "test.png" "Cabin-SemiBold-Italic" 12 0 0 12 12
# old
#renderFont "test.png" "Cabin-SemiBold" 12 0 0 12 12

#renderFont "test.png" "Bookman-Demi" 12 0 0 12 12

#renderFont "test.png" "Helvetica-Narrow" 12 0 0 12 12
#renderFont "test.png" "Helvetica-Narrow-Bold" 11 0 0 12 12
#renderFont "test.png" "Helvetica-Bold" 12 0 0 12 12
#renderFont "test.png" "Helvetica-Bold" 11 0 0 12 12
#renderFont "test.png" "Helvetica-Narrow-Bold" 12 0 0 12 12
#renderFont "test.png" "Helvetica-Narrow" 11 0 0 12 12
#renderFont "test.png" "Helvetica-Narrow-Bold" 14 0 0 16 16
#renderFont "test.png" "Helvetica" 12 0 0 12 12

#renderFont "test.png" "DejaVu-Sans-Bold" 12 0 0 12 12
#renderFont "test.png" "Droid-Sans" 11 0 0 12 12
#renderFont "test.png" "FreeSans-Bold" 12 0 0 12 12
#renderFont "test.png" "Nimbus-Sans-L-Bold" 11 0 0 12 12
#renderFont "test.png" "Nimbus-Sans-L-Bold-Condensed" 12 0 0 12 12
#renderFont "test.png" "Nimbus-Sans-L" 12 0 0 12 12
#renderFont "test.png" "Nimbus-Sans-L-Bold-Condensed" 11 0 0 12 12
renderFont "test.png" "Nimbus-Sans-L-Bold" 12 0 0 16 16

# don't use these until black background has been removed...
#convert test.png -dither none -remap palette.png PNG32:test.png
#convert remap.png -dither none -remap palette.png PNG32:remap.png
#convert test.png -dither none -remap sheet.png PNG32:test_remap.png



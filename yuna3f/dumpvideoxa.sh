set -o errexit

make blackt
make libpsx
make psx_xa_decode

mkdir -p "xa/video"

for file in disc/rawfiles/MOVIE/*.STR; do
  echo $file
  baseFileName=$(basename $file)
  outFileName=$(basename $baseFileName .STR).wav
  
  ./psx_xa_decode "$file" "xa/video/$outFileName" -i 7
done
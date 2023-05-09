
set -o errexit

cd script

#soffice --headless --convert-to csv --infilter=CSV:44,34,64 script_main.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_main.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_mapscript.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_new.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_system.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_video.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_xa.ods

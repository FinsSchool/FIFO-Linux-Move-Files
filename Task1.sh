sort -u CleanFile.txt -o CleanFile.txt
grep -v '[[:punct:]]' CleanFile.txt > CleanFile.txt.temp
mv CleanFile.txt.temp CleanFile.txt
grep -v '[[:space:]]' CleanFile.txt > CleanFile.txt.temp
mv CleanFile.txt.temp CleanFile.txt
grep -v '[[:digit:]]' CleanFile.txt > CleanFile.txt.temp
mv CleanFile.txt.temp CleanFile.txt
grep -v '[[:upper:]]' CleanFile.txt > CleanFile.txt.temp
mv CleanFile.txt.temp CleanFile.txt
grep -E '^.{3,15}$' CleanFile.txt > CleanFile.txt.temp
mv CleanFile.txt.temp CleanFile.txt
sort -k1.3,1.15 CleanFile.txt -o CleanFile.txt
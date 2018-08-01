#!/bin/bash

INKSCAPE=/cygdrive/c/Program\ Files/Inkscape/inkscape.exe

for i in `cat icons.svg |grep -o -e '%%[^%]*%%'|sed 's/%//g'` 
do
 echo "$INKSCAPE" `cygpath -w \`pwd\`/icons.svg` -z -e `cygpath -w \`pwd\`/$i.png` -i $i
 rm  -f $i.png
 "$INKSCAPE" `cygpath -w \`pwd\`/icons.svg` -z -e `cygpath -w \`pwd\`/$i.png` -i $i
done

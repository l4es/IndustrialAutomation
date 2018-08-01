#!/bin/bash

INKSCAPE=inkscape

for i in `cat icons.svg |grep -o -e '%%[^%]*%%'|sed 's/%//g'` 
do
 echo "$INKSCAPE" icons.svg -z -e $i.png -i $i
 rm  -f $i.png
 "$INKSCAPE" icons.svg -z -e $i.png -i $i
done

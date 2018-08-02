#! /bin/sh
# this script is used in EiC's binary distribution
# It sets up all eic scripts so that #!/dir/eic points
# to eic 

usage='
  echo ""
  echo "    usage: batch2.sh <EiC-install-directory>"
  echo "          batch2.sh /usr/bin"
  echo " "

'

if [ $# -eq 0 ] ; then
  eval "$usage";
  exit 1;
fi

for i in `find . \( -name  "*.eic" -o -name "*.eic.cgi" \) -print`
do
        echo "$i -> $i.tmp -> $i"
	tail +2 $i > $i.tmp
	echo "#!$1/eic -f" > $i
	cat $i.tmp >> $i
	rm $i.tmp
done




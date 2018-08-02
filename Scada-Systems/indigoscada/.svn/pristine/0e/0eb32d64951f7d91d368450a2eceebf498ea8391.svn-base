usage='
  echo ""
  echo "    usage: batch.sh C-files"
  echo " "

'

if [ $# -eq 0 ] ; then
  eval "$usage";
  exit 1;
fi

for arg
do
	#name=`expr $arg : '\(.*\)\.tmp' \| $arg`
        echo "$arg -> $arg.tmp -> $arg"
	sed 's/inst_t/InsT_t/g' $arg > $arg.tmp
	mv $arg.tmp $arg
done


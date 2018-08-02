echo "in EiCtests"
cd EiCtests
../runtest test*.c
cd ..

echo "in eicScripts"
cd eicScripts
../runtest Drive*.c
cd ..

echo "in sniptests"
cd sniptests
../runtest snip?.c
cd ..

echo "in gnutests"
cd gnutests
../runtest *.c
cd ..

echo "in lcctests"
cd lcctests
../runtest tst*.c
cd ..

echo "in testcode"
cd testcode
../runtest *.c
cd ..

echo "in posix.1"
cd posix.1
../runtest tst*.c
cd ..

cd ../liboapc
make -s clean
make -s -w
make -s -f Makefilewx clean
make -s -w -f Makefilewx
cd ../ControlRoom

make -s clean
make -s -w
strip OpenEditor

make -s -f Makedebugger clean
make -s -w -f Makedebugger
strip OpenDebugger

make -s -f Makeplayer clean
make -s -w -f Makeplayer 
strip OpenPlayer

make -s -f Makehplayer clean
make -s -w -f Makehplayer 
strip OpenHPlayer

make -s -f Makeplugger clean
make -s -w -f Makeplugger 
strip OpenPlugger

cd luaPLC
make -s clean
make -s -w
strip luaPLC
cd ..

cd ilPLC
make -s clean
make -s -w
strip ilPLC
cd ..

cd IServer
make -s clean
make -s -w
strip OpenIServer
cd ..

# here we are back in ControlRoom directory

cd ../plugins
make -s -w
cd ..

cd ControlRoom/
# ...and end up in ControlRoom directory where we started



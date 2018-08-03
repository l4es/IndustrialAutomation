#!/bin/bash
# this is meant to be run from root project folder
#cd ../..

set -m # Enable Job Control

./configure --disable-ui > configure.out
make clean > /dev/null
make > make.out
if [ ! -f plcpipe ];
then
    mkfifo plcpipe
fi

#should be able to load a valid ld program
#and execute it with consistent results

cp tst/black/ld_random_file/sim.in .
cp tst/black/ld_random_file/sim.out .

(./plcemu -c tst/black/ld_random_file/plc.config -i tst/black/ld_random_file/plc.init > plc.out )& 
sleep 2
killall -w plcemu 
diff <(head -n 2 ./sim.out) <(head -n 2 tst/black/ld_random_file/sim.out)
diff -I 'PLC-EMU.*' <(head -n 85 plc.out) <(head -n 85 tst/black/ld_random_file/plc.out)

#triple majority circuit in LD

cp tst/black/ld_majority_circuit/sim.in .
cp tst/black/ld_majority_circuit/sim.out .

(./plcemu -c tst/black/ld_majority_circuit/plc.config -i tst/black/ld_majority_circuit/plc.init > plc.out) & 
sleep 2
killall -w plcemu 
diff <(head -n 5 sim.out) <(head -n 5 tst/black/ld_majority_circuit/sim.out)
diff -I 'PLC-EMU.*' <(head -n 100 plc.out) <(head -n 100 tst/black/ld_majority_circuit/plc.out)

#greatest common divisor in IL

cp tst/black/il_gcd/sim.in .
cp tst/black/il_gcd/sim.out .

(./plcemu -c tst/black/il_gcd/plc.config -i tst/black/il_gcd/plc.init > plc.out )& 
sleep 2
killall -w plcemu 
diff <(head -n 3 sim.out) <(head -n 3 tst/black/il_gcd/sim.out)
diff -I 'PLC-EMU.*' <(head -n 90 plc.out) <(head -n 90 tst/black/il_gcd/plc.out)

#greatest common divisor in IL

cp tst/black/il_knuth/sim.in .
cp tst/black/il_knuth/sim.out .

(./plcemu -c tst/black/il_knuth/plc.config -i tst/black/il_knuth/plc.init > plc.out )& 
sleep 2
killall -w plcemu 
diff <(head -n 3 sim.out) <(head -n 3 tst/black/il_gcd/sim.out)
diff -I 'PLC-EMU.*' <(head -n 700 plc.out) <(head -n 700 tst/black/il_gcd/plc.out)




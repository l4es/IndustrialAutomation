dim n

Randomize
n = Int((65535 * Rnd) + 1)'Generate random value between 1 and 65535.
n = n - 32767             ' some signed values to test with :-)
SetRegisterValue 3, (270*Rnd), n



Elmo Gold Solo Whistle
======================

This example uses the Elmo Gold Solo Whistle for motor control. The PDO configuration is:

* Torque control of the motor
* Encoder for position of the motor
* Encoder for position of the output (or other auxilary position)
* Analog input

The header can be reconfigured for digital inputs/outputs, note that the some indices and array sizes need to be changed.


Setup
-----

Before using the [Elmo Gold Solo Whistle](http://www.elmomc.com/products/whistle-servo-drive-gold.htm), perform the following steps:

1) Connect STO header to enable operation
2) Connect 24V supply, motor, and commutation encoder (optional)
3) Download ELmo Application Studio (EAS) II
4) Connect the drive via USB, and connect in EAS
5) Follow the quick setup steps, setting motor into torque mode
6) Optionally configure dual axis to read from two encoders.
   NOTE: In the Parameter Explorer panel, set Commuation>CA>CA[79] = 2 (i.e. additional position select is socket 2).
7) Enable the digital inputs if you are using them (also requires changes to "GoldWhistle.h").


Initialization
--------------

sudo ethercat download 0x6072 00 1000 # set max torque to 100% of rated torque
sudo ethercat download 0x6073 00 1000 # set max current to 100% of of rated current
sudo ethercat download 0x6075 00 3210 # set rated current to 3.21 A
sudo ethercat download 0x6076 00 128  # set rated torque to 128 mNm


Build
-----

    mkdir build
    cmake ..
    make -j3


Run
---

    sudo ./elmo_gold_whistle

Beckhof SSI Encoder
===================

This example was written for the Avago T6600 magnetic encoder, using a differential driver/receiver chip (TI SN75ALS180) to make it compatible with the Beckhoff EL5002. The EL9505 provides 5V to the chipset. NOTE: The EL5002 should be positioned before the EL9050, because of the bus voltages.

Initialization
--------------

For additional info, see the [EL5002 Documentation](https://download.beckhoff.com/download/document/io/ethercat-terminals/el500xen.pdf).

sudo ethercat download 0x8000 0x03 1    # enable inhibit time
sudo ethercat download 0x8000 0x12 16   # SSI data length
sudo ethercat download 0x8000 0x13 50   # min. inhibit time [µs]

Build
-----

    mkdir build
    cmake ..
    make -j3

Run
---

    sudo ./beckhoff_ssi_encoder

[https://download.beckhoff.com/download/document/io/ethercat-terminals/el500xen.pdf]

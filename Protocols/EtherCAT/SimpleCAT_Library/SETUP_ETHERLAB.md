Setting up EtherLab
===================

This guide explains how to setup [EtherLab] on Linux, specifically for Ubuntu 14.04 LTS.  
This is not required by SimpleCAT.  

EtherLab has the following benefits: 

* Control scheme is written in simulink
* Simulink blocks for Beckhoff and other devices included
* Real time executable generated which runs on the kernel.  
* TCP server for data access
* Plotting and value editing GUI

Boot the computer into the kernel you will be using with EtherLab  
(In the grub menu, choose the advanced option, then choose the kernel)

Follow the steps in the [Symbitron EtherLab Installation].  
Below are some helpful hints for installing on Ubuntu 14.04.


Installation of EtherLab
------------------------
[EtherLab documentation]  

A. To install the dependencies use:

```sh
sudo apt-get install liblog4cplus-dev libcommoncpp2-dev libyaml-dev libx11-dev libfftw3-dev libfltk1.3-dev autoconf automake doxygen
```

B. Any commands for "chown" and "chmod", change to "chown -R" and "chmod -R"

C. The link to the ec_slav3.c is broken. Instead use:

```
wget http://lists.etherlab.org/pipermail/etherlab-users/attachments/20140402/cf83e324/attachment.c
```

D. For kernal 3.17+, error 'macro "alloc_netdev" requires 4 arguments, but only 3 given' (Kernel 3.17+)  
   Add the fourth argument, "NET_NAME_UNKNOWN", in the following two files:  
   /usr/local/etherlab/src/ethercat-1.5.2/devices/generic.c  
   /usr/local/etherlab/src/ethercat-1.5.2/master/ethercat.c  
   
```c++
alloc_netdev(sizeof(ec_gen_device_t *), &null, NET_NAME_UNKNOWN, ether_setup);
```

E. Simulink build gives an error about /etc/ld/ -lethercat. Add a link to the lib.

```sh
sudo ln -s /usr/local/etherlab/lib/libethercat.so /usr/lib/
sudo ln -s /usr/local/etherlab/lib/libethercat.so.1 /usr/lib/
sudo ldconfig
```

Installation of EtherCAT Master
-------------------------------

A. Ethernet driver e1000e not supported for kernels 4.X

```sh
./configure --prefix=/usr/local/etherlab  --disable-8139too --enable-generic
```

B. Make a symbolic link to the ethercat master.
   Ubuntu does not look in /usr/local/bin/, instead use

```sh
sudo ln -s /usr/local/etherlab/bin/ethercat /usr/bin/
```

C. Ubuntu has no folder /etc/sysconfig. Just make one.

```sh
mkdir /etc/sysconfig
```

D. Edit the configuration file. Use the following:

```
MASTER0_DEVICE="ff:ff:ff:ff:ff:ff"  # or replace ff... with the mac address
DEVICE_MODULES="generic"
```

Installing Data Logging Service
-------------------------------

If insserv no such file or directory, then:

```sh
ln -s /usr/lib/insserv/insserv /sbin/insserv
```

Installing MATLAB
-----------------

If ecrt_support.c is not found, then add the following to your Matlab path.  
(Modify path via: Matlab->Home tab->set path)

```
/usr/local/etherlab/rtw/src
```


[EtherLab]:http://www.etherlab.org/en/what.php
[EtherLab download link]:http://www.etherlab.org/en/ethercat/index.php  
[EtherLab documentation]:http://etherlab.org/download/ethercat/ethercat-1.5.2.pdf  
[Symbitron EtherLab Installation]:https://www.symbitron.eu/wiki/index.php?title=EtherLab_Installation#Installing_the_IgH_EtherCAT_master_1.5.2


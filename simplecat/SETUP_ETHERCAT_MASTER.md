Setting up EtherCAT Master
===================

This guide explains how to setup EtherCAT master on Linux, specifically for Ubuntu 14.04 LTS.  
SimplECAT requires only the [IgH EtherCAT Master], which is part of EtherLab.  
For a full installion of EtherLab, see [EtherLab Setup].

First, boot the computer into the kernel you will be using with EtherLab  
(In the grub menu, choose the advanced option, then choose the kernel)

Installation of EtherCAT Master
-------------------------------
[EtherCAT documentation]  
[EtherCAT download link]  

** Automated installation of EtherCAT Master **

Follow the steps in the [Synapticon Master Software].  
This has a bash script that will complete the installation

** Manual installation of EtherCAT Master  **

Follow the steps in the [Symbitron EtherLab Installation].  
Here are some helpful hints for installing on Ubuntu 14.04.3.  

1. Ethernet driver e1000e not supported for kernels 4.X

        ./configure --prefix=/usr/local/etherlab  --disable-8139too --enable-generic

2. Make a symbolic link to the ethercat master.
   Ubuntu does not look in /usr/local/bin/, instead use

        sudo ln -s /usr/local/etherlab/bin/ethercat /usr/bin/

3. Ubuntu has no folder /etc/sysconfig. Just make one.

        mkdir /etc/sysconfig

4. Edit the configuration file. Use the following:

        MASTER0_DEVICE="ff:ff:ff:ff:ff:ff"  # or replace ff... with the mac address
        DEVICE_MODULES="generic"


EtherCat Master Commands
------------------------
**Start, stop, or check the ethercat master**

Optionally, make a link the EtherCAT master

     ln -s /etc/init.d/ethercat /usr/bin/ethercat_master
     sudo ethercat_master start
     sudo ethercat_master stop
     sudo ethercat_master status

**List operational status of connected slaves**

Optionally, make a link the EtherCAT client

     sudo ln -s /usr/local/etherlab/bin/ethercat /usr/bin/
     sudo ethercat slaves

**List the PDO's of connected slaves**

     sudo ethercat pdos

**To print the PDO's as a c struct**

     sudo ethercat cstruct
     

[IgH EtherCAT Master]:http://www.etherlab.org/en/ethercat/
[EtherLab Setup]:https://bitbucket.org/bsoe/simplecat/src/master/SETUP_ETHERLAB.md
[EtherCAT download link]:http://www.etherlab.org/en/ethercat/index.php  
[EtherCAT documentation]:http://etherlab.org/download/ethercat/ethercat-1.5.2.pdf  
[Synapticon Master Software]:https://doc.synapticon.com/tutorials/ethercat_master_software/index.html
[Symbitron EtherLab Installation]:https://www.symbitron.eu/wiki/index.php?title=EtherLab_Installation#Installing_the_IgH_EtherCAT_master_1.5.2


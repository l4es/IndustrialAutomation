Setting up a hard or soft real-time system
==========================================

This guide explains how to setup a hard and soft real-time (RT) systems on Linux,  
specifically for Ubuntu 14.04 LTS.  
For an explanation on hard vs soft real-time (rt) sytem,  
please see [UbuntuStudio/RealTimeKernel].  

Benefits of soft real-time: 

* High priority or real time thread can be good enough
* Graphics drivers work as normal  
* Does not sacrifice power saving features  
* Easy to install  

Benefits of hard real-time: 

* Guarentee of timing loop within a small margin
* Derivatives will therefore be less noisy
* Used in commercial robots, so it must be important


Installing low latency kernel (soft RT)
----------------------------
You may want to install the same version as your current kernel ($ uname -a). 
This assumes are using Ubuntu 14.04.03 LTS, with Kernel 3.19.0

    cd ~
    apt-get download linux-headers-3.19.0-25
    apt-get download linux-headers-3.19.0-25-lowlatency 
    apt-get download linux-image-3.19.0-25-lowlatency
    sudo dpkg -i *.deb
    sudo update-grub

If the grub menu does not show at bootup, then see the section below.


Installing RT Preempt kernel (hard RT)
---------------------------

### Automatic install ###

[https://capocasa.net/realtime-kernels](https://capocasa.net/realtime-kernels)

### Manual install ###

These instructions are based upon the [rt kernel wiki article] and the [kernelnewbies article].  

1. Install a [mainstream kernel] version that matches an [RT kernel version], preferably one with [long term support].
   It is safer to use a kernel version higher than the default Ubuntu Kernel ($ uname -a).  
   The following assumes the current computer runs Ubuntu 14.04.3 (kernel 3.19.0), and the desired RT kernel is 4.1.27.  

        cd ~
        wget ftp://ftp.kernel.org/pub/linux/kernel/v4.x/linux-4.1.27.tar.gz
        tar -xvzf linux-4.1.27.tar.gz

2. download the corresponding realtime patch to your home folder:

        cd ~
        wget https://www.kernel.org/pub/linux/kernel/projects/rt/4.1/patch-4.1.27-rt31.patch.xz
        cd linux-4.1.27
        xzcat ../patch-4.1.27-rt31.patch.xz | patch -p1

3. Copy the config from the closest generic (or low latency) version kernel config file

        cp /boot/config-3.19.0-25-generic .config

4. Open up the config, and make sure the following are set to yes  
   In Kernel 2.6.x:
        
        CONFIG_PREEMPT=y
        CONFIG_PREEMPT_RT=y
   In Kernel 3.x and 4.x:
        
        CONFIG_PREEMPT=y  
        CONFIG_PREEMPT_RT_BASE=y  
        CONFIG_PREEMPT_RT_FULL=y  
        CONFIG_HIGH_RES_TIMERS=y  

5. Now install:

        make -j3
        sudo make modules_install install        
   During the installation prompts, pick fully preemptible rt. Use enter to leave all else at default. 
   If you encounter a build error where warnings are treated as errors, then in the makefile, comment out "-Werror-implicit-....".

6. Finally, update the grub menu. If the grub menu does not show at bootup, see the section below.

        sudo update-grub

Note: Do not delete the built kernel. If you really want to delete it:

1. first copy it to /usr/src/  
2. redo the links in /lib/modules/4.1.7-rt8/
3. delete the kernel in /home/

Like so:

    cd ~
    sudo cp -avr ./linux-4.1.7/  /usr/src/linux-4.1.7/
    cd /lib/modules/4.1.7-blah
    sudo rm -vf build source
    ln -s /usr/src/linux-4.1.7/ build
    ln -s /usr/src/linux-4.1.7/ source
    rm -rf ~/linux-4.1.7/


Displaying the Grub Menu at Startup
-----------------------------------

The grub menu is hidden by default (unless setup for dual boot).  
To show the grub menu at startup, so that the kernel can be chosen:  

    sudo gedit /etc/default/grub

Comment out the following line (Add a "#" to the beginning):

    GRUB_HIDDEN_TIMER

Setting the Default Kernel
--------------------------
Grub uses the newest kernel as the default.  
To manually set the default kernel, edit the grub config.

    sudo gedit /etc/default/grub

The following will choose the 2nd item (index 1) on the grub menu,  
i.e. "Advanced options for Ubuntu", followed by the 1st item (index 0)  
on the second menu, i.e. "Ubuntu, with Linux 4.1.7-r8"

    GRUB_DEFAULT="1>0"

Uninstalling a Kernel
---------------------
If you would like to uninstall a mainline kernel, find the exact name of the kernel packages.  
Before removing, make sure to boot into a different kernel then the one being removed.

    dpkg -l | grep "linux\-[a-z]*\-" <OR> $ dpkg --list | grep kernel-image
    sudo apt-get purge KERNEL_PACKAGES_TO_REMOVE
    sudo update-grub

Uninstalling a Custom Kernel
----------------------------

    cd /lib/modules/
     sudo rm -rf KERNEL_PACKAGE_TO_REMOVE
    cd /boot/
    sudo rm sudo rm config-KERNEL_PACKAGE_TO_REMOVE
    sudo rm initrd.img-KERNEL_PACKAGE_TO_REMOVE
    sudo rm System.map-KERNEL_PACKAGE_TO_REMOVE
    sudo rm vmlinuz-KERNEL_PACKAGE_TO_REMOVE

Testing Kernel Latency
----------------------
[Cyclictest] can be used to test latency. 

    sudo apt-get install libnuma-dev
    git clone git://git.kernel.org/pub/scm/linux/kernel/git/clrkwllms/rt-tests.git 
    cd rt-tests
    make all
    sudo ./cyclictest -t1 -p 80 -n -i 10000 -l 10000








[UbuntuStudio/RealTimeKernel]:https://help.ubuntu.com/community/UbuntuStudio/RealTimeKernel
[mainstream kernel]:ftp://ftp.kernel.org/pub/linux/kernel/v4.x/
[RT kernel version]:https://www.kernel.org/pub/linux/kernel/projects/rt/  
[rt kernel wiki article]:https://rt.wiki.kernel.org/index.php/RT_PREEMPT_HOWTO  
[long term support]:https://en.wikipedia.org/wiki/Linux_kernel#4.x.y_releases
[kernelnewbies article]:http://kernelnewbies.org/KernelBuild  
[manually set the default kernel]:http://askubuntu.com/questions/216398/set-older-kernel-as-default-grub-entry
[Cyclictest]:https://rt.wiki.kernel.org/index.php/Cyclictest 
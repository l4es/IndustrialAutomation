**SimplECAT Library**
======================
Authors : Brian Soe, Xiyang Yeh, Arjun Balasingam, Arya Banait  
Contact : bsoe at stanford.edu

Developed in the Stanford Robotics Lab, Stanford University.  

About
-----
SimplECAT is a lightweight and simple C++ Library for EtherCat control,  
built upon the [EtherLab EtherCat Master], designed as a c++ object wrapper.  

Each device is written as derived class, that can be easily customized.  
New devices can be added by creating a new device header file.  
Please submit an email or pull request if you would like it added to the library.

Current supported devices include:

* Elmo Gold Whistle
* Beckhoff EK1100
* Beckhoff EL1124
* Beckhoff EL3104
* Beckhoff EL4314
* Beckhoff EL5002
* Beckhoff EL9505

Readme instructions are provided for installing on Ubuntu 14.04:  

* real-time linux (recommended)
* EtherCAT master (required)
* EtherLab (not required)


NOTE: SimpleCAT does not require EtherLab, Matlab, or Simulink.    


EtherCAT
-------------
EtherCAT is an Ethernet-based fieldbus system.  
It is suitable for both hard and soft real-time systems.

Example Code
-------------------

Example Code is provided for the following scenarios:

* Beckoff analog read write
* Elmo Gold Whistle torque control
* Interprocess shared memory
* Interprocess tcp server


```c++
simplecat::Master master;
simplecat::Beckhoff_EK1100 bh_ek1100;
simplecat::Beckhoff_EL4134 bh_el4134;

// set the values between ~2.5V and ~10V
bh_el4134.write_data_[0] =  8000;
bh_el4134.write_data_[1] = 16000;
bh_el4134.write_data_[2] = 24000;
bh_el4134.write_data_[3] = 32000;

master.addSlave(0,0,&bh_ek1100);
master.addSlave(0,1,&bh_el4134);

master.setThreadHighPriority();
master.activate();

while(1){
    // placeholder - do some loop timing
    // increment all values by 2.5 V
    for (int i=0; i<4; ++i){
        bh_el4134.write_data_[i] +=  8000;   
        if (bh_el4134.write_data_[i]>32000){
            bh_el4134.write_data_[i] = 0;
        }
    }
    master.update();
}
```

```c++
simplecat::Master master;
simplecat::Elmo_GoldWhistle elmo_gw;
elmo_master.addSlave(0,0,&elmo_gw);

elmo_gw.max_torque_ = 1500;

master.setThreadHighPriority();
master.activate();

while(1){
    // placeholder - do some loop timing

    master.update();
    
    if (elmo_gw.initialized()){
        // write data
        elmo_gw.target_torque_ += 1;
        if (elmo_gw.target_torque_>(elmo_gw.max_torque_/2)){
            elmo_gw.target_torque_ = 0;
        }
        // read data
        std::cout << "actual  position: " << elmo_gw.position_ << '\n';
        std::cout << "maximum torque: " << elmo_gw.max_torque_ << '\n';
        std::cout << "desired torque: " << elmo_gw.target_torque_ << '\n';
        std::cout << "actual  torque: " << elmo_gw.torque_ << '\n';
        for (int i=0; i<6; ++i){
            std::cout << "digital inputs " << i << ": "  elmo_gw.digital_inputs_[i] << '\n';
        }
    }
}
```

Install
-------
1. Install a real-time system (optional)  
2. Install EtherCAT master for that kernel  
3. Download SimplECAT
4. Install boost for the shared memory example (optional)
5. Build the library and examples (except tcp_server example)

```sh
git clone https://bitbucket.org/bsoe/simplecat.git  
apt-get install boost-all-dev
sh build_all.sh
```


Additional Setup Tutorials
----------------------------------
Installing a soft or hard real-time system on Ubuntu 14.04 LTS:  
[SETUP_RT_LINUX.md]  

Installing EtherCAT Master on Ubuntu 14.04 LTS:  
[SETUP_ETHERCAT_MASTER.md]

Installing EtherLab (not required) on Ubuntu 14.04 LTS:  
[SETUP_ETHERLAB.md]

Understanding PDO's:  
[ABOUT_PDOS.md]

Dependencies
------------
[EtherLab EtherCat Master]



[EtherLab EtherCat Master]:http://www.etherlab.org/en/ethercat/
[SETUP_RT_LINUX.md]:https://bitbucket.org/bsoe/simplecat/src/master/SETUP_RT_LINUX.md
[SETUP_ETHERLAB.md]:https://bitbucket.org/bsoe/simplecat/src/master/SETUP_ETHERLAB.md
[SETUP_ETHERCAT_MASTER.md]:https://bitbucket.org/bsoe/simplecat/src/master/SETUP_ETHERCAT_MASTER.md
[ABOUT_PDOS.md]:https://bitbucket.org/bsoe/simplecat/src/master/ABOUT_PDOS.md

[EtherLab]:http://www.etherlab.org/
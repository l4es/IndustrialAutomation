Data Logging
============

This example uses a tcp server to handle data exchange  
between control loop and data logging loop.  
This seperation prevents file writes from slowing down the real time control loop.  
There is no timing synchronization between the two loops.  
Data logging samples the values from shared memory and writes them to a file.  

Dependencies
============

This example uses [Redis], a popular "in-memory data structure store".  
To install on ubuntu:

```sh
sudo apt-get install redis-server redis-tools libhiredis-dev libev-dev
```

This example uses Redox, a c++ wrapper for hiredis.  
Note that Redox requires g++ 4.8 or above.

```sh
cd simplecat/examples/tcp_server/
git clone https://github.com/hmartiro/redox.git
cd redox
sh make.sh
```

The examples can then be built with

```sh
cd simplecat/examples/tcp_server
bash build.sh
```

To launch, run the following three process in seperate terminals:

```sh
./redis-server
./tcp_server_control_loop
./tcp_server_data_logger
```


[Redis]:http://redis.io/

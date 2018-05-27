Data Logging
============

This example uses shared memory to handle data exchange  
between control loop and data logging loop.  
This seperation prevents file writes from slowing down  
the real time control loop.  
There is no timing synchronization between the two loops.  
Data logging samples the values from shared memory  
and writes them to a file.  

To run, first launch "shared_memory_control_loop",  
then launch "shared_memory_data_logger".  

Notes on Mutex
==============

1. Reads and writes to shared memory are not guaranteed atomic
2. Reads and writes can be made atomic, but I don't know how in shared memory
3. There is optional mutex protection of shared memory.
4. Locks are blocking, and defeat the point of real time
5. In control loop, it is better try to lock, instead of lock
6. Trying to lock can result in data not being updated
7. If occasional invalid data is acceptable, then don't bother with locks

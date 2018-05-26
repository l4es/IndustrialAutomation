#ifndef SHAREDMEMORYSTRUCT_H_
#define SHAREDMEMORYSTRUCT_H_

// for mutex
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

struct SharedMemoryStruct
{
    int16_t analog_out_value[4] = {0};
    int16_t analog_in_value[4]  = {0};
    
    //Mutex to protect access to the queue
    boost::interprocess::interprocess_mutex mutex;
};

#endif

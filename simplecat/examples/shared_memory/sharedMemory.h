#ifndef SIMPLECAT_SHAREDMEMORY_H_
#define SIMPLECAT_SHAREDMEMORY_H_

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <string>

namespace simplecat {

#define SIMPLECAT_MUTEX boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>

template <class T>
class SharedMemory
{
public:
    SharedMemory(){}

    ~SharedMemory()
    {
        boost::interprocess::shared_memory_object::remove(name_.c_str());
    }

    /** creates the shared memory struct */
    T* createSharedMemory(std::string name)
    {
        name_ = name;

        //Remove shared memory
        boost::interprocess::shared_memory_object::remove(name_.c_str());

        //Create a shared memory object.
        shm_ = boost::interprocess::shared_memory_object
            (boost::interprocess::create_only   //only create
            ,name_.c_str()                      //name
            ,boost::interprocess::read_write    //read-write mode
            );

        //Set size
        shm_.truncate(sizeof(T));

        //Map the whole shared memory in this process
        region_ = boost::interprocess::mapped_region
            (shm_                               //What to map
            ,boost::interprocess::read_write    //Map it as read-write
            );

        //Get the address of the mapped region
        addr_ = region_.get_address();

        //Construct the shared structure in memory
        data_ = new (addr_) T;
        return data_;
    }

    /** open the shared memory struct */
    T* openSharedMemory(std::string name)
    {
        name_ = name;

        //Open the shared memory object.
        shm_ = boost::interprocess::shared_memory_object
            (boost::interprocess::open_only     //only create
            ,name_.c_str()                      //name
            ,boost::interprocess::read_write    //read-write mode
            );

        //Map the whole shared memory in this process
        region_ = boost::interprocess::mapped_region
            (shm_                               //What to map
            ,boost::interprocess::read_write    //Map it as read-write
            );

        //Get the address of the mapped region
        addr_ = region_.get_address();

        //Construct the shared structure in memory
        data_ = static_cast<T*>(addr_);
        return data_;
    }

private:
    std::string name_;
    boost::interprocess::shared_memory_object shm_;
    boost::interprocess::mapped_region region_;
    void * addr_ = NULL;
    T* data_ = NULL;
};

}

#endif // SHAREDMEMORY_H

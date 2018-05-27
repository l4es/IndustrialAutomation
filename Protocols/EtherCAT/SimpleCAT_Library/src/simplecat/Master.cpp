/* This file is part of SimpleECAT.
 *
 * SimpleECAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimplECAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with SimpleECAT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Master.h"

#include <simplecat/Slave.h>

#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>
#include <sstream> 

namespace simplecat {


Master::DomainInfo::DomainInfo(ec_master_t* master)
{
    domain = ecrt_master_create_domain(master);
    if (domain==NULL){
        printWarning("Failed to create domain");
        return;
    }

    const ec_pdo_entry_reg_t empty = {0};
    domain_regs.push_back(empty);
}


Master::DomainInfo::~DomainInfo()
{
   for (Entry& entry : entries){
       delete [] entry.offset;
       delete [] entry.bit_position;
   }
}


Master::Master(const int master)
{
    master_ = ecrt_request_master(master);
    if (master_==NULL) {
        printWarning("Failed to obtain master.");
        return;
    }
}

Master::~Master()
{
    for (SlaveInfo& slave : slave_info_){
        //
    }
    for (auto& domain : domain_info_){
        delete domain.second;
    }
}

void Master::addSlave(uint16_t alias, uint16_t position, Slave* slave)
{
    // configure slave in master
    SlaveInfo slave_info;
    slave_info.slave = slave;   
    slave_info.config = ecrt_master_slave_config(master_, alias, position,
                                                 slave->vendor_id_,
                                                 slave->product_id_);
    if (slave_info.config==NULL){
        printWarning("Add slave. Failed to get slave configuration.");
        return;
    }
    slave_info_.push_back(slave_info);

    // check if slave has pdos
    size_t num_syncs = slave->syncSize();
    const ec_sync_info_t* syncs = slave->syncs();
    if (num_syncs>0)
    {
        // configure pdos in slave
        int pdos_status = ecrt_slave_config_pdos(slave_info.config, num_syncs, syncs);
        if (pdos_status){
            printWarning("Add slave. Failed to configure PDOs");
            return;
        }
    } else {      
        printWarning("Add slave. Sync size is zero for " + static_cast<std::ostringstream*>( &(std::ostringstream() << alias) )->str() + ":" + static_cast<std::ostringstream*>( &(std::ostringstream() << position) )->str());
    }

    // check if slave registered any pdos for the domain
    Slave::DomainMap domain_map;
    slave->domains(domain_map);
    for (auto& iter : domain_map){

        // get the domain info, create if necessary
        unsigned int domain_index = iter.first;
        DomainInfo* domain_info = domain_info_[domain_index];
        if (domain_info==NULL){
            domain_info = new DomainInfo(master_);
            domain_info_[domain_index] = domain_info;
        }

        registerPDOInDomain(alias, position,
                            iter.second, domain_info,
                            slave);
    }
}


void Master::registerPDOInDomain(uint16_t alias, uint16_t position,
                                 std::vector<unsigned int>& channel_indices,
                                 DomainInfo* domain_info,
                                 Slave* slave)
{
    // expand the size of the domain
    unsigned int num_pdo_regs = channel_indices.size();
    size_t start_index = domain_info->domain_regs.size()-1; //empty element at end
    domain_info->domain_regs.resize(domain_info->domain_regs.size()+num_pdo_regs);

    // create a new entry in the domain
    DomainInfo::Entry domain_entry;
    domain_entry.slave        = slave;
    domain_entry.num_pdos     = num_pdo_regs;
    domain_entry.offset       = new unsigned int[num_pdo_regs];
    domain_entry.bit_position = new unsigned int[num_pdo_regs];
    domain_info->entries.push_back(domain_entry);

    Slave::DomainMap domain_map;
    slave->domains(domain_map);

    // add to array of pdos registrations
    const ec_pdo_entry_info_t* pdo_regs = slave->channels();
    for (size_t i=0; i<num_pdo_regs; ++i)
    {  
        // create pdo entry in the domain
        ec_pdo_entry_reg_t& pdo_reg = domain_info->domain_regs[start_index+i];
        pdo_reg.alias       = alias;
        pdo_reg.position    = position;
        pdo_reg.vendor_id   = slave->vendor_id_;
        pdo_reg.product_code= slave->product_id_;
        pdo_reg.index       = pdo_regs[channel_indices[i]].index;
        pdo_reg.subindex    = pdo_regs[channel_indices[i]].subindex;
        pdo_reg.offset      = &(domain_entry.offset[i]);
        pdo_reg.bit_position= &(domain_entry.bit_position[i]);


        // print the domain pdo entry
        std::cout << "{" << pdo_reg.alias <<", "<< pdo_reg.position;
        std::cout << ", 0x" << std::hex << pdo_reg.vendor_id;
        std::cout << ", 0x" << std::hex << pdo_reg.product_code;
        std::cout << ", 0x" << std::hex << pdo_reg.index;
        std::cout << ", 0x" << std::hex << (int)pdo_reg.subindex;
        std::cout << "}" << std::dec << std::endl;
    }

    // set the last element to null
    ec_pdo_entry_reg_t empty = {0};
    domain_info->domain_regs.back() = empty;
}


void Master::activate()
{
    // register domain
    for (auto& iter : domain_info_){
        DomainInfo* domain_info = iter.second;
        bool domain_status = ecrt_domain_reg_pdo_entry_list(
                                    domain_info->domain,
                                    &(domain_info->domain_regs[0]));
        if (domain_status){
            printWarning("Activate. Failed to register domain PDO entries.");
            return;
        }
    }
    // activate master
    bool activate_status = ecrt_master_activate(master_);
    if (activate_status){
        printWarning("Activate. Failed to activate master.");
        return;
    }

    // retrieve domain data
    for (auto& iter : domain_info_){
        DomainInfo* domain_info = iter.second;
        domain_info->domain_pd = ecrt_domain_data(domain_info->domain);
        if (domain_info->domain_pd==NULL){
            printWarning("Activate. Failed to retrieve domain process data.");
            return;
        }
    }
}

void Master::update(unsigned int domain)
{
    // receive process data
    ecrt_master_receive(master_);

    DomainInfo* domain_info = domain_info_[domain];

    ecrt_domain_process(domain_info->domain);

    // check process data state (optional)
    checkDomainState(domain);

    // check for master and slave state change
    if (update_counter_ % check_state_frequency_ == 0){
        checkMasterState();
        checkSlaveStates();
    }

    // read and write process data
    for (DomainInfo::Entry& entry : domain_info->entries){
        for (int i=0; i<entry.num_pdos; ++i){
            (entry.slave)->processData(i, domain_info->domain_pd + entry.offset[i]);
        }
    }

    // send process data
    ecrt_domain_queue(domain_info->domain);
    ecrt_master_send(master_);

    ++update_counter_;
}


void Master::setCtrlCHandler(SIMPLECAT_EXIT_CALLBACK user_callback)
{
    // ctrl c handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = user_callback;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}


void Master::run(SIMPLECAT_CONTRL_CALLBACK user_callback, double frequency)
{
    printf("Running loop at [%.1f] Hz\n", frequency);

    unsigned int interval = 1000000000.0/frequency;

    // start after one second
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC ,&t);
    t.tv_sec++;

    running_ = true;
    start_t_ = std::chrono::system_clock::now();    
    while(running_)
    {
        // wait until next shot
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

        // update EtherCAT bus
        this->update();

        // get actual time
        curr_t_ = std::chrono::system_clock::now();

        // user callback
        user_callback();

        // calculate next shot. carry over nanoseconds into microseconds.
        t.tv_nsec += interval;
        while (t.tv_nsec >= 1000000000){
            t.tv_nsec -= 1000000000;
            t.tv_sec++;
        }
    }
}


double Master::elapsedTime()
{   
    std::chrono::duration<double> elapsed_seconds = curr_t_ - start_t_;
    return elapsed_seconds.count()-1.0; // started after 1 second
}


unsigned long long Master::elapsedCycles()
{
    return update_counter_;
}



void Master::setThreadHighPriority()
{
    pid_t pid = getpid();
    int priority_status = setpriority(PRIO_PROCESS, pid, -19);
    if (priority_status){
        printWarning("setThreadHighPriority. Failed to set priority.");
        return;
    }
}


void Master::setThreadRealTime()
{  
    /* Declare ourself as a real time task, priority 49.
       PRREMPT_RT uses priority 50
       for kernel tasklets and interrupt handler by default */
    struct sched_param param;
    param.sched_priority = 49;
    //pthread_t this_thread = pthread_self();
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed");
        exit(-1);
    }

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        exit(-2);
    }

    /* Pre-fault our stack
       8*1024 is the maximum stack size
       which is guaranteed safe to access without faulting */
    int MAX_SAFE_STACK = 8*1024;
    unsigned char dummy[MAX_SAFE_STACK];
    memset(dummy, 0, MAX_SAFE_STACK);
}

void Master::checkDomainState(unsigned int domain)
{
    DomainInfo* domain_info = domain_info_[domain];

    ec_domain_state_t ds;
    ecrt_domain_state(domain_info->domain, &ds);

    if (ds.working_counter != domain_info->domain_state.working_counter){
        printf("Domain: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain_info->domain_state.wc_state){
        printf("Domain: State %u.\n", ds.wc_state);
    }
    domain_info->domain_state = ds;
}


void Master::checkMasterState()
{
    ec_master_state_t ms;
    ecrt_master_state(master_, &ms);

    if (ms.slaves_responding != master_state_.slaves_responding){
        printf("%u slave(s).\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state_.al_states){
        printf("Master AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state_.link_up){
        printf("Link is %s.\n", ms.link_up ? "up" : "down");
    }
    master_state_ = ms;
}


void Master::checkSlaveStates()
{
    for (SlaveInfo& slave : slave_info_)
    {
        ec_slave_config_state_t s;
        ecrt_slave_config_state(slave.config, &s);

        if (s.al_state != slave.config_state.al_state){
            //this spams the terminal at initialization.
            printf("Slave: State 0x%02X.\n", s.al_state);
        }
        if (s.online != slave.config_state.online){
            printf("Slave: %s.\n", s.online ? "online" : "offline");
        }
        if (s.operational != slave.config_state.operational){
            printf("Slave: %soperational.\n", s.operational ? "" : "Not ");
        }
        slave.config_state = s;
    }
}


void Master::printWarning(const std::string& message)
{
    std::cout << "WARNING. Master. " << message << std::endl;
}

}




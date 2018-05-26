/**
 * This file is part of SimpleECAT.
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

 *
 * \class Master
 *
 * \ingroup SimplECAT
 *
 * \brief Master for reading and writing to slave devices.
 *
 * This class allows mulitple EtherCAT slaves to be configured and run
 * using EtherLab's EtherCAT Master 1.5.2 code.
 **/


#ifndef SIMPLECAT_MASTER_H_
#define SIMPLECAT_MASTER_H_

#include <ecrt.h>

#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace simplecat {

class Slave;

class Master
{
public:
    Master(const int master = 0);
    virtual ~Master();

    /** \brief add a slave device to the master
      * alias and position can be found by running the following command
      * /opt/etherlab/bin$ sudo ./ethercat slaves
      * look for the "A B:C STATUS DEVICE" (e.g. B=alias, C=position)
      */
    void addSlave(uint16_t alias, uint16_t position, Slave* slave);

    /** call after adding all slaves, and before update */
    void activate();

    /** perform one EtherCAT cycle, passing the domain to the slaves */
    virtual void update(unsigned int domain = 0);

    /** run a control loop of update() and user_callback(), blocking.
     *  call activate and setThreadHighPriority/RealTime first. */
    typedef void (*SIMPLECAT_CONTRL_CALLBACK)(void);
    virtual void run(SIMPLECAT_CONTRL_CALLBACK user_callback, double frequency);

    /** stop the control loop. use within callback, or from a seperate thread. */
    virtual void stop() {running_ = false;}

    /** time of last ethercat update, since calling run. stops if stop called. 
     *  returns actual time. use elapsedCycles()/frequency for discrete time at last update. */
    virtual double elapsedTime();

    /** number of EtherCAT updates since calling run. */
    virtual unsigned long long elapsedCycles();

    /** add ctr-c exit callback.
      * default exits the run loop and prints timing */
    typedef void (*SIMPLECAT_EXIT_CALLBACK)(int);
    static void setCtrlCHandler(SIMPLECAT_EXIT_CALLBACK user_callback = NULL);

    /** set the thread to a priority of -19
     *  priority range is -20 (highest) to 19 (lowest) */
    static void setThreadHighPriority();

    /** set the thread to real time (FIFO)
     *  thread cannot be preempted.
     *  set priority as 49 (kernel and interrupts are 50) */
    static void setThreadRealTime();

private:

    /** true if running */
    volatile bool running_ = false;

    /** start and current time */
    std::chrono::time_point<std::chrono::system_clock> start_t_, curr_t_;

    // EtherCAT Control

    /** register a domain of the slave */
    struct DomainInfo;
    void registerPDOInDomain(uint16_t alias, uint16_t position,
                             std::vector<unsigned int>& channel_indices,
                             DomainInfo* domain_info,
                             Slave* slave);

    /** check for change in the domain state */
    void checkDomainState(unsigned int domain);

    /** check for change in the master state */
    void checkMasterState();

    /** check for change in the slave states */
    void checkSlaveStates();

    /** print warning message to terminal */
    static void printWarning(const std::string& message);

    /** EtherCAT master data */
    ec_master_t *master_ = NULL;
    ec_master_state_t master_state_ = {};

    /** data for a single domain */
    struct DomainInfo
    {
        DomainInfo(ec_master_t* master);
        ~DomainInfo();

        ec_domain_t *domain = NULL;
        ec_domain_state_t domain_state = {};
        uint8_t *domain_pd = NULL;

        /** domain pdo registration array.
         *  do not modify after active(), or may invalidate */
        std::vector<ec_pdo_entry_reg_t> domain_regs;

        /** slave's pdo entries in the domain */
        struct Entry {
            Slave* slave               = NULL;
            int num_pdos               = 0;
            unsigned int* offset       = NULL;
            unsigned int* bit_position = NULL;
        };

        std::vector<Entry> entries;
    };

    /** map from domain index to domain info */
    std::map<unsigned int, DomainInfo*> domain_info_;

    /** data needed to check slave state */
    struct SlaveInfo {
        Slave*                  slave               = NULL;
        ec_slave_config_t*      config              = NULL;
        ec_slave_config_state_t config_state        = {0};
    };

    std::vector<SlaveInfo> slave_info_;

    /** counter of control loops */
    unsigned long long update_counter_ = 0;

    /** frequency to check for master or slave state change.
     *  state checked every frequency_ control loops */
    unsigned int check_state_frequency_ = 100;

};

}

#endif

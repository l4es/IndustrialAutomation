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
 * \class Beckhoff_EL5101
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EL5101
 *
 * 1 Quadrature Incremental encoder input
 *
 * https://download.beckhoff.com/download/document/io/ethercat-terminals/el5101en.pdf
 */


#ifndef SIMPLECAT_BECKHOFF_EL5101_H_
#define SIMPLECAT_BECKHOFF_EL5101_H_

#include <simplecat/Slave.h>
#include <iostream>

namespace simplecat {


class Beckhoff_EL5101 : public Slave
{

public:
    Beckhoff_EL5101() : Slave(0x00000002, 0x13ed3052) {}
    virtual ~Beckhoff_EL5101() {}

    /** Returns true if counter values are ready to be read */
    bool initialized()
    {
        return status_byte.status_input;
    }

    /** Set the counter, takes 3 cycles */
    void setCounter(uint16_t value)
    {
        write_value = value;
        cntset_state = CNTSET_START;
    }

    /** Returns the counter value, as signed value */
    int16_t counter()
    {
        return (int16_t)read_value;
    }

    virtual void processData(size_t index, uint8_t* domain_address){
        switch (index) {
            case 0:
                // first read the value
                write_ctrl = EC_READ_U8(domain_address);

                // bit masking to get individual input values
                //control_byte.en_latch_extn  = write_ctrl & 0b001000;
                //control_byte.cnt_set        = write_ctrl & 0b000100;
                //control_byte.en_latch_extp  = write_ctrl & 0b000010;
                //control_byte.en_latc        = write_ctrl & 0b000001;

                // set the value
                switch(cntset_state)
                {
                    case CNTSET_INACTIVE : break;
                    case CNTSET_START    : 
                    {
                        write_ctrl &= ~(0b000100) ; 
                        cntset_state = CNTSET_CLEAR;
                        break;
                    }
                    case CNTSET_CLEAR    : 
                    {
                        write_ctrl |= 0b000100 ; 
                        cntset_state = CNTSET_RISING;
                        break;
                    }
                    case CNTSET_RISING   : 
                    {
                        write_ctrl &= ~(0b000100) ; 
                        cntset_state = CNTSET_INACTIVE;
                        break;
                    }
                }
                EC_WRITE_U8(domain_address, write_ctrl);
                break;
            case 1:
                EC_WRITE_U16(domain_address, write_value);
                break;
            case 2:
                // counter read in normal operating mode
                read_status = EC_READ_U8(domain_address);

                // bit masking to get individual input values
                status_byte.status_input = read_status & 0b100000;
                status_byte.overflow     = read_status & 0b010000;
                status_byte.undeflow     = read_status & 0b001000;
                status_byte.cntset_acc   = read_status & 0b000100;
                status_byte.lat_ext_val  = read_status & 0b000010;
                status_byte.latc_val     = read_status & 0b000001;
                break;
            case 3:        
                // counter read in normal operating mode
                read_value = EC_READ_U16(domain_address);
                break;
            case 4:
                // latch read in normal operating mode
                read_latch = EC_READ_U16(domain_address);
                break;
        }
    }

    virtual const ec_sync_info_t* syncs() { return &syncs_[0]; }

    virtual size_t syncSize() {
        return sizeof(syncs_)/sizeof(ec_sync_info_t);
    }

    virtual const ec_pdo_entry_info_t* channels() {
        return channels_;
    }

    virtual void domains(DomainMap& domains) const {
        domains = domains_;
    }

    // write: encoder ctrl
    uint8_t write_ctrl = 0;
    // write: encoder value
    uint16_t write_value = 0;
    // read: encoder status
    uint8_t read_status = 0;
    // read: encoder counts
    uint16_t read_value = 0;
    // read: encoder latch
    uint16_t read_latch = 0;

    // status byte
    struct StatusByte {
        bool status_input = false;
        bool overflow     = false;
        bool undeflow     = false;
        bool cntset_acc   = false;
        bool lat_ext_val  = false;
        bool latc_val     = false;
    } status_byte;

    // control byte
    struct ControlByte {
        bool en_latch_extn = false;
        bool cnt_set = false;
        bool en_latch_extp = false;
        bool en_latc = false;
    }; //control_byte;

    // set count state machine
    enum CntSetState {
        CNTSET_INACTIVE,
        CNTSET_START,
        CNTSET_CLEAR,
        CNTSET_RISING   
    } cntset_state;

private:
    ec_pdo_entry_info_t channels_[5] = {
        // write
        {0x7000, 0x01, 8}, /* Ctrl */
        {0x7000, 0x02, 16}, /* Value */
        // read
        {0x6000, 0x01, 8}, /* Status */
        {0x6000, 0x02, 16}, /* Value */
        {0x6000, 0x03, 16}, /* Latch */
    };

    ec_pdo_info_t pdos_[2] = {
        {0x1600, 2, channels_ + 0}, /* RxPDO-Map Outputs */
        {0x1a00, 3, channels_ + 2}, /* TxPDO-Map Inputs */
    };

    ec_sync_info_t syncs_[5] = {
        {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
        {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
        {2, EC_DIR_OUTPUT, 1, pdos_ + 0, EC_WD_DISABLE},
        {3, EC_DIR_INPUT, 1, pdos_ + 1, EC_WD_DISABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {0,1,2,3,4}}
    };
};


}

#endif

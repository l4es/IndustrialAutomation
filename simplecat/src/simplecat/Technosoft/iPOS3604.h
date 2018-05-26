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
 * \class Technosoft_iPOS3604
 *
 * \ingroup SimplECAT
 *
 * \brief Technosoft iPOS3604
 *
 * Brushed or brushless DC servo drive.
 */


#ifndef SIMPLECAT_TECHNOSOFT_IPOS3604_H_
#define SIMPLECAT_TECHNOSOFT_IPOS3604_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Technosoft_iPOS3604 : public Slave
{

public:
    Technosoft_iPOS3604() : Slave(0x000001a3, 0x01ab46e5) 
    {
        std::cout << "ERROR! THIS CODE IS NOT FUNCTIONAL YET. SORRY!" << std::endl;
    }

    virtual ~Technosoft_iPOS3604() {}

    bool initialized() const {return initialized_;}

    virtual void processData(size_t index, uint8_t* domain_address)
    {
        // DATA READ WRITE
        switch(index)
        {

        case 0:
            control_word_ = EC_READ_U16(domain_address);
            control_word_ = transition(state_, control_word_);
            EC_WRITE_U16(domain_address, control_word_);
            break;
        case 1:
            EC_WRITE_S8(domain_address, mode_of_operation_);
            break;
        case 2:
            //EC_WRITE_S32(domain_address, target_position_);
            EC_WRITE_S16(domain_address, target_torque_);
            break;
        case 3:
            mode_of_operation_display_ = EC_READ_S8(domain_address);
            break;
        case 4:
            status_word_ = EC_READ_U16(domain_address);
            state_ = deviceState(status_word_);
            break;
        case 5:
            position_ = EC_READ_S32(domain_address);
            break;
        default:
            std::cout << "WARNING. Technosoft Gold Whistle pdo index out of range." << std::endl;
        }

        // CHECK FOR STATE CHANGE
        if (index==5) //if last entry  in domain
        {
            if (status_word_ != last_status_word_){
                state_ = deviceState(status_word_);
                //std::cout << "STATUS WORD: " << status_word_ << std::endl;
                std::cout << "STATE: " << device_state_str_[state_] << std::endl;
            }
            if ((state_ == STATE_OPERATION_ENABLED)&&
                (last_state_ == STATE_OPERATION_ENABLED)){
                initialized_ = true;
            } else {
                initialized_ = false;
            }
            last_status_word_ = status_word_;
            last_state_ = state_;
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

    uint16_t control_word_ = 0;              // write
    int8_t   mode_of_operation_ = 0;         // write
    //int32_t target_position_ = 0;          // write
    int16_t  target_torque_ = 0;             // write
    int8_t   mode_of_operation_display_ = 0; // read
    uint16_t status_word_ = 0;               // read
    int32_t  position_ = 0;                  // read

    enum ModeOfOperation
    {
        MODE_NO_MODE = 0,
        MODE_PROFILED_POSITION = 1,
        MODE_PROFILED_VELOCITY = 3,
        MODE_PROFILED_TORQUE = 4,
        MODE_HOMING = 6,
        MODE_INTERPOLATED_POSITION = 7,
        MODE_CYCLIC_SYNC_POSITION = 8,
        MODE_CYCLIC_SYNC_VELEOCITY = 9,
        MODE_CYCLIC_SYNC_TORQUE = 10
    };

private:

    ec_pdo_entry_info_t channels_[6] = {
        {0x6040, 0x00, 16}, /* Control word */
        {0x6060, 0x00, 8},  /* Mode of operation */

        //{0x607a, 0x12, 32}, /* Target position */
        {0x6071, 0x00, 16}, /* Target torque */

        {0x6061, 0x00, 8},  /* Mode of operation display */

        {0x6041, 0x00, 16}, /* Statusword */
        {0x6064, 0x00, 32}, /* Position actual value */
    };

    ec_pdo_info_t pdos_[4] = {
        {0x1600, 2, channels_ + 0}, /* RPDO6 Mapping */
        {0x1602, 1, channels_ + 2}, /* RPDO3 Mapping */
        {0x1a01, 1, channels_ + 3}, /* TPD1E Mapping */
        {0x1a02, 2, channels_ + 4}, /* TPD1C Mapping */
    };

    ec_sync_info_t syncs_[5] = {
        {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
        {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
        {2, EC_DIR_OUTPUT, 2, pdos_ + 0, EC_WD_DISABLE},
        {3, EC_DIR_INPUT, 2, pdos_ + 2, EC_WD_DISABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {0,1,2,3,4,5} }
    };


//========================================================
// Technosoft SPECIFIC
//========================================================

    enum DeviceState
    {
        STATE_UNDEFINED = 0,
        STATE_START = 1,
        STATE_NOT_READY_TO_SWITCH_ON,
        STATE_SWITCH_ON_DISABLED,
        STATE_READY_TO_SWITCH_ON,
        STATE_SWITCH_ON,
        STATE_OPERATION_ENABLED,
        STATE_QUICK_STOP_ACTIVE,
        STATE_FAULT_REACTION_ACTIVE,
        STATE_FAULT
    };

    std::map<DeviceState,std::string> device_state_str_ = {
         {STATE_START,                  "Start"},
         {STATE_NOT_READY_TO_SWITCH_ON, "Not Ready to Switch On"},
         {STATE_SWITCH_ON_DISABLED,     "Switch on Disabled"},
         {STATE_READY_TO_SWITCH_ON,     "Ready to Switch On"},
         {STATE_SWITCH_ON,              "Switch On"},
         {STATE_OPERATION_ENABLED,      "Operation Enabled"},
         {STATE_QUICK_STOP_ACTIVE,      "Quick Stop Active"},
         {STATE_FAULT_REACTION_ACTIVE,  "Fault Reaction Active"},
         {STATE_FAULT,                  "Fault"}
    };

    //0010 0100 0000

    /** returns device state based upon the status_word */
    DeviceState deviceState(uint16_t status_word)
    {
        if      ((status_word & 0b01001111) == 0b00000000){
            return STATE_NOT_READY_TO_SWITCH_ON;
        }
        else if ((status_word & 0b01001111) == 0b01000000){
            return STATE_SWITCH_ON_DISABLED;
        }
        else if ((status_word & 0b01101111) == 0b00100001){
            return STATE_READY_TO_SWITCH_ON;
        }
        else if ((status_word & 0b01101111) == 0b00100011){
            return STATE_SWITCH_ON;
        }
        else if ((status_word & 0b01101111) == 0b00100111){
            return STATE_OPERATION_ENABLED;
        }
        else if ((status_word & 0b01101111) == 0b00000111){
            return STATE_QUICK_STOP_ACTIVE;
        }
        else if ((status_word & 0b01001111) == 0b00001111){
            return STATE_FAULT_REACTION_ACTIVE;
        }
        else if ((status_word & 0b01001111) == 0b00001000){
            return STATE_FAULT;
        }
        return STATE_UNDEFINED;
    }

    /** returns the control word that will take device from state to next desired state */
    uint16_t transition(DeviceState state, uint16_t control_word)
    {
        switch(state)
        {
        case STATE_START:                   // -> STATE_NOT_READY_TO_SWITCH_ON (automatic)
            return control_word;
        case STATE_NOT_READY_TO_SWITCH_ON:  // -> STATE_SWITCH_ON_DISABLED (automatic)
            return control_word;
        case STATE_SWITCH_ON_DISABLED:      // -> STATE_READY_TO_SWITCH_ON
            return ((control_word & 0b01111110) | 0b00000110);
        case STATE_READY_TO_SWITCH_ON:      // -> STATE_SWITCH_ON
            return ((control_word & 0b01110111) | 0b00000111);
        case STATE_SWITCH_ON:               // -> STATE_OPERATION_ENABLED
            return ((control_word & 0b01111111) | 0b00001111);
        case STATE_OPERATION_ENABLED:       // -> GOOD
            return control_word;
        case STATE_QUICK_STOP_ACTIVE:       // -> STATE_OPERATION_ENABLED
            return ((control_word & 0b01111111) | 0b00001111);
        case STATE_FAULT_REACTION_ACTIVE:   // -> STATE_FAULT (automatic)
            return control_word;
        case STATE_FAULT:                   // -> STATE_SWITCH_ON_DISABLED
            return ((control_word & 0b11111111) | 0b10000000);
        default:
            break;
        }
        return control_word;
    }

    int last_status_word_ = -1;
    DeviceState last_state_ = STATE_START;
    DeviceState state_ = STATE_START;

    bool initialized_ = false;

};


}

#endif

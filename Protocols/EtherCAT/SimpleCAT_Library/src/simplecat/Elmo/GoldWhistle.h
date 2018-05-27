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
 * \class Elmo_GoldWhistle
 *
 * \ingroup SimplECAT
 *
 * \brief Elmo Gold Whistle
 *
 * Brushed or brushless DC servo drive.
 */


#ifndef SIMPLECAT_ELMO_GOLDWHISTLE_H_
#define SIMPLECAT_ELMO_GOLDWHISTLE_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Elmo_GoldWhistle : public Slave
{

public:
    Elmo_GoldWhistle() : Slave(0x0000009a, 0x00030924) {}
    virtual ~Elmo_GoldWhistle() {}

    /** Returns true if Elmo has reached "operation enabled" state.
     *  The transition through the state machine is handled automatically. */
    bool initialized() const {return initialized_;}

    /** Write the torque in Nm. 
     *  User must first set max_torque_Nm_.
     *  Provided as a convenience to calculate target_torque_
     *  NOTE: function assumes [max_torque_ = 1000]  */
    void writeTorqueNm(const double torque)
    {
        target_torque_ = 1000.0*torque/max_torque_;
    }

    /** Maximum torque in Nm, 
     *  corresponding to the max current set in Elmo Motion Studio */
    double max_torque_Nm_;



    virtual void processData(size_t index, uint8_t* domain_address)
    {
        // DATA READ WRITE
        switch(index)
        {
        /*
        case 0:
            // bit masking to get individual input values
            if(digital_outputs_[0]){ digital_output_ |= (1 << 24); } else { digital_output_ &= ~(1 << 24); }
            if(digital_outputs_[1]){ digital_output_ |= (1 << 25); } else { digital_output_ &= ~(1 << 25); }
            if(digital_outputs_[2]){ digital_output_ |= (1 << 26); } else { digital_output_ &= ~(1 << 26); }
            if(digital_outputs_[3]){ digital_output_ |= (1 << 27); } else { digital_output_ &= ~(1 << 27); }
            if(digital_outputs_[4]){ digital_output_ |= (1 << 28); } else { digital_output_ &= ~(1 << 28); }
            if(digital_outputs_[5]){ digital_output_ |= (1 << 29); } else { digital_output_ &= ~(1 << 29); }
            EC_WRITE_U32(domain_address, digital_output_);
            break;
        */
        case 0:
            EC_WRITE_S32(domain_address, target_position_);
            break;
        case 1:
            EC_WRITE_S32(domain_address, target_velocity_);
            break;
        case 2:
            EC_WRITE_S16(domain_address, target_torque_);
            break;
        case 3:
            EC_WRITE_U16(domain_address, max_torque_);
            break;
        case 4:
            control_word_ = EC_READ_U16(domain_address);

            //initialization sequence
            control_word_ = transition(state_, control_word_);
            EC_WRITE_U16(domain_address, control_word_);
            break;
        case 5:
            EC_WRITE_S8(domain_address, mode_of_operation_);
            break;
        case 6:
            absolute_position_ = EC_READ_S32(domain_address);
            break;
        case 7:
            analog_input_ = EC_READ_S16(domain_address);
            break;
        case 8:
            position_ = EC_READ_S32(domain_address);
            break;
        case 9:
            torque_ = EC_READ_S16(domain_address);
            break;
        case 10:
            status_word_ = EC_READ_U16(domain_address);
            state_ = deviceState(status_word_);
            break;
        case 11:
            mode_of_operation_display_ = EC_READ_S8(domain_address);
            break;
        /*
        case 12:
            digital_input_ = EC_READ_U32(domain_address);
            // bit masking to get individual input values
            digital_inputs_[0] = ((digital_input_ >> 24) & 1); // bit 24
            digital_inputs_[1] = ((digital_input_ >> 25) & 1); // bit 25
            digital_inputs_[2] = ((digital_input_ >> 26) & 1); // bit 26
            digital_inputs_[3] = ((digital_input_ >> 27) & 1); // bit 27
            digital_inputs_[4] = ((digital_input_ >> 28) & 1); // bit 28
            digital_inputs_[5] = ((digital_input_ >> 29) & 1); // bit 29
            break;
        */
        default:
            std::cout << "WARNING. Elmo Gold Whistle pdo index out of range." << std::endl;
        }

        // CHECK FOR STATE CHANGE
        if (index==10) //if last entry  in domain
        {
            if (status_word_ != last_status_word_){
                state_ = deviceState(status_word_);
                // status word change does not necessarily mean state change
                // http://ftp.beckhoff.com/download/document/motion/ax2x00_can_manual_en.pdf
                // std::bitset<16> temp(status_word_);
                // std::cout << "STATUS WORD: " << temp << std::endl;
                if (state_ != last_state_){
                    std::cout << "STATE: " << device_state_str_[state_] << std::endl;
                }
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



    int32_t  target_position_           = 0; // write
    int32_t  target_velocity_           = 0; // write
    int16_t  target_torque_             = 0; // write (max torque (max current) = 1000)
    uint16_t max_torque_                = 1000; // write (max current = 1000, as set in Elmo Motion Studio)
    uint16_t control_word_              = 0; // write
    int8_t   mode_of_operation_         = 0; // write (use enum ModeOfOperation for convenience)
    
    int32_t  position_                  = 0; // read
    int16_t  torque_                    = 0; // read
    uint16_t status_word_               = 0; // read
    int8_t   mode_of_operation_display_ = 0; // read

    int32_t  absolute_position_         = 0; // read

    int16_t  analog_input_              = 0; // read

    bool     digital_outputs_[6]        = {false}; // write
    bool     digital_inputs_[6]         = {false}; // read  


    enum ModeOfOperation
    {
        MODE_NO_MODE                = 0,
        MODE_PROFILED_POSITION      = 1,
        MODE_PROFILED_VELOCITY      = 3,
        MODE_PROFILED_TORQUE        = 4,
        MODE_HOMING                 = 6,
        MODE_INTERPOLATED_POSITION  = 7,
        MODE_CYCLIC_SYNC_POSITION   = 8,
        MODE_CYCLIC_SYNC_VELEOCITY  = 9,
        MODE_CYCLIC_SYNC_TORQUE     = 10
    };

private:

    uint32_t digital_output_            = 0; // write
    uint32_t digital_input_             = 0; // read (must be enabled in Elmo Motion Studio) 

    ec_pdo_entry_info_t channels_[12] = {
        //{0x60fe, 1, 32},  /* Digital outputs */

        {0x607a, 0, 32}, /* Target position */
        {0x60ff, 0, 32}, /* Target velocity */
        {0x6071, 0, 16}, /* Target torque */
        {0x6072, 0, 16}, /* Max torque */
        {0x6040, 0, 16}, /* Control word */
        {0x6060, 0, 8},  /* Mode of operation */        

        {0x20a0, 0, 32}, /* Absolute position value */

        {0x2205, 1, 16}, /* Analog inputs */

        {0x6064, 0, 32}, /* Position actual value */
        {0x6077, 0, 16}, /* Torque actual value */
        {0x6041, 0, 16}, /* Statusword */
        {0x6061, 0, 8},  /* Mode of operation display */

        //{0x60fd, 0, 32}, /* Digital inputs */
    };

    ec_pdo_info_t pdos_[4] = {
        //{0x161d, 1, channels_ + 0},  /* RPD1D Mapping */
        {0x1605, 6, channels_},  /* RPDO6 Mapping */
        {0x1a1e, 1, channels_ + 6}, /* TPD1E Mapping */
        {0x1a1d, 1, channels_ + 7}, /* TPD1D Mapping */
        {0x1a02, 4, channels_ + 8},  /* TPDO3 Mapping */
        //{0x1a1c, 1, channels_ + 13}, /* TPD1C Mapping */
    };

    ec_sync_info_t syncs_[5] = {
        {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
        {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
        {2, EC_DIR_OUTPUT, 1, pdos_, EC_WD_ENABLE},
        {3, EC_DIR_INPUT, 3, pdos_ + 1, EC_WD_DISABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {0,1,2,3,4,5,6,7,8,9,10,11}}
    };


//========================================================
// ELMO SPECIFIC
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

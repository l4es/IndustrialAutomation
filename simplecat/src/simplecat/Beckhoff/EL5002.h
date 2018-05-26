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
 * \class Beckhoff_EL5002
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EL5002
 *
 * 2 SSI encoder inputs
 */


#ifndef SIMPLECAT_BECKHOFF_EL5002_H_
#define SIMPLECAT_BECKHOFF_EL5002_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Beckhoff_EL5002 : public Slave
{

public:
    Beckhoff_EL5002() : Slave(0x00000002, 0x138a3052) {}
    virtual ~Beckhoff_EL5002() {}

    virtual void processData(size_t index, uint8_t* domain_address){
       read_data_[index] = EC_READ_U32(domain_address);
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

    // encoder counts
    uint32_t read_data_[2] = {0};

private:
    ec_pdo_entry_info_t channels_[16] = {
        {0x6000, 0x01, 1}, /* Data error */
        {0x6000, 0x02, 1}, /* Frame error */
        {0x6000, 0x03, 1}, /* Power failure */
        {0x0000, 0x00, 10}, /* Gap */
        {0x1c32, 0x20, 1},
        {0x1800, 0x07, 1},
        {0x1800, 0x09, 1},
        {0x6000, 0x11, 32}, /* Counter value */
        {0x6010, 0x01, 1}, /* Data error */
        {0x6010, 0x02, 1}, /* Frame error */
        {0x6010, 0x03, 1}, /* Power failure */
        {0x0000, 0x00, 10}, /* Gap */
        {0x1c32, 0x20, 1},
        {0x1801, 0x07, 1},
        {0x1801, 0x09, 1},
        {0x6010, 0x11, 32}, /* Counter value */
    };

    ec_pdo_info_t pdos_[2] = {
        {0x1a00, 8, channels_ + 0}, /* SSI TxPDO-Map Inputs */
        {0x1a01, 8, channels_ + 8}, /* SSI TxPDO-Map Inputs */
    };

    ec_sync_info_t syncs_[2] = {
        {3, EC_DIR_INPUT, 2, pdos_ + 0, EC_WD_ENABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {7,15} }
    };
};


}

#endif

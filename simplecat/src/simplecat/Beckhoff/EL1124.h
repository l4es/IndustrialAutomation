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
 * \class Beckhoff_EL1124
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EL1124
 *
 * Four digital inputs 5V TTL
 */


#ifndef SIMPLECAT_BECKHOFF_EL1124_H_
#define SIMPLECAT_BECKHOFF_EL1124_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Beckhoff_EL1124 : public Slave
{

public:
    Beckhoff_EL1124() : Slave(0x00000002, 0x04643052) {}
    virtual ~Beckhoff_EL1124() {}

    virtual void processData(size_t index, uint8_t* domain_address){
        read_data_ = EC_READ_U8(domain_address);
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

    // digital input values
    uint8_t read_data_ = 0;

private:
    ec_pdo_entry_info_t channels_[4] = {
        {0x6000, 0x01, 1}, /* Input */
        {0x6010, 0x01, 1}, /* Input */
        {0x6020, 0x01, 1}, /* Input */
        {0x6030, 0x01, 1}, /* Input */
    };

    ec_pdo_info_t pdos_[4] = {
        {0x1a00, 1, channels_ + 0}, /* Channel 1 */
        {0x1a01, 1, channels_ + 1}, /* Channel 2 */
        {0x1a02, 1, channels_ + 2}, /* Channel 3 */
        {0x1a03, 1, channels_ + 3}, /* Channel 4 */
    };

    ec_sync_info_t syncs_[2] = {
        {0, EC_DIR_INPUT, 4, pdos_ + 0, EC_WD_ENABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {0} }
    };
};


}

#endif

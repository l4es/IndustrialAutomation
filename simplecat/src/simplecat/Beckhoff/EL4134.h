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
 * \class Beckhoff_EL4134
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EL4134
 *
 * Four analog outputs +/- 10V
 */


#ifndef SIMPLECAT_BECKHOFF_EL4134_H_
#define SIMPLECAT_BECKHOFF_EL4134_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Beckhoff_EL4134 : public Slave
{

public:
    Beckhoff_EL4134() : Slave(0x00000002, 0x10263052) {}
    virtual ~Beckhoff_EL4134() {}

    virtual void processData(size_t index, uint8_t* domain_address){
        EC_WRITE_S16(domain_address, write_data_[index]);
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

    // analog write values
    int16_t write_data_[4] = {0};

private:
    ec_pdo_entry_info_t channels_[4] = {
        {0x7000, 0x01, 16}, /* Analog output */
        {0x7010, 0x01, 16}, /* Analog output */
        {0x7020, 0x01, 16}, /* Analog output */
        {0x7030, 0x01, 16}, /* Analog output */
    };

    ec_pdo_info_t pdos_[4] = {
        {0x1600, 1, channels_ + 0}, /* AO RxPDO-Map OutputsCh.1 */
        {0x1601, 1, channels_ + 1}, /* AO RxPDO-Map OutputsCh.2 */
        {0x1602, 1, channels_ + 2}, /* AO RxPDO-Map OutputsCh.3 */
        {0x1603, 1, channels_ + 3}, /* AO RxPDO-Map OutputsCh.4 */

    };

    ec_sync_info_t syncs_[2] = {
        {2, EC_DIR_OUTPUT, 4, pdos_ + 0, EC_WD_ENABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {0,1,2,3} }
    };
};


}

#endif

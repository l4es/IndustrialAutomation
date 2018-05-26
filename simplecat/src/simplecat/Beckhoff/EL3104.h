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
 * \class Beckhoff_EL3104
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EL3104
 *
 * Four analog inputs +/- 10V
 */


#ifndef SIMPLECAT_BECKHOFF_EL3104_H_
#define SIMPLECAT_BECKHOFF_EL3104_H_

#include <simplecat/Slave.h>

namespace simplecat {


class Beckhoff_EL3104 : public Slave
{

public:
    Beckhoff_EL3104() : Slave(0x00000002, 0x0c203052) {}
    virtual ~Beckhoff_EL3104() {}

    virtual void processData(size_t index, uint8_t* domain_address){
        read_data_[index] = EC_READ_S16(domain_address);
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

 	//array to store the data to be read or sent
    int16_t read_data_[4] = {0}; //example

private:
    ec_pdo_entry_info_t channels_[44] = {
    {0x6000, 0x01, 1}, /* Underrange */
    {0x6000, 0x02, 1}, /* Overrange */
    {0x6000, 0x03, 2}, /* Limit 1 */
    {0x6000, 0x05, 2}, /* Limit 2 */
    {0x6000, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6000, 0x0e, 1}, /* Sync error */
    {0x6000, 0x0f, 1}, /* TxPDO State */
    {0x6000, 0x10, 1}, /* TxPDO Toggle */
    {0x6000, 0x11, 16}, /* Value */
    {0x6010, 0x01, 1}, /* Underrange */
    {0x6010, 0x02, 1}, /* Overrange */
    {0x6010, 0x03, 2}, /* Limit 1 */
    {0x6010, 0x05, 2}, /* Limit 2 */
    {0x6010, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6010, 0x0e, 1}, /* Sync error */
    {0x6010, 0x0f, 1}, /* TxPDO State */
    {0x6010, 0x10, 1}, /* TxPDO Toggle */
    {0x6010, 0x11, 16}, /* Value */
    {0x6020, 0x01, 1}, /* Underrange */
    {0x6020, 0x02, 1}, /* Overrange */
    {0x6020, 0x03, 2}, /* Limit 1 */
    {0x6020, 0x05, 2}, /* Limit 2 */
    {0x6020, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6020, 0x0e, 1}, /* Sync error */
    {0x6020, 0x0f, 1}, /* TxPDO State */
    {0x6020, 0x10, 1}, /* TxPDO Toggle */
    {0x6020, 0x11, 16}, /* Value */
    {0x6030, 0x01, 1}, /* Underrange */
    {0x6030, 0x02, 1}, /* Overrange */
    {0x6030, 0x03, 2}, /* Limit 1 */
    {0x6030, 0x05, 2}, /* Limit 2 */
    {0x6030, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6030, 0x0e, 1}, /* Sync error */
    {0x6030, 0x0f, 1}, /* TxPDO State */
    {0x6030, 0x10, 1}, /* TxPDO Toggle */
    {0x6030, 0x11, 16}, /* Value */
    };

    ec_pdo_info_t pdos_[4] = {
    {0x1a00, 11, channels_ + 0}, /* AI TxPDO-Map Standard Ch.1 */
    {0x1a02, 11, channels_ + 11}, /* AI TxPDO-Map Standard Ch.2 */
    {0x1a04, 11, channels_ + 22}, /* AI TxPDO-Map Standard Ch.3 */
    {0x1a06, 11, channels_ + 33}, /* AI TxPDO-Map Standard Ch.4 */
    };

    ec_sync_info_t syncs_[2] = {
      	{3, EC_DIR_INPUT, 4, pdos_ + 0, EC_WD_ENABLE},
        {0xff}
    };

    DomainMap domains_ = {
        {0, {10,21,32,43} }
    };
};

}

#endif

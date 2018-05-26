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
 * \class Slave
 *
 * \ingroup SimplECat
 *
 * \brief Base class for a beckhoff slave module
 *
 * The device data to fill in this class can be found using EtherLab
 * /opt/etherlab/bin$ sudo ./ethercat cstructs
 * this will return the vendor_id, product_it, channels, pdos, syncs
 * modify the pdos and syncs according to the data of interest
 * select the pdos for the domain from the channels list.
 *
 * NOTE: No error checking on indices.
 *       If index exceed array dimensions, Master will segfault.
 */


#ifndef SIMPLECAT_SLAVE_H_
#define SIMPLECAT_SLAVE_H_

#include "ecrt.h"
#include <map>
#include <vector>

namespace simplecat {


class Slave
{
public:

    Slave(uint32_t vendor_id, uint32_t product_id) :
        vendor_id_(vendor_id),
        product_id_(product_id) {}

    virtual ~Slave() {}

    /** read or write data to the domain */
    virtual void processData(size_t index, uint8_t* domain_address){
        //read/write macro needs to match data type. e.g.
        //read_data_[index] = EC_READ_U8(domain_address)
        //EC_WRITE_S16(domain_address, write_data_[index]);
    }

    /** a pointer to syncs. return &syncs[0] */
    virtual const ec_sync_info_t* syncs() {
        //return &syncs_[0];
        return NULL;
    }

    /** number of elements in the syncs array. */
    virtual size_t syncSize() {
        //return sizeof(syncs_)/sizeof(ec_sync_info_t);
        return 0;
    }

    /** a pointer to all PDO entries */
    virtual const ec_pdo_entry_info_t* channels() {
        //return channels_;
        return NULL;
    }

    /** a map from domain index to pdo indices in that domain.
     *  map<domain index, vector<channels_ indices> > */
    typedef std::map<unsigned int, std::vector<unsigned int> > DomainMap;

    virtual void domains(DomainMap& domains) const {
        //domains = domains;
    }

    const uint32_t vendor_id_;
    const uint32_t product_id_;

    // //array to store the data to be read or sent
    //uint8_t               read_data_[E]; //example
    //int16_t               write_data_[F]; //example

protected:
    // //see SETUP_ETHERLAB.md for explanation
    //ec_pdo_entry_info_t   channels_[A];
    //ec_pdo_info_t         pdos_[B];
    //ec_sync_info_t        syncs_[C];
    //ec_pdo_entry_info_t   domain_regs_[D];

    //DomainMap domains_;
};

}

#endif

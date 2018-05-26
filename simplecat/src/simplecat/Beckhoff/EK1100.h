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
 * \class Beckhoff_EK1100
 *
 * \ingroup SimplECAT
 *
 * \brief Beckhoff EK1100
 *
 * EtherCAT coupler with input and output EtherCAT port
 **/


#ifndef SIMPLECAT_BECKHOFF_EK1100_H_
#define SIMPLECAT_BECKHOFF_EK1100_H_


#include <simplecat/Slave.h>


namespace simplecat {


class Beckhoff_EK1100 : public Slave
{
public:
    Beckhoff_EK1100() : Slave(0x00000002, 0x044c2c52) {}
    virtual ~Beckhoff_EK1100() {}
};


}

#endif

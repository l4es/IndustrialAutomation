/*****************************************************************************
 *
 *  $Id$
 *
 *  copyright (C) 2012 Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdserv library.
 *
 *  The pdserv library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdserv library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef DATATYPE_H
#define DATATYPE_H

#include "Debug.h"

#include <list>
#include <stdint.h>
#include <vector>
#include <string>
#include <ostream>

namespace PdServ {

class DataType {
    public:
        struct DimType: public std::vector<size_t> {
            DimType(size_t ndims, const size_t *dim);
            const size_t nelem;

            bool isScalar() const {
                return nelem == 1;
            }

            bool isVector() const {
                return size() == 1;
            }

            bool isMultiDim() const {
                return size() > 1;
            }

        };

        struct Field {
            Field(const std::string& name, const DataType& type,
                    size_t offset, size_t ndims, const size_t *dims);
            const std::string name;
            const DataType& type;
            const size_t offset;
            const DimType dim;
        };

        enum Primary {
            compound_T = 0,
            boolean_T,
            uint8_T,  int8_T,
            uint16_T, int16_T,
            uint32_T, int32_T,
            uint64_T, int64_T,
            double_T, single_T
        };

        DataType( const std::string& name, size_t size);
        virtual ~DataType();

        const std::string name;
        const size_t size;
        virtual Primary primary() const;
        bool isPrimary() const;
        static const size_t maxWidth = 8; /**< Maximum supported data type
                                            size in bytes */

        virtual size_t align() const;

        void addField(const std::string& name,
                const DataType& type,
                size_t offset,
                size_t ndims = 1,
                const size_t* dims = 0);

        struct FieldList: public std::list<const Field*> {
            friend std::ostream& operator<<(std::ostream& os,
                    const FieldList& fieldList);
        };
        const FieldList& getFieldList() const;

        bool operator==(const DataType& other) const;
        bool operator!=(const DataType& other) const;
        void (* const setValue)(char *&, double);

        static const DataType& boolean;
        static const DataType&   uint8;
        static const DataType&    int8;
        static const DataType&  uint16;
        static const DataType&   int16;
        static const DataType&  uint32;
        static const DataType&   int32;
        static const DataType&  uint64;
        static const DataType&   int64;
        static const DataType& float64;
        static const DataType& float32;

        virtual void print(std::ostream& os, const char *data,
                const char* start, const char* end) const;

        struct Printer {
            Printer(const DataType* dt, const char *addr, size_t nelem):
                type(dt), addr(addr), size(nelem * dt->size) {
                }

            friend std::ostream& operator<<(std::ostream& os,
                    const Printer& obj) {
                obj.type->print(os, obj.addr, obj.addr,
                        obj.addr + obj.size);
                return os;
            }

            const DataType* const type;
            const char * const addr;
            const size_t size;
        };

        Printer operator()(const void *addr, size_t n = 1) const {
            return Printer(this, reinterpret_cast<const char*>(addr), n);
        }

        //////////////  Iterator to walk through the DataType
        // Requires a class with the following members:
        //    bool newVariable(
        //            const PdServ::DataType& dtype,
        //            const PdServ::DataType::DimType& dim,
        //            size_t dimIdx, size_t elemIdx, size_t offset) const;
        //    void newDimension(
        //            const PdServ::DataType& dtype,
        //            const PdServ::DataType::DimType& dim,
        //            size_t dimIdx, size_t elemIdx,
        //            CreateChannels& c, size_t offset);
        //    void newField(const PdServ::DataType::Field *field,
        //            CreateChannels& c, size_t offset);

        template <class C>
            struct Iterator {
                Iterator(const DataType& dtype,
                        const DimType& dim, const C& c);

                void dispatch(
                        const DataType& dtype, size_t dtypeSize,
                        const DimType& dim, size_t dimIdx,
                        size_t elemIdx, C c, size_t offset);

                void iterateDims(
                        const DataType& dtype, size_t dtypeSize,
                        const DimType& dim,
                        size_t dimIdx, size_t elemIdx, C& c, size_t offset);

                void iterateFields(const FieldList& fieldList,
                        C& c, size_t offset);
            };


    protected:
        DataType(const DataType& other);
        explicit DataType(size_t size, void (*)(char *&dst, double src));

    private:

        FieldList fieldList;
};

/////////////////////////////////////////////////////////////////////////////
    template <class C>
DataType::Iterator<C>::Iterator(const DataType& dtype,
        const DataType::DimType& dim, const C& c)
{
    dispatch(dtype, dtype.size * dim.nelem, dim, 0, 0, c, 0);
}


/////////////////////////////////////////////////////////////////////////////
    template <class C>
void DataType::Iterator<C>::dispatch(
        const DataType& dtype, size_t dtypeSize, const DimType& dim,
        size_t dimIdx, size_t elemIdx, C c, size_t offset)
{
    if (c.newVariable(dtype,dim,dimIdx,elemIdx,offset))
        return;

    if (!dim.isScalar() and dimIdx != dim.size())
        iterateDims(dtype, dtypeSize, dim, dimIdx, elemIdx, c, offset);
    else if (!dtype.isPrimary())
        iterateFields(dtype.getFieldList(), c, offset);
}

/////////////////////////////////////////////////////////////////////////////
    template <class C>
void DataType::Iterator<C>::iterateDims(
        const DataType& dtype, size_t dtypeSize, const DimType& dim,
        size_t dimIdx, size_t /*elemIdx*/, C& c, size_t offset)
{
    size_t end = dim[dimIdx];
    dtypeSize /= dim[dimIdx];
    ++dimIdx;
    for (size_t i = 0; i < end; ++i) {
        c.newDimension(dtype, dim, dimIdx, i, c, offset);
        dispatch(dtype, dtypeSize, dim, dimIdx, i, c, offset);
        offset += dtypeSize;
    }
}

/////////////////////////////////////////////////////////////////////////////
    template <class C>
void DataType::Iterator<C>::iterateFields(
        const FieldList& fieldList, C& c, size_t offset)
{
    for (FieldList::const_iterator it = fieldList.begin();
            it != fieldList.end(); ++it) {
        const Field *field = *it;
        c.newField(field, c, offset + field->offset);
        dispatch(field->type, field->type.size, field->dim,
                0, 0, c, offset + field->offset);
    }
}

};

#endif //DATATYPE_H

/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright (C) 2012 Richard Hacker (lerichi at gmx dot net)
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

#include "DataType.h"
#include "Debug.h"
#include <stdint.h>
#include <functional>   // std::multiplies
#include <numeric>      // std::accumulate


using namespace PdServ;

template <class T>
    struct TypeNum {};

template <>
    struct TypeNum<bool> {
        static const DataType::Primary value = DataType::boolean_T;
        static const size_t align = sizeof(bool);
    };

template <>
    struct TypeNum<uint8_t> {
        static const DataType::Primary value = DataType::uint8_T;
        static const size_t align = sizeof(uint8_t);
    };

template <>
    struct TypeNum<int8_t> {
        static const DataType::Primary value = DataType::int8_T;
        static const size_t align = sizeof(int8_t);
    };

template <>
    struct TypeNum<uint16_t> {
        static const DataType::Primary value = DataType::uint16_T;
        static const size_t align = sizeof(uint16_t);
    };

template <>
    struct TypeNum<int16_t> {
        static const DataType::Primary value = DataType::int16_T;
        static const size_t align = sizeof(int16_t);
    };

template <>
    struct TypeNum<uint32_t> {
        static const DataType::Primary value = DataType::uint32_T;
        static const size_t align = sizeof(uint32_t);
    };

template <>
    struct TypeNum<int32_t> {
        static const DataType::Primary value = DataType::int32_T;
        static const size_t align = sizeof(int32_t);
    };

template <>
    struct TypeNum<uint64_t> {
        static const DataType::Primary value = DataType::uint64_T;
        static const size_t align = sizeof(uint64_t);
    };

template <>
    struct TypeNum<int64_t> {
        static const DataType::Primary value = DataType::int64_T;
        static const size_t align = sizeof(int64_t);
    };

template <>
    struct TypeNum<double> {
        static const DataType::Primary value = DataType::double_T;
        static const size_t align = sizeof(double);
    };

template <>
    struct TypeNum<float> {
        static const DataType::Primary value = DataType::single_T;
        static const size_t align = sizeof(float);
    };

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
namespace {
    template <class T>
        class PrimaryType: public DataType {
            public:
                PrimaryType(): DataType(sizeof(T), setValue) {
                }

            private:
                void print(std::ostream& os, const char *data,
                        const char* start, const char *end) const {
                    const T* val = reinterpret_cast<const T*>(data);
                    const T* lastVal = reinterpret_cast<const T*>(end);
                    char delim = 0;

                    for (;val < lastVal; val++) {
                        if ((const char*)val < start)
                            continue;

                        if (delim)
                            os << delim;
                        delim = ',';

                        os << *val;
                    }
                }

                DataType::Primary primary() const {
                    return TypeNum<T>::value;
                }

                size_t align () const {
                    return TypeNum<T>::align;
                }

                static void setValue(char *&dst, double src) {
                    *reinterpret_cast<T*>(dst) = static_cast<T>(src);
                    dst += sizeof(T);
                }
        };

    template<>
        void PrimaryType<int8_t>::print(std::ostream& os, const char *data,
                const char* start, const char *end) const {
            const int8_t* val = reinterpret_cast<const int8_t*>(data);
            const int8_t* lastVal = reinterpret_cast<const int8_t*>(end);
            char delim = 0;

            for (;val < lastVal; val++) {
                if ((const char*)val < start)
                    continue;

                if (delim)
                    os << delim;
                delim = ',';

                os << (int)*val;
            }
        }

    template<>
        void PrimaryType<uint8_t>::print(std::ostream& os, const char *data,
                const char* start, const char *end) const {
            const uint8_t* val = reinterpret_cast<const uint8_t*>(data);
            const uint8_t* lastVal = reinterpret_cast<const uint8_t*>(end);
            char delim = 0;

            for (;val < lastVal; val++) {
                if ((const char*)val < start)
                    continue;

                if (delim)
                    os << delim;
                delim = ',';

                os << (unsigned)*val;
            }
        }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const DataType& DataType::boolean = PrimaryType<    bool>();
const DataType& DataType::  uint8 = PrimaryType< uint8_t>();
const DataType& DataType::   int8 = PrimaryType<  int8_t>();
const DataType& DataType:: uint16 = PrimaryType<uint16_t>();
const DataType& DataType::  int16 = PrimaryType< int16_t>();
const DataType& DataType:: uint32 = PrimaryType<uint32_t>();
const DataType& DataType::  int32 = PrimaryType< int32_t>();
const DataType& DataType:: uint64 = PrimaryType<uint64_t>();
const DataType& DataType::  int64 = PrimaryType< int64_t>();
const DataType& DataType::float64 = PrimaryType<  double>();
const DataType& DataType::float32 = PrimaryType<   float>();

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataType::DimType::DimType (size_t ndims, const size_t *dim):
    std::vector<size_t>(dim, dim + ndims),
//    std::vector<size_t>(dim ? dim : &ndims, dim ? dim + ndims : &ndims + 1),
    nelem(std::accumulate(begin(), end(), 1U, std::multiplies<size_t>()))
{
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataType::Field::Field (const std::string& name, const DataType& type,
        size_t offset, size_t ndims, const size_t *dims):
    name(name), type(type), offset(offset), dim(ndims, dims)
{
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataType::DataType (const std::string& name, size_t size):
    name(name), size(size), setValue(0)
{
}

//////////////////////////////////////////////////////////////////////
DataType::DataType (const DataType& other):
    name(other.name), size(other.size), setValue(other.setValue)
{
}

//////////////////////////////////////////////////////////////////////
DataType::DataType (size_t size, void (*setValue)(char *&, double )):
    size(size), setValue(setValue)
{
}

//////////////////////////////////////////////////////////////////////
DataType::~DataType ()
{
    for (FieldList::const_iterator it = fieldList.begin();
            it != fieldList.end(); ++it)
        delete *it;
}

//////////////////////////////////////////////////////////////////////
bool DataType::isPrimary () const
{
    return primary() != compound_T;
}

//////////////////////////////////////////////////////////////////////
DataType::Primary DataType::primary () const
{
    return compound_T;
}

//////////////////////////////////////////////////////////////////////
bool DataType::operator==(const DataType& other) const
{
    return this == &other;
}

//////////////////////////////////////////////////////////////////////
bool DataType::operator!=(const DataType& other) const
{
    return this != &other;
}

//////////////////////////////////////////////////////////////////////
void DataType::print(std::ostream& os,
        const char *data, const char *start, const char *end) const
{
    char delim = 0;

    for (; data < end; data += this->size) {
        for (FieldList::const_iterator it = fieldList.begin();
                it != fieldList.end(); ++it) {

            const char *p1 = data + (*it)->offset;
            if (p1 < start)
                continue;

            if (delim)
                os << delim;
            delim = ',';

            const char *p2 = p1 + (*it)->dim.nelem * (*it)->type.size;
            if (p2 < end)
                (*it)->type.print(os, p1, start, p2);
            else {
                (*it)->type.print(os, p1, start, end);
                return;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
void DataType::addField (const std::string& name, const DataType& type,
        size_t offset, size_t ndims, const size_t* dims)
{
    fieldList.push_back(dims
            ? new Field(name, type, offset, ndims, dims)
            : new Field(name, type, offset, 1, &ndims));
}

//////////////////////////////////////////////////////////////////////
const DataType::FieldList& DataType::getFieldList () const
{
    return fieldList;
}

//////////////////////////////////////////////////////////////////////
size_t DataType::align () const
{
    size_t n = 1U;
    for (FieldList::const_iterator it = fieldList.begin();
            it != fieldList.end(); ++it) {
        size_t n1 = (*it)->type.align();
        if (n < n1)
            n = n1;
    }

    return n;
}

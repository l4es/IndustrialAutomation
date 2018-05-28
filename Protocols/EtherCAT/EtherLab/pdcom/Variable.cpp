/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "pdcom/Variable.h"
#include "Debug.h"

#include <stdint.h>
#include <sstream>
#include <algorithm>
#include <numeric>

using namespace PdCom;

//////////////////////////////////////////////////////////////////////
static const size_t _typeWidth[11] = {
    sizeof(    bool),
    sizeof( uint8_t), sizeof( int8_t),
    sizeof(uint16_t), sizeof(int16_t),
    sizeof(uint32_t), sizeof(int32_t),
    sizeof(uint64_t), sizeof(int64_t),
    sizeof(  double), sizeof(  float),
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <typename T> struct UserTypeIdx {};
template <> struct UserTypeIdx<    bool> { static const size_t id = 0; };
template <> struct UserTypeIdx<    char> { static const size_t id = 1; };
template <> struct UserTypeIdx< uint8_t> { static const size_t id = 2; };
template <> struct UserTypeIdx<  int8_t> { static const size_t id = 3; };
template <> struct UserTypeIdx<uint16_t> { static const size_t id = 4; };
template <> struct UserTypeIdx< int16_t> { static const size_t id = 5; };
template <> struct UserTypeIdx<uint32_t> { static const size_t id = 6; };
template <> struct UserTypeIdx< int32_t> { static const size_t id = 7; };
template <> struct UserTypeIdx<uint64_t> { static const size_t id = 8; };
template <> struct UserTypeIdx< int64_t> { static const size_t id = 9; };
template <> struct UserTypeIdx<  double> { static const size_t id = 10; };
template <> struct UserTypeIdx<   float> { static const size_t id = 11; };

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
template <typename T>
struct TypeNum {};

template <>
struct TypeNum<bool> {
    static const size_t id = Variable::boolean_T;
    static const char* const cname;
};
template <>
struct TypeNum<double> {
    static const size_t id = Variable::double_T;
    static const char* const cname;
};
template <>
struct TypeNum<float> {
    static const size_t id = Variable::single_T;
    static const char* const cname;
};
template <>
struct TypeNum<uint8_t> {
    static const size_t id = Variable::uint8_T;
    static const char* const cname;
};
template <>
struct TypeNum<uint16_t> {
    static const size_t id = Variable::uint16_T;
    static const char* const cname;
};
template <>
struct TypeNum<uint32_t> {
    static const size_t id = Variable::uint32_T;
    static const char* const cname;
};
template <>
struct TypeNum<uint64_t> {
    static const size_t id = Variable::uint64_T;
    static const char* const cname;
};
template <>
struct TypeNum<int8_t> {
    static const size_t id = Variable::int8_T;
    static const char* const cname;
};
template <>
struct TypeNum<int16_t> {
    static const size_t id = Variable::int16_T;
    static const char* const cname;
};
template <>
struct TypeNum<int32_t> {
    static const size_t id = Variable::int32_T;
    static const char* const cname;
};
template <>
struct TypeNum<int64_t> {
    static const size_t id = Variable::int64_T;
    static const char* const cname;
};

const char* const TypeNum<    bool>::cname =     "bool";
const char* const TypeNum<  int8_t>::cname =   "int8_t";
const char* const TypeNum< int16_t>::cname =  "int16_t";
const char* const TypeNum< int32_t>::cname =  "int32_t";
const char* const TypeNum< int64_t>::cname =  "int64_t";
const char* const TypeNum< uint8_t>::cname =  "uint8_t";
const char* const TypeNum<uint16_t>::cname = "uint16_t";
const char* const TypeNum<uint32_t>::cname = "uint32_t";
const char* const TypeNum<uint64_t>::cname = "uint64_t";
const char* const TypeNum<   float>::cname =    "float";
const char* const TypeNum<  double>::cname =   "double";

const char* const _ctype[] = {
    TypeNum<    bool>::cname,
    TypeNum< uint8_t>::cname, TypeNum<  int8_t>::cname,
    TypeNum<uint16_t>::cname, TypeNum< int16_t>::cname,
    TypeNum<uint32_t>::cname, TypeNum< int32_t>::cname,
    TypeNum<uint64_t>::cname, TypeNum< int64_t>::cname,
    TypeNum<  double>::cname, TypeNum<   float>::cname,
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// getValue() is used to cast the variable's value with its native
// data type to the data type required by the caller
template <class CType, class UserType>
void Variable::getValue(
        void* _dst, size_t nelem, const char* _src, size_t offset) const
{
    const CType* src = reinterpret_cast<const CType*>(_src) + offset;

    std::copy(src, src + nelem, reinterpret_cast<UserType*>(_dst));
}

// CAREFUL: Index this using UserTypeIdx
template <class CType>
const Variable::GetValueFunc_T
Variable::DataConverter<CType>::getValueFunc[] = {
    &Variable::getValue<CType,    bool>,
    &Variable::getValue<CType,    char>,
    &Variable::getValue<CType, uint8_t>,
    &Variable::getValue<CType,  int8_t>,
    &Variable::getValue<CType,uint16_t>,
    &Variable::getValue<CType, int16_t>,
    &Variable::getValue<CType,uint32_t>,
    &Variable::getValue<CType, int32_t>,
    &Variable::getValue<CType,uint64_t>,
    &Variable::getValue<CType, int64_t>,
    &Variable::getValue<CType,  double>,
    &Variable::getValue<CType,   float>,
};

const Variable::GetValueFunc_T* const Variable::getValueFuncList[] = {
    DataConverter<    bool>::getValueFunc,
    DataConverter< uint8_t>::getValueFunc,
    DataConverter<  int8_t>::getValueFunc,
    DataConverter<uint16_t>::getValueFunc,
    DataConverter< int16_t>::getValueFunc,
    DataConverter<uint32_t>::getValueFunc,
    DataConverter< int32_t>::getValueFunc,
    DataConverter<uint64_t>::getValueFunc,
    DataConverter< int64_t>::getValueFunc,
    DataConverter<  double>::getValueFunc,
    DataConverter<   float>::getValueFunc,
};

//////////////////////////////////////////////////////////////////////
// These functions are used to cast any data type to the variable's
// internal data type
template <class CType, class UserType>
void Variable::copyValue(char* _dst, const void* _src, size_t nelem) const
{
    const UserType* src = reinterpret_cast<const UserType*>(_src);

    std::copy(src, src + nelem, reinterpret_cast<CType*>(_dst));
}

// CAREFUL: Index this using UserTypeIdx
template <class CType>
const Variable::CopyValueFunc_T
Variable::DataConverter<CType>::copyValueFunc[] = {
    &Variable::copyValue<CType,    bool>,
    &Variable::copyValue<CType,    char>,
    &Variable::copyValue<CType, uint8_t>,
    &Variable::copyValue<CType,  int8_t>,
    &Variable::copyValue<CType,uint16_t>,
    &Variable::copyValue<CType, int16_t>,
    &Variable::copyValue<CType,uint32_t>,
    &Variable::copyValue<CType, int32_t>,
    &Variable::copyValue<CType,uint64_t>,
    &Variable::copyValue<CType, int64_t>,
    &Variable::copyValue<CType,  double>,
    &Variable::copyValue<CType,   float>,
};

const Variable::CopyValueFunc_T* const Variable::copyValueFuncList[] = {
    DataConverter<    bool>::copyValueFunc,
    DataConverter< uint8_t>::copyValueFunc,
    DataConverter<  int8_t>::copyValueFunc,
    DataConverter<uint16_t>::copyValueFunc,
    DataConverter< int16_t>::copyValueFunc,
    DataConverter<uint32_t>::copyValueFunc,
    DataConverter< int32_t>::copyValueFunc,
    DataConverter<uint64_t>::copyValueFunc,
    DataConverter< int64_t>::copyValueFunc,
    DataConverter<  double>::copyValueFunc,
    DataConverter<   float>::copyValueFunc,
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
inline T        convert(      T val) { return val; }
inline int      convert( int8_t val) { return val; }
inline unsigned convert(uint8_t val) { return val; }

template <class T>
void Variable::printFcn(std::ostream& os, const char* buf, char delim) const
{
    const T* value = reinterpret_cast<const T*>(buf);

    if (nelem > 1)
        os << '[';

    for (size_t i = 0; i < nelem; ++i) {
        if (i)
            os << delim;
        os << convert(*value++);
    }

    if (nelem > 1)
        os << ']';
}

const Variable::PrintValueFunc_T Variable::printValueFuncList[] = {
    &Variable::printFcn<    bool>,
    &Variable::printFcn< uint8_t>,
    &Variable::printFcn<  int8_t>,
    &Variable::printFcn<uint16_t>,
    &Variable::printFcn< int16_t>,
    &Variable::printFcn<uint32_t>,
    &Variable::printFcn< int32_t>,
    &Variable::printFcn<uint64_t>,
    &Variable::printFcn< int64_t>,
    &Variable::printFcn<  double>,
    &Variable::printFcn<   float>,
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
inline void read(std::istream& is, T& val) { is >> val; }

inline void read(std::istream& is,   char& val) { 
    int i;
    is >> i;
    val = i;
}

inline void read(std::istream& is, int8_t& val) { 
    int i;
    is >> i;
    val = i;
}

inline void read(std::istream& is, uint8_t& val) { 
    unsigned i;
    is >> i;
    val = i;
}

template <class T>
bool Variable::readFcn(std::istream& is) const
{
    T val[nelem];
    char delim = '\0';
    size_t count;
    
    if (nelem > 0 and is.peek() == '[')
        is >> delim;

    for (count = 0; count < nelem and !is.eof(); ++count) {
        if (count)
            is.ignore();

        ::read(is, val[count]);
    }

    if (delim)
        is.ignore();

    return bool(is) and setValue(val, 0, count);
}

const Variable::ReadValueFunc_T Variable::readValueFuncList[] = {
    &Variable::readFcn<    bool>,
    &Variable::readFcn< uint8_t>,
    &Variable::readFcn<  int8_t>,
    &Variable::readFcn<uint16_t>,
    &Variable::readFcn< int16_t>,
    &Variable::readFcn<uint32_t>,
    &Variable::readFcn< int32_t>,
    &Variable::readFcn<uint64_t>,
    &Variable::readFcn< int64_t>,
    &Variable::readFcn<  double>,
    &Variable::readFcn<   float>,
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Variable::Variable(
        Type type,
        double sampleTime,
        size_t task,
        bool writeable,
        const std::string& alias,
        size_t ndim,
        const size_t *dims):
    type(type), ctype(_ctype[type]),
    sampleTime(sampleTime), task(task), writeable(writeable),
    alias(alias),
    nelem(std::accumulate(dims, dims + ndim, 1, std::multiplies<size_t>())),
    dim(dims, dims + ndim), typeWidth(_typeWidth[type]),
    bytes(nelem * typeWidth),
    getValueFunc(getValueFuncList[type]),
    copyValueFunc(copyValueFuncList[type]),
    printValueFunc(printValueFuncList[type]),
    readValueFunc(readValueFuncList[type])
{
    //    log_debug("Var %p", this);
}

//////////////////////////////////////////////////////////////////////
Variable::~Variable()
{
//    log_debug("Del var %p", this);
}

//////////////////////////////////////////////////////////////////////
bool Variable::isScalar() const
{
    return dim[0] == 1 and dim.size() == 1;
}

//////////////////////////////////////////////////////////////////////
size_t Variable::isVector() const
{
    return dim[0] > 1 and dim.size() == 1;
}

//////////////////////////////////////////////////////////////////////
void Variable::copyValue(char* dst, const void* src, size_t t, size_t n) const
{
    (this->*copyValueFunc[t])(dst, src, n);
}

//////////////////////////////////////////////////////////////////////
void Variable::printValue(std::ostream& os, const char* buf, char delim) const
{
    (this->*printValueFunc)(os, buf, delim);
}

//////////////////////////////////////////////////////////////////////
bool Variable::read(std::istream& is) const
{
    return (this->*readValueFunc)(is);
}

//////////////////////////////////////////////////////////////////////
std::istream& operator>> (std::istream& is, const Variable& var)
{
    var.read(is);
    return is;
}

//////////////////////////////////////////////////////////////////////
bool Variable::setStringValue(const std::string& str) const
{
    std::istringstream is(str);
    return read(is) and bool(is);
}

//////////////////////////////////////////////////////////////////////
template <typename T>
bool Variable::setValue(
        const T* val, size_t offset, size_t count) const
{
    if (offset + count > nelem)
        return true;

    return setValue(val, UserTypeIdx<T>::id, offset, count);
}
template bool Variable::setValue(const   double *, size_t, size_t) const;
template bool Variable::setValue(const    float *, size_t, size_t) const;
template bool Variable::setValue(const     bool *, size_t, size_t) const;
template bool Variable::setValue(const     char *, size_t, size_t) const;
template bool Variable::setValue(const  uint8_t *, size_t, size_t) const;
template bool Variable::setValue(const uint16_t *, size_t, size_t) const;
template bool Variable::setValue(const uint32_t *, size_t, size_t) const;
template bool Variable::setValue(const uint64_t *, size_t, size_t) const;
template bool Variable::setValue(const   int8_t *, size_t, size_t) const;
template bool Variable::setValue(const  int16_t *, size_t, size_t) const;
template bool Variable::setValue(const  int32_t *, size_t, size_t) const;
template bool Variable::setValue(const  int64_t *, size_t, size_t) const;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Variable::Subscription::Subscription(Process* process, int id,
        const Variable* v, int decimation,
        const uint64_t * const* timePtr, const char * const* dataPtr):
    process(process),
    id(id), variable(v), decimation(decimation),
    time_ns(timePtr), data(dataPtr)
{
}

//////////////////////////////////////////////////////////////////////
Variable::Subscription::~Subscription()
{
//    log_debug("%p", this);
}

//////////////////////////////////////////////////////////////////////
bool Variable::Subscription::poll() const
{
    if (decimation < 0)
        return variable->poll(this);
    return true;
}

//////////////////////////////////////////////////////////////////////
template <typename T>
void Variable::Subscription::getValue(
        T* val, size_t offset, size_t nelem) const
{
    const GetValueFunc_T getValue = variable->getValueFunc[UserTypeIdx<T>::id];
    (variable->*getValue)(val, nelem, *data, offset);
}
template void Variable::Subscription::getValue(  double*, size_t, size_t) const;
template void Variable::Subscription::getValue(   float*, size_t, size_t) const;
template void Variable::Subscription::getValue(    bool*, size_t, size_t) const;
template void Variable::Subscription::getValue(    char*, size_t, size_t) const;
template void Variable::Subscription::getValue( uint8_t*, size_t, size_t) const;
template void Variable::Subscription::getValue(uint16_t*, size_t, size_t) const;
template void Variable::Subscription::getValue(uint32_t*, size_t, size_t) const;
template void Variable::Subscription::getValue(uint64_t*, size_t, size_t) const;
template void Variable::Subscription::getValue(  int8_t*, size_t, size_t) const;
template void Variable::Subscription::getValue( int16_t*, size_t, size_t) const;
template void Variable::Subscription::getValue( int32_t*, size_t, size_t) const;
template void Variable::Subscription::getValue( int64_t*, size_t, size_t) const;

//////////////////////////////////////////////////////////////////////
std::string Variable::Subscription::getStringValue(char delimiter) const
{
    std::ostringstream os;
    print(os, delimiter);
    return os.str();
}

//////////////////////////////////////////////////////////////////////
void Variable::Subscription::print(std::ostream& os, char delim) const
{
    variable->printValue(os, *data, delim);
}

//////////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os,
        const Variable::Subscription& subscription)
{
    subscription.print(os, ' ');
    return os;
}

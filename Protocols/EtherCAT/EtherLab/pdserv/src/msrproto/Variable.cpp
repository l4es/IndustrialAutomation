/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
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

#include "Variable.h"
#include "XmlElement.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
Variable::Variable (const PdServ::Variable* v, size_t index,
        const PdServ::DataType& dtype, const PdServ::DataType::DimType& dim,
        size_t offset):
    variable(v), index(index),
    dtype(dtype), dim(dim), offset(offset),
    memSize(dtype.size * dim.nelem),
    hidden(false)
{
}

/////////////////////////////////////////////////////////////////////////////
void Variable::setDataType(XmlElement &element, const PdServ::DataType& dtype,
        const PdServ::DataType::DimType& dim) const
{
    // datasize=
    XmlElement::Attribute(element, "datasize") << dtype.size;

    // typ=
    const char *dtstr;
    switch (dtype.primary()) {
        case PdServ::DataType::boolean_T : dtstr = "TCHAR";     break;
        case PdServ::DataType::  uint8_T : dtstr = "TUCHAR";    break;
        case PdServ::DataType::   int8_T : dtstr = "TCHAR";     break;
        case PdServ::DataType:: uint16_T : dtstr = "TUSHORT";   break;
        case PdServ::DataType::  int16_T : dtstr = "TSHORT";    break;
        case PdServ::DataType:: uint32_T : dtstr = "TUINT";     break;
        case PdServ::DataType::  int32_T : dtstr = "TINT";      break;
        case PdServ::DataType:: uint64_T : dtstr = "TULINT";    break;
        case PdServ::DataType::  int64_T : dtstr = "TLINT";     break;
        case PdServ::DataType:: double_T : dtstr = "TDBL";      break;
        case PdServ::DataType:: single_T : dtstr = "TFLT";      break;
        default                          : dtstr = "COMPOUND";  break;
    }
    if (!dim.isScalar())
        XmlElement::Attribute(element, "typ")
            << dtstr
            << (dim.isVector() ? "_LIST" : "_MATRIX");
    else
        XmlElement::Attribute(element, "typ") << dtstr;

    // For vectors:
    // anz=
    // cnum=
    // rnum=
    // orientation=
    if (!dim.isScalar()) {
        XmlElement::Attribute(element, "anz") << dim.nelem;
        const char *orientation;
        size_t cnum, rnum;

        // Transmit either a vector or a matrix
        if (dim.isVector()) {
            cnum = dim.nelem;
            rnum = 1;
            orientation = "VECTOR";
        }
        else {
            cnum = dim.back();
            rnum = dim.nelem / cnum;
            orientation = "MATRIX_ROW_MAJOR";
        }

        XmlElement::Attribute(element, "cnum") << cnum;
        XmlElement::Attribute(element, "rnum") << rnum;
        XmlElement::Attribute(element, "orientation") << orientation;
    }
}

/////////////////////////////////////////////////////////////////////////////
void Variable::addCompoundFields(XmlElement &element,
        const PdServ::DataType& dtype) const
{
    const PdServ::DataType::FieldList& fieldList = dtype.getFieldList();

    for (PdServ::DataType::FieldList::const_iterator it = fieldList.begin();
            it != fieldList.end(); ++it) {
        XmlElement field(element.createChild("field"));
        XmlElement::Attribute(field, "name").setEscaped((*it)->name);
        XmlElement::Attribute(field, "offset") << (*it)->offset;

        setDataType(element, (*it)->type, (*it)->dim);

        addCompoundFields(field, (*it)->type);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Variable::setAttributes(
        XmlElement &element, bool shortReply) const
{
    // index=
    XmlElement::Attribute(element, "index") << index;

    // name=
    // DO NOT REMOVE: Testmanager requires name even with short reply!
    XmlElement::Attribute(element, "name").setEscaped(path());

    if (shortReply)
        return;

    // alias=
    // comment=
    if (!variable->alias.empty())
        XmlElement::Attribute(element, "alias") << variable->alias;
    if (!variable->comment.empty())
        XmlElement::Attribute(element, "comment") << variable->comment;

    setDataType(element, dtype, dim);

    // text=
    // FIXME:  variable->comment is used twice! 2012-12-14 fp
    if (!variable->comment.empty())
        XmlElement::Attribute(element, "text") << variable->comment;

    if (childCount())
        XmlElement::Attribute(element, "dir") << 1;

    // hide=
    // unhide=
}

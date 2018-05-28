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

#include "Parameter.h"
#include "XmlElement.h"
#include "Session.h"
#include "../Parameter.h"
#include "../Debug.h"
#include "../DataType.h"

#include <sstream>
#include <cerrno>
#include <stdint.h>

#define MSR_R   0x01    /* Parameter is readable */
#define MSR_W   0x02    /* Parameter is writeable */
#define MSR_WOP 0x04    /* Parameter is writeable in real-time */
//#define MSR_MONOTONIC 0x8 /* List must be monotonic */
//#define MSR_S   0x10    /* Parameter must be saved by clients */
#define MSR_G   0x20    /* Gruppenbezeichnung (unused) */
#define MSR_AW  0x40    /* Writeable by admin only */
#define MSR_P   0x80    /* Persistant parameter, written to disk */
#define MSR_DEP 0x100   /* This parameter is an exerpt of another parameter.
                           Writing to this parameter also causes an update
                           notice for the encompassing parameter */
#define MSR_AIC 0x200   /* Asynchronous input channel */

/* ein paar Kombinationen */
#define MSR_RWS (MSR_R | MSR_W | MSR_S)
#define MSR_RP (MSR_R | MSR_P)


using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
Parameter::Parameter(const PdServ::Parameter *p, size_t index,
                const PdServ::DataType& dtype,
                const PdServ::DataType::DimType& dim,
                size_t offset, Parameter *parent):
    Variable(p, index, dtype, dim, offset),
    mainParam(p),
    persistent(false),
    dependent(parent)
{
    if (parent) {
        parent->addChild(this);
        hidden = parent->hidden;
        persistent = parent->persistent;
    }
}

/////////////////////////////////////////////////////////////////////////////
bool Parameter::inform(Session* session, size_t begin, size_t end) const
{
    if (begin >= offset + memSize)
        return false;
    else if (offset >= end)
        return true;

    session->parameterChanged(this);
    for (List::const_iterator it = children.begin();
            it != children.end(); ++it) {
        if ((*it)->inform(session, begin, end))
            break;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
void Parameter::addChild(const Parameter* child)
{
    children.push_back(child);
}

/////////////////////////////////////////////////////////////////////////////
void Parameter::setXmlAttributes(XmlElement &element, const char *valueBuf,
        struct timespec const& mtime, bool shortReply, bool hex,
        std::streamsize precision) const
{
    unsigned int flags = MSR_R | MSR_W | MSR_WOP;

    // <parameter name="/lan/Control/EPC/EnableMotor/Value/2"
    //            index="30" value="0"/>

    setAttributes(element, shortReply);

    if (!shortReply) {
        XmlElement::Attribute(element, "flags")
            << flags + (dependent ? 0x100 : 0);

        // persistent=
        if (persistent)
            XmlElement::Attribute(element, "persistent") << 1;
    }

    // mtime=
    XmlElement::Attribute(element, "mtime") << mtime;

    if (valueBuf) {
        if (hex)
            XmlElement::Attribute(element, "hexvalue")
                .hexDec(valueBuf + offset, memSize);
        else
            XmlElement::Attribute(element, "value")
                .csv(this, valueBuf + offset, 1, precision);
    }

    if (!shortReply)
        addCompoundFields(element, variable->dtype);

    return;
}

/////////////////////////////////////////////////////////////////////////////
int Parameter::setHexValue(const Session *session,
        const char *s, size_t startindex) const
{
    char valueBuf[memSize];
    const char *valueEnd = valueBuf + memSize;
    static const char hexNum[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
        0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,10,11,12,13,14,15
    };

    char *c;
    for (c = valueBuf; *s and c < valueEnd; c++) {
        unsigned char c1 = *s++ - '0';
        unsigned char c2 = *s++ - '0';
        if (std::max(c1,c2) >= sizeof(hexNum))
            return -EINVAL;
        *c = hexNum[c1] << 4 | hexNum[c2];
    }
    // FIXME: actually the setting operation must also check for
    // endianness!

    return mainParam->setValue( session, valueBuf,
            offset + startindex * dtype.size, c - valueBuf);
}

/////////////////////////////////////////////////////////////////////////////
int Parameter::setElements(std::istream& is,
        const PdServ::DataType& dtype, const PdServ::DataType::DimType& dim,
        char*& buf, size_t& count) const
{
    if (dtype.primary() == dtype.compound_T) {
        const PdServ::DataType::FieldList& fieldList = dtype.getFieldList();
        PdServ::DataType::FieldList::const_iterator it;
        for (size_t i = 0; i < dim.nelem; ++i) {
            for (it = fieldList.begin(); it != fieldList.end(); ++it) {
                int rv = setElements(is, (*it)->type, (*it)->dim, buf, count);
                if (rv)
                    return rv;
            }
        }
        return 0;
    }

    double v;
    char c;
    for (size_t i = 0; i < dim.nelem; ++i) {

        is >> v;
        if (!is)
            return -EINVAL;

        dtype.setValue(buf, v);
        count += dtype.size;

        is >> c;
        if (!is)
            return 1;

        if (c != ',' and c != ';' and !isspace(c))
            return -EINVAL;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int Parameter::setDoubleValue(const Session *session,
        const char *buf, size_t startindex) const
{
    char valueBuf[memSize];
    char *dataBuf = valueBuf;

    std::istringstream is(buf);
    is.imbue(std::locale::classic());

    //log_debug("buf=%s", buf);
    size_t count = 0;
    int rv = setElements(is, dtype, dim, dataBuf, count);

//    log_debug("rv=%i startindex=%zu offset=%zu count=%zu",
//            rv, startindex, offset/dtype.size + startindex, count);
//    for (size_t i = 0; i < count; i++)
//        std::cerr << ((const double*)valueBuf)[i] << ' ';
//    std::cerr << std::endl;
    return rv < 0
        ? rv
        : mainParam->setValue( session, valueBuf,
                offset + startindex * dtype.size, count);
}

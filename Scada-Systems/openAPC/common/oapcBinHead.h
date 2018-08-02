/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources can be used freely according to the OpenAPC Dual License: As long as the
sources and the resulting applications/libraries/Plug-Ins are used together with the OpenAPC
software, they are licensed as freeware. When you use them outside the OpenAPC software they
are licensed under the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/

#ifndef OAPC_BIN_HEAD_H
#define OAPC_BIN_HEAD_H

#include "liboapc.h"

class oapcBinHead
{
public:
    oapcBinHead(unsigned char type,unsigned char subType,unsigned char compression,int sizeData);
    oapcBinHead(struct oapc_bin_head *bin);
    ~oapcBinHead();

    struct oapc_bin_head *m_bin;
    int                   m_refCnt;
};

class oapcBinHeadSp
{
public:
    oapcBinHeadSp();
    oapcBinHeadSp(oapcBinHead *bin);
    oapcBinHeadSp(const oapcBinHeadSp &binSp);
    virtual ~oapcBinHeadSp();
    void reset();

    operator bool();
    oapcBinHead* operator->();
    oapcBinHeadSp operator=(const oapcBinHeadSp &rhs);

private:
    oapcBinHead *m_bin;
};

#endif

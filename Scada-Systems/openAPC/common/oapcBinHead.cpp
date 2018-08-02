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

#include "oapcBinHead.h"

#include <string.h>
#include <assert.h>

/**
This is a shared pointer implementation for binary data of type struct oapc_bin_head It encapsulates and
stores pointers to this data structure, checks how many instances are currently using it and releases
the data structures memory area as soon as the last instance drops it. So this is some kind of
auto-clean-up for this structure.
Please note :to let this mechanism work and to avoid strange crashes it is mandatory to use the structure
ONLY via objects of type oapcBinHead and never store and use any raw pointers to it!
*/


/**
 * Creates a new and empty oapcBinHead object that encapsulates a struct oapc_bin_head of the given type
 * and size
 * @param[in] type the type of the structure to be created
 * @param[in] subType the subtype that is assigned to the type of the structure to be created
 * @param[in] compression used data compression mechanism
 * @param[in] sizeData size of the attached data in bytes
 */
oapcBinHead::oapcBinHead(unsigned char type,unsigned char subType,unsigned char compression,int sizeData)
{
   m_bin=oapc_util_alloc_bin_data(type,subType,compression,sizeData);
   m_refCnt=0;
}


/**
 * Creates a new oapcBinHead object using the given input data
 * @param[in] bin the binary structure to create this object from, this structure is copied and
 *            remains active and valid after this operation, if necessary it has to be released
 *            using oapc_util_release_bin_data() explictly!
 */
oapcBinHead::oapcBinHead(struct oapc_bin_head *bin)
{
   assert(bin->sizeData+sizeof(struct oapc_bin_head)>0);
   m_bin=oapc_util_alloc_bin_data(0,0,0,bin->sizeData);
   assert(bin);
   if (m_bin) memcpy(m_bin,bin,sizeof(struct oapc_bin_head)+bin->sizeData);
   m_refCnt=0;
}

oapcBinHead::~oapcBinHead()
{
   if (m_bin) oapc_util_release_bin_data(m_bin);
}

// === Shared pointer implementation =========================================================================================


oapcBinHeadSp::oapcBinHeadSp()
{
    m_bin=NULL;
}


oapcBinHeadSp::oapcBinHeadSp(oapcBinHead *bin)
{
    bin->m_refCnt++;
    m_bin=bin;
}


oapcBinHeadSp::oapcBinHeadSp(const oapcBinHeadSp &binSp)
{    
   m_bin=binSp.m_bin;
   if (m_bin) m_bin->m_refCnt++;
}


oapcBinHeadSp::~oapcBinHeadSp()
{
   reset();
}


void oapcBinHeadSp::reset()
{
   if (m_bin)
   {
      m_bin->m_refCnt--;
      assert(m_bin->m_refCnt>=0);
      if (m_bin->m_refCnt<=0)
       delete m_bin;
      m_bin=NULL;
   }
}


oapcBinHeadSp::operator bool()
{
   return (m_bin!=NULL);
}


oapcBinHead* oapcBinHeadSp::operator->()
{
   return m_bin;
}


oapcBinHeadSp oapcBinHeadSp::operator=(const oapcBinHeadSp &rhs)
{
   if (this == &rhs) return *this;   
   m_bin=rhs.m_bin;
   if (m_bin) m_bin->m_refCnt++;
   return *this;
}

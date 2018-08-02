/******************************************************************************

This file is part of E1803inter shared library

E1803inter is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

E1803inter is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
E1803inter. If not, see <http://www.gnu.org/licenses/>.

E1803inter linking exception: Linking this library dynamically with other
modules is making a combined work based on this library. Thus, the terms and
conditions of the GNU General Public License cover the whole combination. As a
special exception, the copyright holders of this library give you permission
to link this library with independent modules to produce an executable,
regardless of the license terms of these independent modules, and to copy and
distribute the resulting executable under terms of your choice, provided that
you also meet, for each linked independent module, the terms and conditions of
the license of that module. An independent module is a module which is not
derived from or based on this library. If you modify this library, you may
extend this exception to your version of the library, but you are not
obliged to do so. If you do not wish to do so, delete this exception statement
from your version.

*******************************************************************************/

#include <stddef.h>

#include "libe1803.h"
#include "libe1803inter.h"

static struct point_xy_rgbia m_lastPoint[E1803_MAX_HEAD_NUM+1]={{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
       char                  m_isOpened[E1803_MAX_HEAD_NUM+1]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


bool checkOpen(unsigned char card)
{
   if (m_isOpened[card]) return false;
   if (E1803_load_correction(card,NULL,0)==E1803_OK)
   {
      E1803_tune(card,E1803_TUNE_10V_ANALOGUE_XYZ);
      return true;
   }
   return false;
}


E1803_API int  EzAudDacGetCardNum(void)
{
   return E1803_MAX_HEAD_NUM;
}

E1803_API bool EzAudDacWriteFrame(const int *card,const struct point_xy_rgbia *data,int size,unsigned short pps)
{
   return EzAudDacWriteFrameNR(card,data,size,pps,0);
}

E1803_API bool EzAudDacWriteFrameNR(const int *card,const struct point_xy_rgbia *data,int size,unsigned short,unsigned short rep)
{
   int i,j,count;

   if (!checkOpen((unsigned char)*card)) return false;
   count=size/sizeof(struct point_xy_rgbia);
   for (i=0; i<rep; i++) // repeats
    for (j=0; j<count; j++)
   {
      E1803_set_pos((unsigned char)*card,data[j].x<<10,data[j].y<<10,0,1);
      if ((data[j].r!=m_lastPoint[*card].r) || (data[j].g!=m_lastPoint[*card].g) ||
          (data[j].b!=m_lastPoint[*card].b))
      {
         E1803_ana_write((unsigned char)*card,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_ANA_AOUT0,data[j].r);
         E1803_ana_write((unsigned char)*card,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_ANA_AOUT1,data[j].g);
         //E1803_ana_write((unsigned char)*card,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_ANA_A2,data[j].b);
         m_lastPoint[*card]=data[j];
      }
   }
   return false;
}

E1803_API int  EzAudDacGetStatus(const int *card)
{
   if (!checkOpen((unsigned char)*card)) return 0;
   if (E1803_get_card_state((unsigned char)*card) & E1803_CSTATE_MARKING) return GET_STATUS_BUSY;
   return GET_STATUS_READY;
}

E1803_API bool EzAudDacStop(const int *card)
{
   if (!checkOpen((unsigned char)*card)) return false;
   return (E1803_stop_execution((unsigned char)*card)==E1803_OK);
}

E1803_API bool EzAudDacClose(void)
{
   unsigned char i;

   for (i=1; i<=E1803_MAX_HEAD_NUM; i++)
   {
      m_isOpened[i]=0;
      E1803_close(i);
   }
   return true;
}

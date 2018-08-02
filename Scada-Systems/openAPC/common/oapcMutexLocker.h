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

#ifndef OAPC_MUTEX_LOCKER
#define OAPC_MUTEX_LOCKER

class oapcMutexLocker
{
public:
   oapcMutexLocker(void *mutex,const char *file,int line);
   virtual ~oapcMutexLocker();

private:
   const void *m_mutex;
#ifndef NDEBUG
   char        m_file[300];
   int         m_line;
#endif
   
};


#endif // PTHREAD_MUTEX_LOCKER

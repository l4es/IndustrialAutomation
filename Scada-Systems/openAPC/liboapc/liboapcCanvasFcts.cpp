/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4127)
#endif


#include <string.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <wx/wx.h>

#include "liboapc.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifndef _UNICODE
  #define _UNICODE
 #endif
#endif



/**
 * Release the internal data strucutres of a canvas (but not the canvas itself);
 * this function must not be called by an HMI plug-in!
 */
OAPC_EXT_API void oapc_canvas_release_data(wxPanel *canvas)
{
   struct oapc_canvas_userdata *userdata;

   assert(canvas);
   if (!canvas) return;
   userdata=(struct oapc_canvas_userdata*)canvas->GetClientData();
   if (userdata)
   {
      canvas->SetClientData(NULL);
      free(userdata);
   }
}



/**
 * Get the read-only flag from a wxPanel that is used as canvas for an HMI plug-in
 * @param[in] canvas the canvas that is handed over from the main application to the
 *            plug-in
 * @return true when the canvas is read-only, false otherwise
 */
OAPC_EXT_API bool oapc_canvas_get_readonly(wxPanel *canvas)
{
   struct oapc_canvas_userdata *userdata;

   assert(canvas);
   if (!canvas) return true;
   userdata=(struct oapc_canvas_userdata*)canvas->GetClientData();
   if (userdata->identifier!=OAPC_CANVAS_IDENTIFIER) return true;
   return ((userdata->flags & OAPC_CANVAS_READONLY)==OAPC_CANVAS_READONLY);
}



/**
 * Sets a canvas read-only
 * @param[in] canvas the canvas that is handed over from the main application to the
 *            plug-in
 * @param[in] readonly the new read-only state, set this parameter to true when the canvas
 *            has to be set to read-only, set it to false otherwise
 */
OAPC_EXT_API void oapc_canvas_set_readonly(wxPanel *canvas,bool readonly)
{
   struct oapc_canvas_userdata *userdata;

   assert(canvas);
   if (!canvas) return;

   userdata=(struct oapc_canvas_userdata*)canvas->GetClientData();
   if (!userdata)
   {
      userdata=(struct oapc_canvas_userdata*)malloc(sizeof(struct oapc_canvas_userdata));
      assert(userdata);
      if (!userdata) return;
      userdata->identifier=OAPC_CANVAS_IDENTIFIER;
      userdata->flags=OAPC_CANVAS_ENABLED;
      canvas->SetClientData(userdata);
   }
   if (readonly) userdata->flags|=OAPC_CANVAS_READONLY;
   else userdata->flags&=~OAPC_CANVAS_READONLY;
}



/**
 * Get the enabled flag from a wxPanel that is used as canvas for an HMI plug-in
 * @param[in] canvas the canvas that is handed over from the main application to the
 *            plug-in
 * @return true when the canvas is enabled, false otherwise
 */
OAPC_EXT_API bool oapc_canvas_get_enabled(wxPanel *canvas)
{
   assert(canvas);
   if (!canvas) return true;
   return canvas->IsEnabled();
}



/**
 * Sets a canvas enabled or disabled
 * @param[in] canvas the canvas that is handed over from the main application to the
 *            plug-in
 * @param[in] enable the new enable state, set this parameter to true when the canvas
 *            has to be enabled, set it to false otherwise
 */
OAPC_EXT_API void oapc_canvas_set_enabled(wxPanel *canvas,bool enable)
{
   assert(canvas);
   if (!canvas) return;
   canvas->Enable(enable);
}

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

#include <wx/filename.h>

#include "oapc_libio.h"
#include "liboapc.h"


/**
 * Splits a given path name into its directory and file name part and optionally adds an extension
 * in case the file name does not already owns it
 * @param[in] path pointer to the variable that holds the full path
 * @param[out] dir pointer to a variable to store the directory part of the full path into
 * @param[out] file pointer to a variable to store the file name into
 * @param[in] extension if a non-empty string is given here the extracted file is checked for
 *            this extension. If it does not exist, the extension is added to the files name
 */
OAPC_EXT_API void oapc_path_split(wxString *path,wxString *dir,wxString *file,wxString extension)
{
   int separatorPos;

   if (!extension.IsEmpty())
   {
      if (path->Mid(path->Len()-extension.Length()).CompareTo(extension)) *path=*path+extension;
   }
   separatorPos=path->Find(wxFileName::GetPathSeparator(),true);
   if (separatorPos==wxNOT_FOUND)
   {
      *dir=wxEmptyString;
      *file=*path;
   }
   else
   {
      *dir=path->Mid(0,separatorPos);
      *file=path->Mid(separatorPos);
   }
}



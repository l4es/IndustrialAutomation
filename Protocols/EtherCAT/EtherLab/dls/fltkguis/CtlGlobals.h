/******************************************************************************
 *
 *  $Id$
 *
 *  This file is part of the Data Logging Service (DLS).
 *
 *  DLS is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef CtlGlobalsH
#define CtlGlobalsH

/*****************************************************************************/

#define WATCH_ALERT 3

#define META_MASK_FIXED true
#define META_REDUCTION_FIXED true

/*****************************************************************************/

class CtlDialogMsg;

extern CtlDialogMsg *msg_win;

/*****************************************************************************/

// Versions-String mit Build-Nummer aus ctl_build.cpp
extern const char *ctl_version_str;

/*****************************************************************************/

/**
   Nachricht des DLSD mit Zeitpunkt, Typ und Text
*/

struct CtlMessage
{
    string time;
    int type;
    string text;
};

/*****************************************************************************/

#endif

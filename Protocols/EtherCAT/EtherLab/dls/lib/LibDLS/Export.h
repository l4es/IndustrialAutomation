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

#ifndef LibDLSExportH
#define LibDLSExportH

/*****************************************************************************/

#include <fstream>

#include "Exception.h"
#include "Channel.h"

/*****************************************************************************/

namespace LibDLS
{
	class File;

    /*************************************************************************/

    class ExportException:
        public Exception
    {
    public:
        ExportException(const std::string &pmsg): Exception(pmsg) {};
    };

    /*************************************************************************/

    typedef struct
    {
        long type;
        long mrows;
        long ncols;
        long imagf;
        long namelen;
    }
    Mat4Header;

    /*************************************************************************/

    class Export
    {
    public:
        Export();
        virtual ~Export();

        virtual void begin(const Channel &, const std::string &,
                const std::string & = std::string()) = 0;
        virtual void data(const Data *) = 0;
        virtual void end() = 0;
    };

    /*************************************************************************/

    class ExportAscii:
        public Export
    {
    public:
        ExportAscii();
        ~ExportAscii();

        void begin(const Channel &, const std::string &,
                const std::string & = std::string());
        void data(const Data *);
        void end();

    private:
        std::ofstream _file;
    };

    /*************************************************************************/

    class ExportMat4:
        public Export
    {
    public:
        ExportMat4();
        ~ExportMat4();

        void begin(const Channel &, const std::string &,
                const std::string & = std::string());
        void data(const Data *);
        void end();

    private:
        Mat4Header _header;
        File *_file;
    };
}

/*****************************************************************************/

#endif

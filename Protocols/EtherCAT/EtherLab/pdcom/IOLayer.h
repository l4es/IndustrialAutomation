/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 * @file
 *
 *****************************************************************************/


#ifndef PDCOM_IOLAYER_H
#define PDCOM_IOLAYER_H

#include <stddef.h>

/** Base data transport class
 *
 * Communication within PdCom is organized in layers, topmost layer being
 * the protocol handler and the lowest the socket layer.
 *
 * Data is collected from lower layers by issuing a read() call on them.
 * Data is sent to lower layers by calling write() method.
 * 
 * Output data is expected to be buffered only at the lowest layer. The
 * protocol handler regularly calls flush() to tell that layer to flush
 * the buffer.
 */
class IOLayer {
    public:
        IOLayer(IOLayer* parent);
        virtual ~IOLayer();

        size_t rxbytes;     /**< Received bytes */
        size_t txbytes;     /**< Transmitted bytes */

        void setEOF();

        /** Read data from a lower layer
         *
         * Reimplement this method to customize the read operation. The
         * default implementation calls IOLayer::read() on the lower
         * layer.
         *
         * @param buf data destination
         * @param count buffer size
         *
         * \return Number of bytes read, negative on error
         */
        virtual int  read(       char *buf, size_t count);

        /** Write data to a lower layer
         *
         * Write \p count bytes from \p buf to lower layer
         *
         * Reimplement this method to customize the write operation. The
         * default implementation calls IOLayer::write() on the
         * lower layer.
         *
         * All data must be written; short writes are not expected.
         *
         * @param buf data source
         * @param count buffer size
         */
        virtual void write(const char *buf, size_t count);

        /** Flush data in output buffer */
        virtual void flush();

    protected:
        void insert(IOLayer* io);

    private:
        IOLayer* m_parent;
};

#endif // PDCOM_PROTOCOLLAYER_H

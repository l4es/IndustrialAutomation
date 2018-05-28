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

#ifndef SaverMetaTH
#define SaverMetaTH

/*****************************************************************************/

#include "lib/LibDLS/Time.h"

#include "globals.h"
#include "SaverT.h"

/*****************************************************************************/

/**
   Saver-Objekt für Meta-Daten
*/

template <class T>
class SaverMetaT: public SaverT<T>
{
    using SaverT<T>::_block_buf;
    using SaverT<T>::_block_buf_index;
    using SaverT<T>::_block_buf_size;
    using SaverT<T>::_block_time;
    using SaverT<T>::_meta_buf;
    using SaverT<T>::_meta_buf_index;
    using SaverT<T>::_meta_buf_size;
    using SaverT<T>::_meta_time;
    using SaverT<T>::_time_of_last;
    using SaverT<T>::_parent_logger;
    using SaverT<T>::_compression;
    using SaverT<T>::_save_rest;
    using SaverT<T>::_finish_files;
    using SaverT<T>::_save_block;

public:
    SaverMetaT(Logger *, LibDLS::MetaType, unsigned int);
    virtual ~SaverMetaT();

    void generate_meta_data(LibDLS::Time, LibDLS::Time, unsigned int,
            const T *);
    void flush();

private:
    SaverMetaT<T> *_next_saver; /**< Zeiger auf das Saver-Objekt
                                      der nächsten Ebene */
    LibDLS::MetaType _type;             /**< Typ dieses Saver-Objektes */
    bool _finished;                /**< true, wenn keine Daten mahr im
                                      Speicher */
    unsigned int _level;           /**< Meta-Ebene dieses Saver-Objektes */

    void _pass_meta_data();
    T _meta_value(const T *, unsigned int);
    int _meta_level() const;
    string _meta_type() const;
};

/*****************************************************************************/

template <class T>
SaverMetaT<T>::SaverMetaT(Logger *parent_logger,
                                LibDLS::MetaType type,
                                unsigned int level)
    : SaverT<T>(parent_logger)
{
    _next_saver = (SaverMetaT<T> *) 0;
    _type = type;
    _finished = true;
    _level = level;
}

/*****************************************************************************/

template <class T>
SaverMetaT<T>::~SaverMetaT()
{
#if 0
    if (!_finished)
    {
        msg() << "Meta-Saver Level " << _level << ": not finished!";
        log(Warning);
    }
#endif

    // Nächsten MetaSaver freigeben
    if (_next_saver) delete _next_saver;
}

/*****************************************************************************/

/**
   Generiert einen Meta-Wert aus den gegebenen Daten

   \param start_time Zeit des ersten Wertes im Quellpuffer
   \param end_time Zeit des letzten Wertes im Quellpuffer
   \param length Anzahl der Werte im Quellpuffer
   \param buffer Quellpuffer
*/

template <class T>
void SaverMetaT<T>::generate_meta_data(LibDLS::Time start_time,
                                          LibDLS::Time end_time,
                                          unsigned int length,
                                          const T *buffer)
{
    if (length == 0) return;

    // Ab jetzt sind Daten im Speicher
    _finished = false;

    // Meta-Wert erzeugen
    T meta_value = _meta_value(buffer, length);

    // Zeit der Anfänge der ersten Werte in den Puffern vermerken
    if (_block_buf_index == 0) _block_time = start_time;
    if (_meta_buf_index == 0) _meta_time = start_time;

    // Zeit des Endwertes im letzten Metawert vermerken
    _time_of_last = end_time;

    // Wert in die Puffer übernehmen
    _block_buf[_block_buf_index++] = meta_value;
    _meta_buf[_meta_buf_index++] = meta_value;

    // Block-Puffer voll?
    if (_block_buf_index == _block_buf_size) _save_block();

    // Meta-Puffer voll?
    if (_meta_buf_index == _meta_buf_size) _pass_meta_data();

    if (_block_buf_index == 0 && _meta_buf_index == 0) _finished = true;
}

/*****************************************************************************/

template <class T>
T SaverMetaT<T>::_meta_value(const T *buffer, unsigned int length)
{
    T meta_val = 0;

    if (_type == LibDLS::MetaMean)
    {
        double sum = 0;
        for (unsigned int i = 0; i < length; i++)
        {
            sum += buffer[i];
        }
        meta_val = (T) (sum / length);
    }

    else if (_type == LibDLS::MetaMin)
    {
        T min = buffer[0];
        for (unsigned int i = 1; i < length; i++)
        {
            if (buffer[i] < min) min = buffer[i];
        }
        meta_val = min;
    }

    else if (_type == LibDLS::MetaMax)
    {
        T max = buffer[0];
        for (unsigned int i = 1; i < length; i++)
        {
            if (buffer[i] > max) max = buffer[i];
        }
        meta_val = max;
    }

    return meta_val;
}

/*****************************************************************************/

template <class T>
void SaverMetaT<T>::_pass_meta_data()
{
    // Wenn noch kein nächster Saver existiert - erzeugen!
    if (!_next_saver)
    {
        _next_saver = new SaverMetaT<T>(_parent_logger,
                                           _type,
                                           _level + 1);
    }

    // Daten an nächsten Saver weiterreichen
    _next_saver->generate_meta_data(_meta_time, _time_of_last,
                                    _meta_buf_index, _meta_buf);

    _meta_buf_index = 0;
}

/*****************************************************************************/

template <class T>
void SaverMetaT<T>::flush()
{
    // Zuerst Blockdaten speichern
    _save_block();

    // Eventuell restliche Daten des Kompressionsobjektes speichern
    _save_rest();

    // Dateien Beenden
    _finish_files();

    // Persistenten Speicher des Kompressionsobjekt leeren
    _compression->clear();

    // Puffer sind jetzt leer
    _meta_buf_index = 0;
    _finished = true;

    // Kinder sollen auch alle flush() aufrufen
    if (_next_saver) _next_saver->flush();
}

/*****************************************************************************/

template <class T>
inline int SaverMetaT<T>::_meta_level() const
{
    return _level;
}

/*****************************************************************************/

template <class T>
string SaverMetaT<T>::_meta_type() const
{
    switch (_type)
    {
        case LibDLS::MetaMean: return "mean";
        case LibDLS::MetaMin: return "min";
        case LibDLS::MetaMax: return "max";
        default: return "undef";
    }
}

/*****************************************************************************/

#endif

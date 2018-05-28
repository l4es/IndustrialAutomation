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

#ifndef SaverGenTH
#define SaverGenTH

/*****************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <list>
#include <string>
using namespace std;

/*****************************************************************************/

#include "SaverT.h"
#include "SaverMetaT.h"

/*****************************************************************************/

//#define DEBUG

/*****************************************************************************/

/**
   Abstrakte Basisklasse von SaverGenT

   Da SaverGenT eine Template-Klasse ist und nicht mehrere
   unterschiedliche Instanzen dieser Template-Klasse in einer
   Liste verwaltet werden können, wird stattdessen eine Liste
   mit Zeigern auf diese abstrakte Basisklasse verwendet,
   die bereits alle wichtigen Methoden kennt.
*/

class SaverGen
{
public:
    SaverGen() {};
    virtual ~SaverGen() {};

    /**
       Fügt einen Meta-Saver für einen bestimmten Meta-Typ hinzu

       \param type Meta-Typ
    */

    virtual void add_meta_saver(LibDLS::MetaType type) = 0;

    /**
       Schreibt alle wartenden Daten ins Dateisystem
    */

    virtual void flush() = 0;

    /**
       Nimmt einen Puffer voller Binärdaten entgegen

       \param buffer Adresse des Puffers mit den zu speichernden Daten
       \param length Anzahl der Datenwerte
       \param time_of_last Zeit des letzten Datenwertes
       \throw ESaver Fehler beim Verarbeiten der Daten
       \throw ETimeTolerance Zeit-Toleranzfehler!
    */
    virtual void process_one(const void *buffer,
            LibDLS::Time time_of_last) = 0;
};

/*****************************************************************************/

/**
   Speichern von generischen Daten

   Nimmt generische Daten entgegen und speichert diese. Hält die
   Meta-Saver vor und verteilt anfallende Daten an sie.
*/

template <class T>
class SaverGenT:
    public SaverGen,
    public SaverT<T>
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
    SaverGenT(Logger *);
    virtual ~SaverGenT();

    void add_meta_saver(LibDLS::MetaType type);
    void process_one(const void *, LibDLS::Time);
    void flush();

private:
    list<LibDLS::MetaType> _meta_types; /**< Liste der zu erfassenden Meta-Typen */
    list<SaverMetaT<T> *> _meta_savers; /**< Liste der aktiven Meta-Saver */
    bool _savers_created; /**< Wurden bereits alle Meta-Saver erstellt? */
    bool _finished; /**< true, wenn keine Daten mehr im Speicher */
    uint64_t _processed_values; /**< Processed values since start. */

    void _fill_buffers(const T *, unsigned int, LibDLS::Time);

    //@{
    void _create_savers();
    void _generate_meta_data();
    void _flush_savers();
    void _clear_savers();
    //@}

    int _meta_level() const;
    string _meta_type() const;

    SaverGenT(); // Default-Konstruktor darf nicht aufgerufen werden!
};

/*****************************************************************************/

/**
   Konstruktor

   \param parent_logger Zeiger auf das besitzende Logger-Objekt
*/

template <class T>
SaverGenT<T>::SaverGenT(
        Logger *parent_logger
        ):
    SaverT<T>(parent_logger),
    _savers_created(false),
    _finished(true),
    _processed_values(0)
{
}

/*****************************************************************************/

/**
   Destruktor

   Gibt eine Warnung aus, wenn noch Daten im Speicher waren
*/

template <class T>
SaverGenT<T>::~SaverGenT()
{
#if 0
    if (!_finished)
    {
        msg() << "saver_gen not finished!";
        log(Warning);
    }
#endif

    // Saver löschen
    _clear_savers();
}

/*****************************************************************************/

/**
   Teilt dem Saver mit, dass Meta-Daten eines
   bestimmten Typs angelegt werden sollen.

   Fügt der Liste der zu erzeugenden Meta-Typen
   einen Typen hinzu. Die entsprechenden
   Meta-Saver werden bei Bedarf in der Methode
   _generate_meta_data() erstellt.

   \param type Typ der zu erstellenden Meta-Daten
*/

template <class T>
void SaverGenT<T>::add_meta_saver(LibDLS::MetaType type)
{
    _meta_types.push_back(type);
    _savers_created = false;
}

/*****************************************************************************/

/** Process one value.
 *
   Diese Methode führt Plausiblitätsprüfungen (vergangene Zeit seit dem
   letzten Datenwert) durch, um die Daten schliesslich als Array vom Typ T an
   die Methode _fill_buffers() weiterzuleiten.

   Wird der zeitliche Toleranzbereich verletzt, wird eine Exception geworfen.
   Der Prozess sollte dann beendet werden.

   \param buffer Adresse des Datenpuffers
   \param time Zeit des Datenwertes im Puffer
   \throw ESaver Fehler beim Speichern der Daten
   \throw ETimeTolerance Toleranzfehler! Prozess beenden!
*/

template <class T>
void SaverGenT<T>::process_one(
        const void *buffer,
        LibDLS::Time time
        )
{
    LibDLS::Time actual_diff, target_diff;
    float error_percent;
    stringstream err;

#if 0
    cerr << time.to_str()
        << " d=" << _time_of_last.diff_str_to(time)
        << " v=" << ((T *) buffer)[0] << endl;
#endif

    // Wenn Werte in den Puffern sind
    if (_block_buf_index || _meta_buf_index) {
        // Zeitabstände errechnen
        double freq = _parent_logger->channel_preset()->sample_frequency;
        target_diff.from_dbl_time(1 / freq); // Erwarteter Zeitabstand
        actual_diff = time - _time_of_last; // Tats. Zeitabstand

        // Relativen Fehler errechnen
        error_percent = (actual_diff.to_dbl() - target_diff.to_dbl())
            / target_diff.to_dbl() * 100.0;
        if (error_percent < 0.0) {
            error_percent *= -1.0;
        }

        // Toleranzbereich verletzt?
        if (error_percent > ALLOWED_TIME_VARIANCE) {
            // Fehler! Prozess beenden!
            err << "Time diff of " << actual_diff;
            err << " us (expected " << target_diff
                << " us, error is " << error_percent << " %)";
            err << " at channel \"" << _parent_logger->channel_preset()->name
                << "\" after processing " << _processed_values << " values.";
            throw ETimeTolerance(err.str());
        }
    }

    // Daten speichern
    _fill_buffers((T *) buffer, 1, time);
    _processed_values++;
}

/*****************************************************************************/

/**
   Speichern der Daten im Block- und Meta-Puffer

   Die Datenwerte werden einzeln in den Block-
   und Meta-Puffer geschoben. Läuft dabei der
   Block-Puffer voll, so wird ein kompletter Block
   in das Dateisystem geschrieben und der Puffer geleert.
   Läuft der Meta-Puffer voll, so werden alle
   Meta-Saver angewiesen, aus den vorhandenen Daten ihre
   Meta-Werte zu generieren. Danach wird der Puffer
   geleert.

   \param buffer Adresse des Datenpuffers
   \param length Anzahl der Datenwerte im Puffer
   \param time_of_first Zeit des ERSTEN Datenwertes im Puffer
*/

template <class T>
void SaverGenT<T>::_fill_buffers(const T *buffer,
                                    unsigned int length,
                                    LibDLS::Time time_of_first)
{
    LibDLS::Time time_of_one;
    double freq = _parent_logger->channel_preset()->sample_frequency;

    time_of_one.from_dbl_time(1 / freq); // Zeit eines Wertes

    // Ab jetzt sind Werte im Speicher!
    _finished = false;

    // Alle Werte übernehmen
    for (unsigned int i = 0; i < length; i++) {
        // Zeit des zuletzt eingefügten Wertes setzen
        _time_of_last = time_of_first + time_of_one * i;

        // Bei Blockanfang, Zeiten vermerken
        if (_block_buf_index == 0) {
            _block_time = _time_of_last;
        }
        if (_meta_buf_index == 0) {
            _meta_time = _time_of_last;
        }

        // Wert in die Puffer übernehmen
        _block_buf[_block_buf_index++] = buffer[i];
        _meta_buf[_meta_buf_index++] = buffer[i];

        // Block-Puffer voll?
        if (_block_buf_index == _block_buf_size) _save_block();

        // Meta-Puffer voll?
        if (_meta_buf_index == _meta_buf_size) _generate_meta_data();
    }
}

/*****************************************************************************/

/**
   Alle wartenden Daten in's Dateisystem schreiben

   Auch wenn noch nicht genug Daten für einen
   vollständigen Block im Block-Puffer sind, werden
   diese in's Dateisystem geschrieben und der Puffer
   anschließend geleert. Das gleiche passiert mit den wartenden
   Meta-Daten: Carries werden erstellt und die Meta-Puffer geleert.

   \throws ESaver Fehler beim Speichern
*/

template <class T>
void SaverGenT<T>::flush()
{
    // Blockdaten speichern
    _save_block();

    // Eventuell restliche Daten des Kompressionsobjektes speichern
    _save_rest();

#ifdef DEBUG
    msg() << "SaverGenT: _finish_files() for channel "
          << _parent_logger->channel_preset()->name;
    log(Debug);
#endif

    // Dateien beenden
    _finish_files();

#ifdef DEBUG
    msg() << "SaverGenT: _compression_clear()";
    log(Debug);
#endif

    // Persistenten Speicher des Kompressionsobjekt leeren
    _compression->clear();

#ifdef DEBUG
    msg() << "SaverGenT: _flush_savers()";
    log(Debug);
#endif

    // Metadaten speichern
    _flush_savers();

#ifdef DEBUG
    msg() << "SaverGenT: _clear_savers()";
    log(Debug);
#endif

    // Alle Saver beenden
    _clear_savers();

#ifdef DEBUG
    msg() << "SaverGenT: flush finished!";
    log(Debug);
#endif

    // Jetzt ist nichts mehr im Speicher
    _finished = true;
    _processed_values = 0;
}

/*****************************************************************************/

/**
   Meta-Daten generieren

   Diese Methode wird aufgerufen, wenn der Meta-Puffer
   voll ist. Wenn noch keine Meta-Saver erstellt wurden,
   passiert dies nun (vorrausgesetzt es sind welche gewünscht).
   Alle Meta-Saver bekommen Gelegenheit, aus dem vollen
   Meta-Puffer ihre Meta-Werte zu generieren.
   Danach wird dieser geleert.

   \throw ESaver Fehler beim Speichern
*/

template <class T>
void SaverGenT<T>::_generate_meta_data()
{
    typename list<SaverMetaT<T> *>::iterator meta_i;

    // Wenn Meta-Saver noch nicht existieren - erzeugen
    if (!_savers_created) _create_savers();

    // Meta-Daten generieren
    meta_i = _meta_savers.begin();
    while (meta_i != _meta_savers.end())
    {
        (*meta_i)->generate_meta_data(_meta_time, _time_of_last,
                                      _meta_buf_index, _meta_buf);
        meta_i++;
    }

    _meta_buf_index = 0;
}

/*****************************************************************************/

/**
   Veranlasst alle Saver, ihre Daten zu schreiben
*/

template <class T>
void SaverGenT<T>::_flush_savers()
{
    typename list<SaverMetaT<T> *>::iterator meta_i;

    meta_i = _meta_savers.begin();
    while (meta_i != _meta_savers.end())
    {
        // Alle Daten im Speicher auf die Festplatte schreiben
        (*meta_i)->flush();
        meta_i++;
    }

    // Alle Saver haben die Restdaten verwertet. Meta-Puffer leeren.
    _meta_buf_index = 0;
}

/*****************************************************************************/

/**
   Meta-Saver aus den Vorgaben erzeugen

   Alle erzeugten Meta-Saver haben Meta-Level 1.

   \throw ESaver Saver konnte nicht erzeugt werden
*/

template <class T>
void SaverGenT<T>::_create_savers()
{
    list<LibDLS::MetaType>::iterator meta_i;
    SaverMetaT<T> *_new_saver;

    _clear_savers();

    meta_i = _meta_types.begin();
    while (meta_i != _meta_types.end())
    {
        try
        {
            _new_saver = new SaverMetaT<T>(_parent_logger, *meta_i, 1);
        }
        catch (ESaver &e)
        {
            throw ESaver(e);
        }

        _meta_savers.push_back(_new_saver);
        meta_i++;
    }

    _savers_created = true;
}

/*****************************************************************************/

/**
   Alle Meta-Saver entfernen

   Entfernt nacheinander alle Meta-Saver und deren Kinder
*/

template <class T>
void SaverGenT<T>::_clear_savers()
{
    typename list<SaverMetaT<T> *>::iterator meta_i;

    // Meta-Saver löschen
    meta_i = _meta_savers.begin();
    while (meta_i != _meta_savers.end())
    {
        delete *meta_i;
        meta_i++;
    }

    _meta_savers.clear();
    _savers_created = false;
}

/*****************************************************************************/

// Doku: Siehe dls_saver_t.h

template <class T>
inline int SaverGenT<T>::_meta_level() const
{
    return 0;
}

/*****************************************************************************/

// Doku: Siehe dls_saver_t.h

template <class T>
inline string SaverGenT<T>::_meta_type() const
{
    return "gen";
}

/*****************************************************************************/

#ifdef DEBUG
#undef DEBUG
#endif

#endif

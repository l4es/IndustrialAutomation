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

#ifndef LibDLSQuantTH
#define LibDLSQuantTH


/*****************************************************************************/

#include <stdlib.h> // abs()

//#define QUANT_DEBUG

// Nur für Debug
#ifdef QUANT_DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif

#include "LibDLS/globals.h"
#include "LibDLS/Exception.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

enum {
    MAX_QUANT_BYTES = 4 // Maximal 32 Bit für Quantisierung
};

/*****************************************************************************/

/**
   Exception eines Quant-Objektes
*/

class EQuant : public Exception
{
public:
    EQuant(const string &pmsg) : Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Integer-Quantisierung
*/

template <class T>
class QuantT
{
public:
    QuantT(double);
    ~QuantT();

    //@{
    void quantize(const T *, unsigned int);
    void dequantize(const char *, unsigned int, unsigned int);
    //@}

    //@{
    const char *quant_output() const;
    unsigned int quant_output_size() const;
    const T *dequant_output() const;
    unsigned int dequant_output_length() const;
    //@}

    void free();

private:
    //@{
    double _accuracy; /**< Genauigkeit */
    //@}

    //@{
    char *_quant_output; /**< Ausgabespeicher für die
                            quantisierten Daten */
    unsigned int _quant_output_size; /**< Anzahl Bytes im Ausgabespeicher */
    T *_dequant_output; /**< Ausgabespeicher Dequantisierung */
    unsigned int _dequant_output_length; /**< Anzahl der Werte im
                                            Ausgabespeicher IMDCT */
    //@}

    void _int_quant(const T *, unsigned int, unsigned char, int *, double *);
    unsigned int _store_quant(const int *, unsigned int,
                              unsigned char, char *);

    QuantT(); // Default-Konstruktor (soll nicht aufgerufen werden!)
};

/*****************************************************************************/

/**
   Konstruktor

   \param acc Angestrebte Genauigkeit
*/

template <class T>
QuantT<T>::QuantT(double acc)
{
    _accuracy = acc;
    _quant_output = 0;
    _dequant_output = 0;
}

/*****************************************************************************/

/**
   Destruktor

   Gibt alle reservierten Speicherbereiche frei.
*/

template <class T>
QuantT<T>::~QuantT()
{
    free();
}

/*****************************************************************************/

/**
   Gibt alle allozierten Speicherbereiche frei.
*/

template <class T>
void QuantT<T>::free()
{
    if (_quant_output)
    {
        // Den alten Ausgabepuffer freigeben
        delete [] _quant_output;
        _quant_output = 0;
    }

    if (_dequant_output)
    {
        // Den alten Ausgabepuffer freigeben
        delete [] _dequant_output;
        _dequant_output = 0;
    }
}

/*****************************************************************************/

/**
   Führt eine Quantisierung aus

   \param input Array mit zu quantisierenden Werten
   \param input_length Anzahl der Elemente in diesem Array
*/

template <class T>
void QuantT<T>::quantize(const T *input, unsigned int input_length)
{
    unsigned int i, data_size = 0;
    int *quant, offset;
    double max_error, error;
    double scale = 0.0;
    unsigned char bits, use_bits, start, end;


#ifdef QUANT_DEBUG
    msg() << "MDCT::quantize() len=" << input_length;
    log(DLSDebug);
#endif

    _quant_output_size = 0;

    if (!input_length) return; // Keine Daten

    free();

    try
    {
        quant = 0;
        quant = new int[input_length];

        // Speicher für die Ausgabe reservieren
        // Offset, Skalierungsfaktor, Bits, Daten
        _quant_output = new char[sizeof(T) + sizeof(T) + 1 +
                                 MAX_QUANT_BYTES * input_length];
    }
    catch (...)
    {
        if (quant) delete [] quant;

        throw EQuant("Could not allocate memory for buffers!");
    }

#ifdef QUANT_DEBUG
#if 1
    msg() << "Input buffer:";
    log(DLSDebug);
    for (i = 0; i < input_length; i++)
    {
        msg() << convert_to_bin(&input[i], 8, -8) << " "
              << setw(15) << input[i];
        log(DLSDebug);
    }
#endif
#endif

    // Start- und Endpunkt für Bisektion setzen
    start = 2;
    end = MDCT_MAX_BYTES * 8 - 1;

    // Noch keine verwendbare Bitanzahl gefunden
    use_bits = 0;

    while (start <= end)
    {
        bits = (end - start + 1) / 2 + start;

        // Koeffizienten quantisieren
        _int_quant(input, input_length, bits, quant, &scale);

        // Abweichung berechnen
        max_error = 0;
        for (i = 0; i < input_length; i++)
        {
            error = fabs(quant[i] * scale - input[i]);
            if (error > max_error) max_error = error;
        }

#ifdef QUANT_DEBUG
        msg() << "Quant with " << (int) bits
              << " bits. Maximal error: " << max_error;
        log(DLSDebug);
#endif

        if (max_error < _accuracy) // Fehler unter Toleranz
        {
            // Diese Anzahl Bits könnte zum Quantisieren genutzt werden
            use_bits = bits;

            // Probieren, ob weniger Bits genügen würden
            end = bits - 1;
        }
        else // Fehler zu groß!
        {
            // Mehr Bits verwenden
            start = bits + 1;
        }
    }

    if (!use_bits) // Der Fehler war immer zu groß
    {
        // Maximale Anzahl Bits zum Quantisieren verwenden
        use_bits = MDCT_MAX_BYTES * 8 - 1;

#ifdef QUANT_DEBUG
        // Warnung ausgeben!
        msg() << "MDCT - Could not reach maximal error of " << _accuracy;
        msg() << ". Quantizing with " << (int) bits
              << " bits. Error is " << max_error;
        log(DLSWarning);
#endif
    }

#ifdef QUANT_DEBUG
    msg() << "Using quant " << (int) use_bits;
    log(DLSDebug);
    for (i = 0; i < input_length; i++)
    {
        msg() << convert_to_bin(&quant[i], sizeof(int), -sizeof(int))
              << " " << setw(15) << quant[i];
        log(DLSDebug);
    }
#endif

    offset = quant[0];

    // Offset der Differentierung speichern
    *((T *) _quant_output) = (T) offset;
    data_size += sizeof(T);

    // Differentieren
    for (i = 0; i < input_length; i++)
    {
        quant[i] -= offset;
        offset += quant[i];
    }

    // Skalierungsfaktor speichern
    *((T *) (_quant_output + data_size)) = (T) scale;
    data_size += sizeof(T);

    // Anzahl Quantisierungsbits speichern
    *(_quant_output + data_size) = use_bits;
    data_size++;

    // Koeffizienten in den Ausgabepuffer kopieren
    data_size += _store_quant(quant, input_length, use_bits,
                              _quant_output + data_size);

    _quant_output_size = data_size;

#ifdef QUANT_DEBUG
    msg() << "Total packed (" << data_size << " bytes):";
    log(DLSDebug);
    msg() << convert_to_bin(_quant_output, data_size, 8);
    log(DLSDebug);
#endif

    delete [] quant;
}

/*****************************************************************************/

/**
   Absolute Quantisierung

   Nimmt eine absolute Quantisierung der double-Koeffizienten
   vor. D. h. es wird ein Raster über den gesamten Wertebereich
   gelegt und ähnliche Werte mit dem selben Integer-Wert
   repräsentiert.

   \param input        Array von Datenwerten
   \param input_length Anzahl von Datenwerten im Array
   \param bits         Anzahl Bits, mit denen quantisiert werden soll
   \param quant        Zeiger auf einen Speicherbereich zur Ablage
   der quantisieren Integer-Werte
   \param scale        Zeiger auf den Skalierungsfaktor (wird während
   der Verarbeitung beschrieben)
*/

template <class T>
void QuantT<T>::_int_quant(const T *input,
                              unsigned int input_length,
                              unsigned char bits,
                              int *quant,
                              double *scale)
{
    unsigned int i;
    double abs_value, max;

    // Division durch 0 verhindern
    if (bits < 2) return;

    // Maximum ermitteln
    max = 0.0;
    for (i = 0; i < input_length; i++)
    {
        abs_value = fabs(input[i]);
        if (abs_value > max) max = abs_value;
    }

    // Skalierung berechnen
    *scale = 2 * max / ((1 << bits) - 1);

    // Alle Koeffizienten skalieren
    for (i = 0; i < input_length; i++)
    {
        // Skalierte Koeffizienten im Integer-Array ablegen
        quant[i] = (int) round(input[i] / (*scale));
    }
}

/*****************************************************************************/

/**
   Gepacktes Speichern der quantisierten Werte

   \param quant  Array mit quantisierten Werten
   \param length Anzahl der quantisierten Werte
   \param bits   Azzahl Bits, die zur Quantisierung
   herangezogen wurden
   \param output Zeiger auf einen Speicherbereich zum
   Ablegen des Ergebnisses

   \return Größe des benötigten Speichers in Bytes
*/

template <class T>
unsigned int QuantT<T>::_store_quant(const int *quant,
                                        unsigned int length,
                                        unsigned char bits,
                                        char *output)
{
    unsigned int current_byte, bits_free, i;
    unsigned char b;

    current_byte = 0;
    bits_free = 8;
    output[current_byte] = 0;

    // Zuerst die Vorzeichenbits hintereinander an den Anfang schreiben
    for (i = 0; i < length; i++)
    {
        if (quant[i] < 0)
        {
            output[current_byte] |= 1 << (7 - (i % 8));
        }

        if (--bits_free == 0)
        {
            current_byte++;
            output[current_byte] = 0;
            bits_free = 8;
        }
    }

    // Nun die quantisierten Werte "transponiert" speichern
    for (b = bits; b > 0; b--)
    {
        for (i = 0; i < length; i++)
        {
            if (!bits_free)
            {
                current_byte++;
                output[current_byte] = 0;
                bits_free = 8;
            }

            if (abs(quant[i]) & (1 << (b - 1)))
            {
                output[current_byte] |= 1 << (bits_free - 1);
            }

            bits_free--;
        }
    }

    return current_byte + 1;
}

/*****************************************************************************/

/**
   Führt eine Dequantisierung aus


   \param input_length Anzahl der Werte im input-Array
*/

template <class T>
void QuantT<T>::dequantize(const char *input,
                              unsigned int input_size,
                              unsigned int length)
{
    unsigned int i, current_byte, current_bit;
    stringstream err;
    char *signs;
    int *quant, offset;
    double scale;
    unsigned char bits, b;

    current_byte = 0;

#ifdef QUANT_DEBUG
    msg() << "MDCT::detransform() size="<< input_size
          << " len=" << length;
    log(DLSDebug);
#endif

    _dequant_output_length = 0;

    if (input_size < 2 || length == 0) return; // Keine Daten

    free();

    signs = 0;
    try
    {
        _dequant_output = new T[length];
        signs = new char[length];
        quant = new int[length];
    }
    catch (...)
    {
        if (signs) delete [] signs;

        throw EQuant("Could not allocate memory for buffers!");
    }

    // Dequantisieren

    current_byte = 0;
    current_bit = 8;

    // Integer-Koeffizienten mit Nullen vorbelegen
    for (i = 0; i < length; i++) quant[i] = 0;

    // Integrations-Offset lesen
    offset = (int) *((T *) input);
    current_byte += sizeof(T);

    // Skalierungksfaktor aus den komprimierten Daten lesen
    scale = (double) *((T *) (input + current_byte));
    current_byte += sizeof(T);

    // Anzahl der Quantisierungsbits lesen
    bits = (unsigned char) *(input + current_byte);
    current_byte++;

#ifdef QUANT_DEBUG
    msg() << "DeQuant scale=" << scale << ", bits=" << (int) bits;
    log(DLSDebug);
#endif

    // Vorzeichenbits auslesen
    for (i = 0; i < length; i++)
    {
        if (*(input + current_byte) & (1 << (current_bit - 1))) signs[i] = -1;
        else signs[i] = 1;

        if (--current_bit == 0)
        {
            current_byte++;
            current_bit = 8;
        }
    }

#ifdef QUANT_DEBUG
    msg() << "Signs:";
    log(DLSDebug);
    for (i = 0; i < length; i++) msg() << (int) signs[i] << ", ";
    log(DLSDebug);
#endif

    for (b = bits; b > 0; b--)
    {
        for (i = 0; i < length; i++)
        {
            if (input[current_byte] & (1 << (current_bit - 1)))
                quant[i] |= 1 << (b - 1);

            if (--current_bit == 0)
            {
                current_byte++;
                current_bit = 8;
            }
        }
    }

    for (i = 0; i < length; i++)
    {
        // Werte mit Original-Vorzeichen behaften
        quant[i] *= signs[i];

        // Integrieren
        quant[i] += offset;
        offset = quant[i];

        // Rück-Skalieren
        _dequant_output[i] = quant[i] * scale;
    }

    _dequant_output_length = length;

#ifdef QUANT_DEBUG
    msg() << "output_length=" << _dequant_output_length;
    log(DLSDebug);
#endif
}

/*****************************************************************************/

template <class T>
const char *QuantT<T>::quant_output() const
{
    return _quant_output;
}

/*****************************************************************************/

template <class T>
unsigned int QuantT<T>::quant_output_size() const
{
    return _quant_output_size;
}

/*****************************************************************************/

template <class T>
const T *QuantT<T>::dequant_output() const
{
    return _dequant_output;
}

/*****************************************************************************/

template <class T>
unsigned int QuantT<T>::dequant_output_length() const
{
    return _dequant_output_length;
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif

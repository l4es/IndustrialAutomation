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

#ifndef LibDLSCompressionTH
#define LibDLSCompressionTH

/*****************************************************************************/

#include "ZLib.h"
#include "Base64.h"
#include "MdctT.h"
#include "QuantT.h"
#include "LibDLS/Exception.h"

//#define DEBUG

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/**
   Exception eines DLSCompression-Objektes
*/

class ECompression : public Exception
{
public:
    ECompression(const string &pmsg): Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Abstrakte Basisklasse eines Kompressionsobjektes
*/

template <class T>
class CompressionT
{
public:
    CompressionT() {};
    virtual ~CompressionT() {};

    /**
       Gibt alle persistenten Speicher frei.

       Die Speicher werden bei der nächsten Anforderung immer
       neu angelegt. Durch den Aufruf von free() wird der
       Speicher in der Zwischenzeit nicht unnötig belegt.
    */

    virtual void free() = 0;

    /**
       Komprimiert ein Array von Datenwerten beliebigen Typs

       \param input Konstanter Zeiger auf ein Array von Werten
       \param length Anzahl der Werte im Input-Array
       \throw ECompression Fehler beim Komprimieren
    */

    virtual void compress(const T *input,
                          unsigned int length) = 0;

    /**
       Wandelt komprimierte Binärdaten in ein Array von Datenwerten um

       \param input Konstanter Zeiger auf den Speicherbereich mit
       den komprimierten Binärdaten
       \param size Größe der komprimierten Daten in Bytes
       \param length Erwartete Anzahl von Datenwerten
       \throw ECompression Fehler beim Dekomprimieren
    */

    virtual void uncompress(const char *input,
                            unsigned int size,
                            unsigned int length) = 0;

    /**
       Leert den persistenten Speicher des Komprimierungsvorganges
       und liefert die restlichen, komprimierten Daten zurück

       \throw ECompression Fehler beim Komprimieren
    */

    virtual void flush_compress() = 0;

    /**
       Leert den persistenten Speicher des Dekomprimierungsvorganges
       und liefert die restlichen, dekomprimierten Daten

       \param input Konstanter Zeiger auf den Speicherbereich mit
       den zuvor von flush_compress() gelieferten Binärdaten
       \param size Größe der komprimierten Daten in Bytes
       \throw ECompression Fehler beim Dekomprimieren
    */

    virtual void flush_uncompress(const char *input,
                                  unsigned int size) = 0;

    /**
       Löscht alle Persistenzen, die von vorherigen Daten abhängig sind
    */

    virtual void clear() = 0;

    /**
       Liefert die Komprimierten Daten

       \return Konstanter Zeiger auf die komprimierten Daten
    */

    virtual const char *compression_output() const = 0;

    /**
       Liefert die Größe der komprimierten Daten

       \return Größe in Bytes
    */

    virtual unsigned int compressed_size() const = 0;

    /**
       Liefert die Dekomprimierten Daten

       \return Konstanter Zeiger auf die dekomprimierten Daten
    */

    virtual const T *decompression_output() const = 0;

    /**
       Liefert die Anzahl der dekomprimierten Datenwerte

       \return Anzahl Datenwerte
    */

    virtual unsigned int decompressed_length() const = 0;
};

/*****************************************************************************/
//
//  ZLib / Base64
//
/*****************************************************************************/

/**
   Kompressionsobjekt: Erst ZLib, dann Base64
*/

template <class T>
class CompressionT_ZLib : public CompressionT<T>
{
public:
    CompressionT_ZLib();
    ~CompressionT_ZLib();

    void compress(const T *input,
                  unsigned int length);
    void uncompress(const char *input,
                    unsigned int size,
                    unsigned int length);
    void clear();
    void flush_compress();
    void flush_uncompress(const char *input,
                          unsigned int size);

    void free();

    const char *compression_output() const;
    unsigned int compressed_size() const;
    const T *decompression_output() const;
    unsigned int decompressed_length() const;

private:
    ZLib _zlib;            /**< ZLib-Objekt zum Komprimieren */
    Base64 _base64;        /**< Base64-Objekt zum Kodieren */
};

/*****************************************************************************/

template <class T>
CompressionT_ZLib<T>::CompressionT_ZLib()
{
}

/*****************************************************************************/

template <class T>
CompressionT_ZLib<T>::~CompressionT_ZLib()
{
    free();
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::free()
{
    _zlib.free();
    _base64.free();
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::compress(const T *input,
                                       unsigned int length)
{
    stringstream err;

    try
    {
        _zlib.compress((char *) input, length * sizeof(T));
        _base64.encode(_zlib.output(), _zlib.output_size());
    }
    catch (EZLib &e)
    {
        err << "ZLib: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EBase64 &e)
    {
        err << "Base64: " << e.msg;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::uncompress(const char *input,
                                         unsigned int size,
                                         unsigned int length)
{
    stringstream err;

    free();

    try
    {
        _base64.decode(input, size);
        _zlib.uncompress(_base64.output(), _base64.output_size(),
                         length * sizeof(T));
    }
    catch (EBase64 &e)
    {
        err << "While Base64-decoding: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "While ZLib-uncompressing: " << e.msg << endl;
        throw ECompression(err.str());
    }

    if (_zlib.output_size() != length * sizeof(T))
    {
        err << "ZLib output does not have expected size: ";
        err << _zlib.output_size() << " / " << length * sizeof(T);
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::clear()
{
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::flush_compress()
{
    free();
}

/*****************************************************************************/

template <class T>
void CompressionT_ZLib<T>::flush_uncompress(const char *input,
                                               unsigned int size)
{
    free();
}

/*****************************************************************************/

template<class T>
const char *CompressionT_ZLib<T>::compression_output() const
{
    return _base64.output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_ZLib<T>::compressed_size() const
{
    return _base64.output_size();
}

/*****************************************************************************/

template<class T>
const T *CompressionT_ZLib<T>::decompression_output() const
{
    return (T *) _zlib.output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_ZLib<T>::decompressed_length() const
{
    return _zlib.output_size() / sizeof(T);
}

/*****************************************************************************/
//
//  MDCT / ZLib / Base64
//
/*****************************************************************************/

/**
   Kompressionsobjekt: Erst MDCT, dann ZLib und dann Base64
*/

template <class T>
class CompressionT_MDCT : public CompressionT<T>
{
public:
    CompressionT_MDCT(unsigned int, double);
    ~CompressionT_MDCT();

    void compress(const T *input,
                  unsigned int length);
    void uncompress(const char *input,
                    unsigned int size,
                    unsigned int length);
    void clear();
    void flush_compress();
    void flush_uncompress(const char *input,
                          unsigned int size);

    void free();

    const char *compression_output() const;
    unsigned int compressed_size() const;
    const T *decompression_output() const;
    unsigned int decompressed_length() const;

private:
    Base64 _base64;  /**< Base64-Objekt zum Kodieren */
    ZLib _zlib;      /**< ZLib-Objekt zum Komprimieren */
    MDCTT<T> *_mdct; /**< MDCT-Objekt zum Transformieren */

    CompressionT_MDCT() {}; // privat!
};

/*****************************************************************************/

template <class T>
CompressionT_MDCT<T>::CompressionT_MDCT(unsigned int dim,
                                              double acc)
{
    _mdct = 0;

    try
    {
        _mdct = new MDCTT<T>(dim, acc);
    }
    catch (EMDCT &e)
    {
        throw ECompression(e.msg);
    }
    catch (...)
    {
        throw ECompression("Could not allocate memory for MDCT object!");
    }
}

/*****************************************************************************/

template <class T>
CompressionT_MDCT<T>::~CompressionT_MDCT()
{
    if (_mdct) delete _mdct;
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::free()
{
    _zlib.free();
    _base64.free();
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::compress(const T *input,
                                       unsigned int length)
{
    stringstream err;

    try
    {
        _mdct->transform(input, length);
        _zlib.compress((char *) _mdct->mdct_output(),
                       _mdct->mdct_output_size());
        _base64.encode(_zlib.output(), _zlib.output_size());
    }
    catch (EMDCT &e)
    {
        err << "MDCT: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "ZLib: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EBase64 &e)
    {
        err << "Base64: " << e.msg;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::uncompress(const char *input,
                                         unsigned int size,
                                         unsigned int length)
{
    stringstream err;
    unsigned int max_size;

    max_size = _mdct->max_compressed_size(length);

    try
    {
        _base64.decode(input, size);
        _zlib.uncompress(_base64.output(), _base64.output_size(), max_size);
        _mdct->detransform(_zlib.output(), length);
    }
    catch (EBase64 &e)
    {
        err << "While Base64-decoding: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "While ZLib-uncompressing: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EMDCT &e)
    {
        err << "While MDCT-detransforming: " << e.msg << endl;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::clear()
{
    _mdct->clear();
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::flush_compress()
{
    stringstream err;

    try
    {
        _mdct->flush_transform();
        _zlib.compress(_mdct->mdct_output(), _mdct->mdct_output_size());
        _base64.encode(_zlib.output(), _zlib.output_size());
    }
    catch (EMDCT &e)
    {
        err << "MDCT flush: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "ZLib: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EBase64 &e)
    {
        err << "Base64: " << e.msg;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_MDCT<T>::flush_uncompress(const char *input,
                                               unsigned int size)
{
    stringstream err;
    unsigned int max_size;

    // Die maximale Datengröße vor ZLib ermitteln
    max_size = _mdct->max_compressed_size(0);

    try
    {
        _base64.decode(input, size);
        _zlib.uncompress(_base64.output(), _base64.output_size(), max_size);
        _mdct->flush_detransform(_zlib.output(), _zlib.output_size());
    }
    catch (EBase64 &e)
    {
        err << "While Base64-decoding: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "While ZLib-uncompressing: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EMDCT &e)
    {
        err << "While MDCT-detransforming: " << e.msg << endl;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template<class T>
const char *CompressionT_MDCT<T>::compression_output() const
{
    return _base64.output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_MDCT<T>::compressed_size() const
{
    return _base64.output_size();
}

/*****************************************************************************/

template<class T>
const T *CompressionT_MDCT<T>::decompression_output() const
{
    return _mdct->imdct_output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_MDCT<T>::decompressed_length() const
{
    return _mdct->imdct_output_length();
}

/*****************************************************************************/
//
//  Quant / ZLib / Base64
//
/*****************************************************************************/

/**
   Kompressionsobjekt: Erst Quantisierung, dann ZLib, dann Base64
*/

template <class T>
class CompressionT_Quant : public CompressionT<T>
{
public:
    CompressionT_Quant(double);
    ~CompressionT_Quant();

    void compress(const T *input,
                  unsigned int length);
    void uncompress(const char *input,
                    unsigned int size,
                    unsigned int length);
    void clear();
    void flush_compress();
    void flush_uncompress(const char *input,
                          unsigned int size);

    void free();

    const char *compression_output() const;
    unsigned int compressed_size() const;
    const T *decompression_output() const;
    unsigned int decompressed_length() const;

private:
    QuantT<T> *_quant;      /**< Quantisierungs-Objekt */
    ZLib _zlib;            /**< ZLib-Objekt zum Komprimieren */
    Base64 _base64;        /**< Base64-Objekt zum Kodieren */

    CompressionT_Quant();
};

/*****************************************************************************/

template <class T>
CompressionT_Quant<T>::CompressionT_Quant(double acc)
{
    _quant = 0;

    try
    {
        _quant = new QuantT<T>(acc);
    }
    catch (EQuant &e)
    {
        throw ECompression(e.msg);
    }
    catch (...)
    {
        throw ECompression("Could not allocate memory for"
                              " quantization object!");
    }
}

/*****************************************************************************/

template <class T>
CompressionT_Quant<T>::~CompressionT_Quant()
{
    free();

    if (_quant) delete _quant;
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::free()
{
    if (_quant) _quant->free();

    _zlib.free();
    _base64.free();
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::compress(const T *input,
                                        unsigned int length)
{
    stringstream err;

    if (!_quant) throw Exception("No quantization object!");

    try
    {
        _quant->quantize(input, length);
        _zlib.compress(_quant->quant_output(), _quant->quant_output_size());
        _base64.encode(_zlib.output(), _zlib.output_size());
    }
    catch (EQuant &e)
    {
        err << "Quant: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "ZLib: " << e.msg;
        throw ECompression(err.str());
    }
    catch (EBase64 &e)
    {
        err << "Base64: " << e.msg;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::uncompress(const char *input,
                                          unsigned int size,
                                          unsigned int length)
{
    stringstream err;

    if (!_quant) throw Exception("No quantization object!");

    free();

    try
    {
        _base64.decode(input, size);
        _zlib.uncompress(_base64.output(),
                         _base64.output_size(),
                         length * sizeof(T));
        _quant->dequantize(_zlib.output(), _zlib.output_size(), length);
    }
    catch (EBase64 &e)
    {
        err << "While Base64-decoding: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EZLib &e)
    {
        err << "While ZLib-uncompressing: " << e.msg << endl;
        throw ECompression(err.str());
    }
    catch (EQuant &e)
    {
        err << "While de-quantizing: " << e.msg << endl;
        throw ECompression(err.str());
    }

    if (_quant->dequant_output_length() != length)
    {
        err << "Quantization output does not have expected length: ";
        err << _quant->dequant_output_length() << " / " << length;
        throw ECompression(err.str());
    }
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::clear()
{
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::flush_compress()
{
    free();
}

/*****************************************************************************/

template <class T>
void CompressionT_Quant<T>::flush_uncompress(const char *input,
                                                unsigned int size)
{
    free();
}

/*****************************************************************************/

template<class T>
const char *CompressionT_Quant<T>::compression_output() const
{
    return _base64.output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_Quant<T>::compressed_size() const
{
    return _base64.output_size();
}

/*****************************************************************************/

template<class T>
const T *CompressionT_Quant<T>::decompression_output() const
{
    if (!_quant) throw Exception("No quantization object!");

    return _quant->dequant_output();
}

/*****************************************************************************/

template<class T>
unsigned int CompressionT_Quant<T>::decompressed_length() const
{
    if (!_quant) throw Exception("No quantization object!");

    return _quant->dequant_output_length();
}

/*****************************************************************************/

#ifdef DEBUG
#undef DEBUG
#endif

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif

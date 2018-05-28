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

#ifndef LibDLSGlobalsH
#define LibDLSGlobalsH

/*****************************************************************************/

#include <string>
#include <stdint.h>

/*****************************************************************************/

#define DLS_PID_FILE "dlsd.pid"
#define ENV_DLS_DIR  "DLS_DIR" // Name der Umgebungsvariablen
#define ENV_DLS_USER "DLS_USER" // Name der Umgebungsvariablen

/*****************************************************************************/

#ifdef _WIN32
#define DLS_EXPORT __declspec(dllexport)
#else
#define DLS_EXPORT
#endif

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/** Basic DLS channel datatype.
 */
enum ChannelType
{
    TUNKNOWN,
    TCHAR,
    TUCHAR,
    TSHORT,
    TUSHORT,
    TINT,
    TUINT,
    TLINT,
    TULINT,
    TFLT,
    TDBL
};

/*
  ---------------- Hinzufügen von Kanaltypen: ---------------------

  Editiert werden muss an folgenden Stellen:

  - Die beiden Konvertierungsfunktionen in lib/globals.cpp
  - In daemon/Logger::create_gen_saver()
  - In fltkguis/ViewChannel::fetch_chunks()
*/

ChannelType str_to_channel_type(const std::string &);
const char *channel_type_to_str(ChannelType);

/*****************************************************************************/

// Beim Erweitern bitte auch die Behandlungszweige
// in "_meta_value()" und "_ending()" anpassen!

enum MetaType
{
    MetaGen = 0,
    MetaMean = 1,
    MetaMin = 2,
    MetaMax = 4
};

std::string meta_type_str(MetaType);

/*****************************************************************************/

/**
   Beschreibt einen Kanal des MSR-Moduls
*/

struct RealChannel
{
    std::string name;            /**< Name des Kanals */
    std::string unit;            /**< Einheit */
    int index;              /**< Index */
    ChannelType type;    /**< Kanaltyp (TUINT, TDBL, usw.) */
    unsigned int bufsize;   /**< Größe des Kanalpuffers im MSR-Modul */
    unsigned int frequency; /**< Maximale Abtastrate des Kanals */
};

bool operator<(const RealChannel &a, const RealChannel &b);

/*****************************************************************************/

enum {
	FORMAT_INVALID = -1,
	FORMAT_ZLIB,
	FORMAT_MDCT,
	FORMAT_QUANT,
	FORMAT_COUNT
};

/*
  Hinzufügen von Kompressionsarten:
  - globals.cpp    (String-Array)
  - CompressionT.h (Neue Klasse)
*/

extern const char *format_strings[FORMAT_COUNT];

/*****************************************************************************/

#pragma pack(push, 1)

/**
   Index-Record für einen Datendatei-Index innerhalb eines Chunks
*/

struct IndexRecord
{
    uint64_t start_time;
    uint64_t end_time;
    uint32_t position;
};

/*****************************************************************************/

/**
   Index für alle Datendateien eines Chunks
*/

struct GlobalIndexRecord
{
    uint64_t start_time;
    uint64_t end_time;
};

/*****************************************************************************/

/**
   Index für Messages
*/

struct MessageIndexRecord
{
    uint64_t time;
    uint32_t position;
};

#pragma pack(pop)

/*****************************************************************************/

enum {
    MSR_PORT = 2345
};

/*****************************************************************************/

typedef void (*LoggingCallback)(const char *, void *);

void set_logging_callback(LoggingCallback, void *);

void log(const std::string &);

std::string convert_to_bin(const void *, unsigned int, int);

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif

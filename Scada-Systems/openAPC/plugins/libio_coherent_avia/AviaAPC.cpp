#include "AviaAPC.h"
#include "FLib.h"
#include "FClock.h"
#include "FException.h"

#include "oapc_libio.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <string.h>

using namespace std;
using namespace Frewitt;

#ifdef _DEBUG
// #define TEST_MODE // if enabled no external hardware is required
#endif


const unsigned int Avia::PAUSE_WAIT_MS = 4 * 1000; // in

void Avia::Init() {
	pSocket = NULL;
} // Init

Avia::Avia(const std::string& host, const unsigned short port) {
#ifndef TEST_MODE
	try {
		Init();
		pSocket = new Frewitt::TCPSocket(host, port);
		Reset();
		Set(WR_PROMPT, 0);
		Set(WR_ECHO, 1);
		Set(WR_SHUTTER, 0);
		Set(WR_PULSE, 0);
		Set(WR_DIODE, 0);
	} catch (const exception& e) {
		std::cerr << "DEBUG: Avia::Constructor: " << e.what() << std::endl;
	}
#endif
} // Avia

Avia::~Avia() {
#ifndef TEST_MODE
	if (NULL != pSocket) delete pSocket;
#endif
} // ~Avia

void Avia::Load(struct instData *data) {
#ifndef TEST_MODE
   Set(WR_SHUTTER, 0);
   Set(WR_PULSE, 0);
   Set(WR_DIODE, 0);
   Set(WR_LOCK, 1);
   Set(WR_CURRENT, data->config.m_mCurr/1000.0);
   Set(WR_THERMATRACK, data->config.m_thermatrack);

   strncpy(data->m_statusMessage,"Setting frequency...",MAX_STATUSMESSAGE_LENGTH);
   m_oapc_io_callback(OAPC_CHAR_IO4,data->m_callbackID);

   Set(WR_FREQUENCY, data->config.m_freq);
   if (data->config.m_triggerMode==1) Set(WR_TRIGGER_MODE, 0);
   else Set(WR_TRIGGER_MODE, 1);
   if (data->config.m_pulseControl==1) Set(WR_PULSE_CONTROL, 2);
   else Set(WR_PULSE_CONTROL, 4);
   Set(WR_SHG_TEMPERATURE, data->config.m_mSHGTemp/1000.0);
   Set(WR_FHG_TEMPERATURE, data->config.m_mFHGTemp/1000.0);
   //TODO: Disable set temperature for the moment
   //SHGTemperature(recipe.SHGTemp());
   //FHGTemperature(recipe.FHGTemp());
   //Diode1Temperature(recipe.Diode1Temp());
   //Diode2Temperature(recipe.Diode2Temp());
#else
    strncpy(data->m_statusMessage,"Initializing dummy laser controller...",MAX_STATUSMESSAGE_LENGTH);
    m_oapc_io_callback(OAPC_CHAR_IO4,data->m_callbackID);
#endif
} // Load

void Avia::A_Set(const std::string& param, const std::string& value) {
#ifndef TEST_MODE
   std::string query;
   std::string reply;

   query = param;
   query.append(value);
   pSocket->write_CR(query);
   reply = pSocket->readln();
   FLib::TrimCRLF(reply);
   if (query == reply) {
      return;
   } else {
      //			std::cerr << "DEBUG: Avia::Set: 1st ECHO error with query: " << query << " reply: " << reply << endl;
      // if ECHO not correct
      // send CR, and ignore reply
      pSocket->write_CR("");
      reply = pSocket->readln();

      pSocket->write_CR(query);
      reply = pSocket->readln();
      FLib::TrimCRLF(reply);
      if (query == reply) {
         return;
      } else {
         // second time incorrect, failure
         std::cerr << "DEBUG: Avia::A_Set: 2nd ECHO error with query: " << query << " reply: " << reply << std::endl;
         return;
      }
   } // if
#endif
} // A_Set

std::string Avia::A_Get(const std::string& query) const {
#ifndef TEST_MODE
   std::string reply;
   std::string echo;
   std::string data;

   pSocket->write_CR(query);
   reply = pSocket->readln();
   FLib::TrimCRLF(reply);
   try {
      echo.assign(reply, 0, query.length());
      data.assign(reply, query.length(), reply.length() - query.length());
   } catch (const exception& /*e*/) {
      echo = "";
      data = "";
   }

   if (echo == query) {
      return data;
   } else {
      // if ECHO not correct
      // send CR, and ignore reply
      //			std::cerr << "DEBUG: Avia::Get: 1st ECHO error with query: " << query << " reply: " << reply << std::endl;
      pSocket->write_CR("");
      reply = pSocket->readln();

      pSocket->write_CR(query);
      reply = pSocket->readln();
      FLib::TrimCRLF(reply);

      try {
         echo.assign(reply, 0, query.length());
         data.assign(reply, query.length(), reply.length() - query.length());
      } catch (const exception& /*e*/) {
         echo = "";
         data = "";
      }

      if (echo == query) {
         return data;
      } else {
         std::cerr << "DEBUG: Avia::A_Get: 2nd ECHO error with query: " << query << " reply: " << reply << std::endl;
      }
   } // if
#endif
	return "0";
} // A_Get

bool Avia::Ready() const {
#ifndef TEST_MODE
   bool ready = ("SYSTEM OK" == A_Get(RD_SYSTEM_OK));
   return ready;
#else
   return true;
#endif
} // Ready

double Avia::Get(const std::string& param) const {
   std::string value;
   value = A_Get(param);
   double b = false;
   std::stringstream (value) >> b;
   return b;
} // Get

void Avia::Set(const std::string& param, const double value) {
#ifndef TEST_MODE
   std::stringstream s;
   s << value;
   A_Set(param, s.str());
#endif
} // Set

// set and wait until value really changed on the laser
bool Avia::Set_W(const std::string& wr_param, const std::string& rd_param, const double value, const short timeout) {
#ifndef TEST_MODE
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      Set(wr_param, value);
      const double result = Get(rd_param);
      if (result == value) return true;
      std::cout << "Waiting " << rd_param << "=" << value << "..." << std::endl;
      FLib::Sleep(PAUSE_WAIT_MS);
      std::cout << LogLineConsole();
   } // while
	std::cout << "...timeout." << std::endl;
	return false;
#else
	return true;
#endif
} // Set_W

bool Avia::Wait(const std::string& param, const double value, const short timeout) {
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      const double result = Get(param);
      if (result == value) return true;
      std::cout << "Waiting " << param << "=" << value << "..." << std::endl;
      FLib::Sleep(PAUSE_WAIT_MS);
      std::cout << LogLineConsole();
   }
	std::cout << "...timeout." << std::endl;
	return false;
} // Wait

void Avia::Reset() {
   A_Set("EXIT=", "1");
} // Reset

std::string Avia::ErrorCode() {
	std::string err = A_Get(RD_SYSTEM_OK);
	if (err == "SYSTEM OK") return "";
	return err;
} // ErrorCode

std::string Avia::LogLineConsole() const {
   std::stringstream out;
   out << std::fixed << std::setprecision(1)
   << Get(RD_UV_POWER) << "W" << " "
   << Get(RD_UV_ENERGY) << "uJ" << " "
   << Get(RD_CURRENT) <<  "A" << " "
   << "FHG:"  << Get(RD_FHG_TEMPERATURE) << " "
   << "SHG:"  << Get(RD_SHG_TEMPERATURE) << " "
   << "D1:"   << Get(RD_DIODE_1_TEMPERATURE)  << " "
   << "D2:"   << Get(RD_DIODE_1_TEMPERATURE)  << " "
   << "Rod:"  << Get(RD_ROD_TEMPERATURE) << " "
   << "Base:" << Get(RD_BASE_TEMPERATURE) << " "
   << "Lock:" << Get(RD_LOCK)
   << std::endl;
   return out.str();
} // LogLineConsole

std::string Avia::error_message (unsigned error) {
	std::string mess;
	switch (error) {
		case 1: mess = "Emisssion Indicator";
			break;
		case 2: mess = "External Interlock";
			break;
		case 3: mess = "PS Cover Interlock";
			break;
		case 4: mess = "SHG Temp.";
			break;
		case 6: mess = "Laser Rods Temp.";
			break;
		case 7: mess = "FHG Temp.";
			break;
		case 8: mess = "Diode 1 Temp.";
			break;
		case 9: mess = "Diode 2 Temp.";
			break;

		case 10: mess = "Baseplate Temp.";
			break;
		case 11: mess = "Heatsink 1 Temp.";
			break;
		case 12: mess = "Heatsink 2 Temp.";
			break;
		case 14: mess = "PS DC Voltage";
			break;
		case 15: mess = "PS AC Voltage";
			break;
		case 19: mess = "Diode 1 Under Volt";
			break;

		case 20: mess = "Diode 2 Under Volt";
			break;
		case 21: mess = "Diode 1 Over Volt";
			break;
		case 22: mess = "Diode 2 Over Volt";
			break;
		case 25: mess = "Diode 1 EEPROM";
			break;
		case 26: mess = "Diode 1 EEPROM";
			break;
		case 27: mess = "Laser Head EEPROM";
			break;
		case 28: mess = "Power Supply EEPROM";
			break;
		case 29: mess = "PS-Head Mismatch";
			break;

		case 30: mess = "SHG Battery";
			break;
		case 31: mess = "Shutter State Mismatch";
			break;
		case 32: mess = "Head Reset";
			break;
		case 33: mess = "Head Communication";
			break;
		case 34: mess = "Head Comm. Out of Sync";
			break;
		case 35: mess = "UART Hardware";
			break;
		case 36: mess = "Power Lock";
			break;
		case 37: mess = "Servo Temp.";
			break;
		case 38: mess = "Front Panel Switch";
			break;
		case 39: mess = "Shutter Interlock";
			break;
		default:
			mess = "N/A";
			break;
	}
	mess.append(" Fault");
	return mess;
} // error_message

bool Avia::Temperature_OK() const {
   const unsigned int Rod_status = (unsigned int)OAPC_ROUND(Get(RD_ROD_SERVO_STATUS),0);
   const unsigned int D1_status  = (unsigned int)OAPC_ROUND(Get(RD_DIODE_1_SERVO_STATUS),0);
   const unsigned int D2_status  = (unsigned int)OAPC_ROUND(Get(RD_DIODE_2_SERVO_STATUS),0);
   const unsigned int SHG_status = (unsigned int)OAPC_ROUND(Get(RD_SHG_SERVO_STATUS),0);
   const unsigned int FHG_status = (unsigned int)OAPC_ROUND(Get(RD_FHG_SERVO_STATUS),0);

   if (1 == Rod_status)
      if ((1 == D1_status) || (4 == D1_status))
         if ((1 == D2_status) || (4 == D2_status))
            if ((1 == SHG_status) || (4 == SHG_status))
               if ((1 == FHG_status) || (4 < FHG_status) || (FHG_status < 7))
                  return true;
   return false;
} // Temperature_OK

bool Avia::Current_OK(struct instData *data) const {
   const double current = Get(RD_CURRENT);
   if (current > (int)data->config.m_currLimit) return true;
   return false;
} // Current_OK

bool Avia::UV_OK(struct instData *data) const {
   const double energy = Get(RD_UV_ENERGY);
   const double power = Get(RD_UV_POWER);
   if ((energy > data->config.m_mEnergyLimit/1000.) && (power > data->config.m_mPowerLimit/1000.)) return true;
   return false;
} // UV_OK

bool Avia::Wait_Temperature(const short timeout) {
#ifndef TEST_MODE
   std::cout << "Waiting temperature..." << std::endl;
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      std::cout << LogLineConsole();
      if (Temperature_OK()) {
    	  std::cout << "...temperature OK." << std::endl;
    	  return true;
      }
      FLib::Sleep(PAUSE_WAIT_MS);
   } // while
   std::cout << "...timeout." << std::endl;
   return false;
#else
   return true;
#endif
} // Wait_Temperature

bool Avia::Wait_UV(struct instData *data, const short timeout) {
#ifndef TEST_MODE
   std::cout << "Waiting UV : power > " << data->config.m_mPowerLimit/1000. << "W" << "  energy > " <<  data->config.m_mEnergyLimit/1000. << "uJ" << std::endl;
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      std::cout << LogLineConsole();
      if (UV_OK(data)) {
    	  std::cout << "...UV OK." << std::endl;
    	  return true;
      }
      FLib::Sleep(PAUSE_WAIT_MS);
   } // while
   std::cout << "...timeout." << std::endl;
   return false;
#else
   return true;
#endif
} // Wait_UV

bool Avia::Wait_Current(struct instData *data, const short timeout) {
#ifndef TEST_MODE
   std::cout << "Waiting current > " << (int)data->config.m_currLimit << "A" << std::endl;
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      std::cout << LogLineConsole();
      if (Current_OK(data)) {
    	   std::cout << "...current OK." << std::endl;
    	   return true;
      }
      FLib::Sleep(PAUSE_WAIT_MS);
   } // while
   std::cout << "...timeout." << std::endl;
   return false;
#else
   return true;
#endif
} // Wait_Current

bool Avia::Wait_Thermatrack(const short timeout) {
   std::cout << "Waiting thermatrack..." << std::endl;
	short tout = (int)timeout*1000 / PAUSE_WAIT_MS;
	for (short i=0; i<tout; i++) {
      std::cout << LogLineConsole();
      FLib::Sleep(PAUSE_WAIT_MS);
      const bool moving = (Get(RD_MOVING)!=0.0);
      if (!moving) return true;
   } // while
   std::cout << "...timeout." << std::endl;
   return false;
} // Wait_Thermatrack

void Avia::Search_UV(struct instData *data) {
   int track;

   std::cout << FClock::Now().Date() << FLib::TAB << FClock::Now().Time() << FLib::TAB << "*** starting UV coarse search" << std::endl;

   Set(WR_LOCK, 1);
   if (!Get(RD_LOCK)) throw FException("Avia", "Search_UV", "not locked", __FILE__, __LINE__);
   Set(WR_TRIGGER_MODE, 0);
   Set(WR_PULSE_CONTROL, 2);
   Set(WR_DIODE, 0);
   Set(WR_PULSE, 0);
   Set(WR_SHUTTER, 0);

   // disable FHG constant optimization
   if (!Set_W(WR_FHG_OPTIMIZE, RD_FHG_OPTIMIZE, 0)) return;

   // factory settings
   Set(WR_DIODE_1_TEMPERATURE, 31.3); // ?
   Set(WR_DIODE_2_TEMPERATURE, 27.2); // ?
   Set(WR_FHG_TEMPERATURE, 150.9);   // 144.5, to adjust for 40 kHz and 85%
   Set(WR_SHG_TEMPERATURE, 151.3);   // 151.3, to adjust for 40 kHz and 85%

   const double FHG_mini = 150.0;
   const double FHG_maxi = 151.0;
   const double FHG_step = 0.1;

   const double SHG_mini = 151.0;
   const double SHG_maxi = 152.0;
   const double SHG_step = 0.1;

   const int Track_mini = 500;
   const int Track_maxi = 6000;
   const int Track_step = 500;

   Set(WR_SHG_TEMPERATURE, SHG_mini);
   Set(WR_FHG_TEMPERATURE, FHG_mini);
   Set(WR_THERMATRACK, Track_mini); // 3000, to adjust for 40 kHz and 85%
   Set(WR_FREQUENCY, 40000); // 40000
   Set(WR_CURRENT, 85.0); // 85
   if (!Wait_Temperature()) return;
   if (!Set_W(WR_DIODE, RD_DIODE, 1)) return;
   if (!Set_W(WR_PULSE, RD_PULSE, 1)) return;
   if (!Wait_Current(data)) return;
   if (!Wait_Temperature()) return;

   for (double SHG = SHG_mini; SHG <= SHG_maxi; SHG += SHG_step) {
      Set(WR_SHG_TEMPERATURE, SHG);
      for (double FHG = FHG_mini; FHG <= FHG_maxi; FHG += FHG_step) {
         std::cout << "***" << std::endl;

         if (!Get(RD_DIODE)) if (!Set_W(WR_DIODE, RD_DIODE, 1)) return;
         if (!Get(RD_PULSE)) if (!Set_W(WR_PULSE, RD_PULSE, 1)) return;

         Set(WR_THERMATRACK, Track_mini);
         if (!Wait_Thermatrack()) return;
         Set(WR_FHG_TEMPERATURE, FHG);
         if (!Wait_Temperature()) return;
         for (track = Track_mini; track <= Track_maxi; track += Track_step) {
            Set(WR_THERMATRACK, track);
            if (!Wait_Thermatrack()) return;
         } // for

         if (!Get(RD_DIODE)) if (!Set_W(WR_DIODE, RD_DIODE, 1)) return;
         if (!Get(RD_PULSE)) if (!Set_W(WR_PULSE, RD_PULSE, 1)) return;

         FHG += FHG_step;

         std::cout << "***" << std::endl;

         Set(WR_FHG_TEMPERATURE, FHG);
         if (!Wait_Temperature()) return;

         for (track = Track_maxi; track > Track_mini; track -= Track_step) {
            Set(WR_THERMATRACK, track);
            if (!Wait_Thermatrack()) return;
         } // for
      } // for
   } // for

   Set(WR_DIODE, 0);
   Set(WR_PULSE, 0);
   Set(WR_SHUTTER, 0);

   std::cout << FClock::Now().Date() << FLib::TAB << FClock::Now().Time() << FLib::TAB << "*** end UV coarse search" << std::endl;
} // Search_UV

void Avia::Optimize_UV(struct instData *data) {
   if (!Set_W(WR_LOCK, RD_LOCK, 1)) return;
   if (!Get(RD_LOCK)) throw FException("Avia", "Optimize_UV", "not locked", __FILE__, __LINE__);
   Set(WR_TRIGGER_MODE, 0);
   Set(WR_PULSE_CONTROL, 2);
   if (!Set_W(WR_DIODE, RD_DIODE, 0)) return;
   if (!Set_W(WR_PULSE, RD_PULSE, 0)) return;
   if (!Set_W(WR_SHUTTER, RD_SHUTTER, 0)) return;

   std::cout << FClock::Now() << FLib::TAB << FClock::Now().Time() << FLib::TAB << "*** starting UV optimization" << std::endl;

   // factory settings
   Set(WR_DIODE_1_TEMPERATURE, 31.3);
   Set(WR_DIODE_2_TEMPERATURE, 27.2);
   Set(WR_FHG_TEMPERATURE, 150.9);
   Set(WR_SHG_TEMPERATURE, 151.6);
   Set(WR_THERMATRACK, 3105);
   Set(WR_FREQUENCY, 40000);
   Set(WR_CURRENT, 85.0);
   if (!Wait_Temperature()) return;

   if (!Set_W(WR_DIODE, RD_DIODE, 1)) return;
   if (!Set_W(WR_PULSE, RD_PULSE, 1)) return;

   if (!Wait_Current(data)) return;
   if (!Wait_Temperature()) return;

   std::cout << "starting 1st Thermatrack optimization..." << std::endl;
   if (!Set_W(WR_THERMATRACK_OPTIMIZE, RD_THERMATRACK_OPTIMIZE, 1)) return;
   if (!Wait(RD_THERMATRACK_OPTIMIZE, 0)) return;
   std::cout << FClock::Now() << FLib::TAB << "Thermatrack optimization done; position = " << Get("thermatrack") << std::endl;

   // disable FHG constant optimization
   if (!Set_W(WR_FHG_OPTIMIZE, RD_FHG_OPTIMIZE, 0)) return;

   std::cout << "starting SHG optimization..." << std::endl;
   if (!Set_W(WR_SHG_OPTIMIZE, RD_SHG_OPTIMIZE, 1)) return;
   if (!Wait(RD_SHG_OPTIMIZE, 0)) return;
   std::cout << FClock::Now() << FLib::TAB << "SHG optimization done; SHG temp = " << Get("SHG-temperature") << std::endl;
   std::cout << FClock::Now() << FLib::TAB << "SHG optimization done; SHG temp set = " << Get("SHG-temperature-set") << std::endl;

   std::cout << "starting FHG optimization..." << std::endl;
   if (!Set_W(WR_FHG_OPTIMIZE, RD_FHG_OPTIMIZE, 1)) return;
   if (!Wait(RD_FHG_OPTIMIZE, 0)) return;
   std::cout << FClock::Now() << FLib::TAB << "FHG constant optimization done; FHG temp = " << Get("FHG-temperature") << std::endl;
   std::cout << FClock::Now() << FLib::TAB << "FHG constant optimization done; FHG temp set = " << Get("FHG-temperature-set") << std::endl;

   std::cout << "starting 2nd Thermatrack optimization..." << std::endl;
   if (!Set_W(WR_THERMATRACK_OPTIMIZE, RD_THERMATRACK_OPTIMIZE, 1)) return;
   if (!Wait(RD_THERMATRACK_OPTIMIZE, 0)) return;
   std::cout << FClock::Now() << FLib::TAB << "Thermatrack optimization done; position = " << Get("thermatrack") << std::endl;

   std::cout << FClock::Now() << std::endl;
   std::cout << Version() << std::endl;
   std::cout << "SHG temp        = " << Get(RD_SHG_TEMPERATURE) << std::endl;
   std::cout << "SHG temp set    = " << Get(RD_SHG_TEMPERATURE_SET) << std::endl;
   std::cout << "FHG temp        = " << Get(RD_FHG_TEMPERATURE) << std::endl;
   std::cout << "FHG temp set    = " << Get(RD_FHG_TEMPERATURE_SET) << std::endl;
   std::cout << "Thermatrack pos = " << Get(RD_THERMATRACK) << std::endl;

   Set(WR_DIODE, 0);
   Set(WR_PULSE, 0);
   Set(WR_SHUTTER, 0);

   std::cout << FClock::Now().Date() << FLib::TAB << FClock::Now().Time() << FLib::TAB << "*** end UV optimization" << std::endl;
} // Optimze_UV

std::string Avia::Version() const {
   std::stringstream line;
   line << std::setw(3) << std::setprecision(3);
   line << "AVIA version:" << std::endl;
   line << "Power supply S/N         : " << Get(RD_POWER_SUPPLY_SN) << std::endl;
   line << "Power supply software    : " << Get(RD_POWER_SUPPLY_SOFTWARE) << std::endl;
   line << "Head S/N                 : " << Get(RD_HEAD_SN) << std::endl;
   line << "Head software            : " << Get(RD_HEAD_SOFTWARE) << std::endl;
   line << "Head hours            [h]: " << Get(RD_HEAD_HOURS) << std::endl;
   line << "Battery voltage       [V]: " << Get(RD_BATTERY_VOLTAGE) << std::endl;
   line << "Crystal warning       [h]: " << Get(RD_CRYSTAL_WARNING) << std::endl;
   line << "Diode 1 hours         [h]: " << Get(RD_DIODE_1_HOURS) << std::endl;
   line << "Diode 1 set temp. [deg C]: " << Get(RD_DIODE_1_SET_TEMPERATURE) << std::endl;
   line << "Diode 1 temp.     [deg C]: " << Get(RD_DIODE_1_TEMPERATURE) << std::endl;
   line << "Diode 2 hours         [h]: " << Get(RD_DIODE_2_HOURS) << std::endl;
   line << "Diode 2 set temp. [deg C]: " << Get(RD_DIODE_2_SET_TEMPERATURE) << std::endl;
   line << "Diode 2 temp.     [deg C]: " << Get(RD_DIODE_2_TEMPERATURE) << std::endl;
   line << "SHG set temp.     [deg C]: " << Get(RD_SHG_TEMPERATURE_SET) << std::endl;
   line << "SHG temp.         [deg C]: " << Get(RD_SHG_TEMPERATURE) << std::endl;
   line << "FHG set temp.     [deg C]: " << Get(RD_FHG_TEMPERATURE_SET) << std::endl;
   line << "FHG temp.         [deg C]: " << Get(RD_FHG_TEMPERATURE) << std::endl;
   line << "Base plate temp.  [deg C]: " << Get(RD_BASE_TEMPERATURE) << std::endl;
   line << "Laser rod temp.   [deg C]: " << Get(RD_ROD_TEMPERATURE) << std::endl;
   line << "Frequency            [Hz]: " << Get(RD_FREQUENCY) << std::endl;
   line << "Spot location            : " << Get(RD_SPOT_LOCATION) << std::endl;
   line << "Spot hours            [h]: " << Get(RD_SPOT_HOURS + A_Get(RD_SPOT_LOCATION)) << std::endl;
   line << "Thermatrack              : " << Get(RD_THERMATRACK) << std::endl;
   line << "Diode current         [A]: " << Get(RD_CURRENT) << std::endl;
   return line.str();
} // Version

std::string Avia::Status() const {
   std::stringstream line;
   line << "AVIA status:" << std::endl;
   line << "Ready        : " << Ready() << std::endl;
   line << "Main switch  : " << Get(RD_KEY) << std::endl;
   line << "Shutter      : " << Get(RD_SHUTTER) << std::endl;
   line << "Pulse        : " << Get(RD_PULSE) << std::endl;
   line << "Diode        : " << Get(RD_DIODE) << std::endl;
   line << "Lock         : " << Get(RD_LOCK) << std::endl;
   return line.str();
} // Status

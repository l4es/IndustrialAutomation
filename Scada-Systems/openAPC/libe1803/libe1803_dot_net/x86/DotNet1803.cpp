// This is the main DLL file.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "DotNet1803.h"
#include "StringToPointer.h"

#include "../../libe1803.h"

using namespace System;
using namespace System::Threading;

namespace libe1803_dot_net
{
	unsigned char cardNum = 0;
	DotNet1803::~DotNet1803()
	{
		close();
	}
	DotNet1803::DotNet1803()
	{

	}
	DotNet1803::DotNet1803(String^ address)
	{
		set_connection(address);
		load_correction();
	}
	DotNet1803::DotNet1803(String^ address, String^ ethPwd)
	{
		set_connection(address, ethPwd);
		load_correction();
	}
	DotNet1803::DotNet1803(String^ address, String^ ethPwd, String^ correctionfilename)
	{
		set_connection(address, ethPwd);
		load_correction(correctionfilename);
	}
	DotNet1803::DotNet1803(String^ address, String^ ethPwd, String^ correctionfilename, unsigned char correctiontableNum)
	{
		set_connection(address, ethPwd);
		load_correction(correctionfilename, correctiontableNum);
	}

	bool DotNet1803::set_connection(String^ address)
	{
		return set_connection(address, "");
	}

	bool DotNet1803::set_connection(String^ address, String^ ethPwd)
	{
		if (cardNum > 0)
			close();

		WSADATA      wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData); // Needs wsock32.lib as dependancy.  Important before any set_connection

		StringToPointer stp(address);
		cardNum = E1803_set_connection(stp.pointer);
		stp.Free();
		if (cardNum == 0)
		{
			WSACleanup();
			return false;
		}
		if (!String::IsNullOrEmpty(ethPwd))
			set_password(ethPwd);
		return true;
	}

	void DotNet1803::set_password(String^ ethPwd)
	{
		if (!String::IsNullOrEmpty(ethPwd))
		{
			StringToPointer stp(ethPwd);
			E1803_set_password(cardNum, stp.pointer);
			stp.Free();
		}
	}

	bool DotNet1803::set_filepath(unsigned char n, String^ fname, unsigned int mode)
	{
		StringToPointer stp(fname);
		bool results = TestReturn(E1803_set_filepath(cardNum, stp.pointer, mode));
		return results;
	}

	bool DotNet1803::set_debug_logfile(String^ path)
	{
		return set_debug_logfile(path, 0x01 | 0x02);
	}

	bool DotNet1803::set_debug_logfile(String^ path, unsigned char flags)
	{
		StringToPointer stp(path);
		bool results = TestReturn(E1803_set_debug_logfile(cardNum, stp.pointer, flags));
		stp.Free();
		return results;
	}

	void DotNet1803::close()
	{
		if (cardNum > 0)
		{
			E1803_close(cardNum);
			cardNum = 0;
			WSACleanup();
		}
	}

	bool DotNet1803::load_correction()
	{
		return load_correction("", 0);
	}

	bool DotNet1803::load_correction(String^ filename)
	{
		return load_correction(filename, 0);
	}

	bool DotNet1803::load_correction(String^ filename, unsigned char tableNum)
	{
		if (String::IsNullOrEmpty(filename)) // don't bother attempting to allocate
		{
			return TestReturn(E1803_load_correction(cardNum, NULL, tableNum));
		}
		StringToPointer stp(filename);
		bool results = TestReturn(E1803_load_correction(cardNum, stp.pointer, tableNum));
		stp.Free();
		return results;
	}

	bool DotNet1803::switch_correction(unsigned char tableNum)
	{
		bool results = TestReturn(E1803_switch_correction(cardNum, tableNum));
		return results;
	}

	bool DotNet1803::set_xy_correction(unsigned int flags, double gainX, double gainY, double rot, int offsetX, int offsetY, double slantX, double slantY)
	{
		bool results = TestReturn(E1803_set_xy_correction(cardNum, flags, gainX, gainY, rot, offsetX, offsetY, slantX, slantY));
		return results;
	}

	bool DotNet1803::set_z_correction(unsigned int h, double xy_to_z_ratio, int res2)
	{
		bool results = TestReturn(E1803_set_z_correction(cardNum, h, xy_to_z_ratio, res2));
		return results;
	}

	bool DotNet1803::tune(unsigned int tuneFlags)
	{
		bool results = TestReturn(E1803_tune(cardNum, tuneFlags));
		return results;
	}

	bool DotNet1803::set_speeds(double jumpspeed, double markspeed)
	{
		bool results = TestReturn(E1803_set_speeds(cardNum, jumpspeed, markspeed));
		return results;
	}

	bool DotNet1803::set_laser_delays(double ondelay, double offdelay)
	{
		bool results = TestReturn(E1803_set_laser_delays(cardNum, ondelay, offdelay));
		return results;
	}

	bool DotNet1803::set_laser_mode(unsigned int mode)
	{
		bool results = TestReturn(E1803_set_laser_mode(cardNum, mode));
		return results;
	}

	bool DotNet1803::set_laser_mode(lasermode lmode)
	{
		unsigned int mode = 0;
		if (lmode == lasermode::LASERMODE_CO2)
			mode = E1803_LASERMODE_CO2;
		if (lmode == lasermode::LASERMODE_YAG1)
			mode = E1803_LASERMODE_YAG1;
		if (lmode == lasermode::LASERMODE_YAG2)
			mode = E1803_LASERMODE_YAG2;
		if (lmode == lasermode::LASERMODE_YAG3)
			mode = E1803_LASERMODE_YAG3;
		if (lmode == lasermode::LASERMODE_CRF)
			mode = E1803_LASERMODE_CRF;
		if (lmode == lasermode::LASERMODE_DFREQ)
			mode = E1803_LASERMODE_DFREQ;
		if (lmode == lasermode::LASERMODE_IPG)
			mode = E1803_LASERMODE_IPG;
		bool results = TestReturn(E1803_set_laser_mode(cardNum, mode));
		return results;
	}

	bool DotNet1803::set_laser(bool bOn)
	{
		return set_laser(E1803_COMMAND_FLAG_DIRECT, bOn);
	}

	bool DotNet1803::set_laser(unsigned int flags, bool bOn)
	{
		char on = 0;
		if (bOn)
			on = 1;
		return set_laser(flags, on);
	}

	bool DotNet1803::set_laser(unsigned int flags, char on)
	{
		bool results = TestReturn(E1803_set_laser(cardNum, flags, on));
		return results;
	}

	bool DotNet1803::set_wobble(unsigned int x, unsigned int y, double freq)
	{
		bool results = TestReturn(E1803_set_wobble(cardNum, x, y, freq));
		return results;
	}

	bool DotNet1803::set_scanner_delays(double jumpdelay, double markdelay, double polydelay)
	{
		return set_scanner_delays(0, jumpdelay, markdelay, polydelay);
	}

	bool DotNet1803::set_scanner_delays(unsigned int flags, double jumpdelay, double markdelay, double polydelay)
	{
		if (jumpdelay < 0.5)
			jumpdelay = 0.5;
		if (markdelay < 0.5)
			markdelay = 0.5;
		if (polydelay < 0.5)
			polydelay = 0.5;
		bool results = TestReturn(E1803_set_scanner_delays(cardNum, flags, jumpdelay, markdelay, polydelay));
		return results;
	}

	bool DotNet1803::jump_abs(int x, int y)
	{
		return jump_abs(x, y, 0);
	}

	bool DotNet1803::jump_abs(int x, int y, int z)
	{
		bool results = TestReturn(E1803_jump_abs(cardNum, x, y, z));
		return results;
	}

	bool DotNet1803::mark_abs(int x, int y)
	{
		return mark_abs(x, y, 0);
	}

	bool DotNet1803::mark_abs(int x, int y, int z)
	{
		bool results = TestReturn(E1803_mark_abs(cardNum, x, y, z));
		return results;
	}

	bool DotNet1803::set_pixelmode(pixelmode pmode, double powerThres)
	{
		unsigned int mode = 0;
		if (pmode == pixelmode::PIXELMODE_NO_JUMPS)
			mode = E1803_PIXELMODE_NO_JUMPS;
		else if (pmode == pixelmode::PIXELMODE_JUMP_N_SHOOT)
			mode = E1803_PIXELMODE_JUMP_N_SHOOT;
		else if (pmode == pixelmode::PIXELMODE_HW_POWER_CONTROL)
			mode = E1803_PIXELMODE_HW_POWER_CONTROL;
		else if (pmode == pixelmode::PIXELMODE_GATE_POWER_CONTROL)
			mode = E1803_PIXELMODE_GATE_POWER_CONTROL;
		return set_pixelmode(mode, powerThres, 0);
	}

	bool DotNet1803::set_pixelmode(unsigned int mode, double powerThres, unsigned int res)
	{
		bool results = TestReturn(E1803_set_pixelmode(cardNum, mode, powerThres, res));
		return results;
	}

	// User is responsible for making sure memory is not garabage collected until end due to callbacks.
	bool DotNet1803::mark_pixelline(int x, int y, int z, int pixWidth, int pixHeight, int pixDepth, unsigned int pixNum, double *pixels, E1803_power_callback power_callback, void *userData)
	{
		bool results = TestReturn(E1803_mark_pixelline(cardNum, x, y, z, pixWidth, pixHeight, pixDepth, pixNum, pixels, power_callback, userData));
		return results;
	}

	bool DotNet1803::set_pos(int x, int y)
	{
		return set_pos(x, y, 0, 0);
	}

	bool DotNet1803::set_pos(int x, int y, bool bLaserOn)
	{
		unsigned char laserOn = 0;
		if (bLaserOn)
			laserOn = 1;
		return set_pos(x, y, 0, laserOn);
	}

	bool DotNet1803::set_pos(int x, int y, int z, unsigned char laserOn)
	{
		bool results = TestReturn(E1803_set_pos(cardNum, x, y, z, laserOn));
		return results;
	}

	bool DotNet1803::set_matrix(double m11, double m12, double m21, double m22)
	{
		bool results = TestReturn(E1803_set_matrix(cardNum, m11, m12, m21, m22));
		return results;
	}

	bool DotNet1803::set_trigger_point()
	{
		bool results = TestReturn(E1803_set_trigger_point(cardNum));
		return results;
	}

	bool DotNet1803::release_trigger_point()
	{
		bool results = TestReturn(E1803_release_trigger_point(cardNum));
		return results;
	}

	bool DotNet1803::execute()
	{
		bool results = TestReturn(E1803_execute(cardNum));
		return results;
	}

	bool DotNet1803::execute(executemode mode)
	{
		return execute(mode, 0);
	}
	bool DotNet1803::execute(executemode mode, int timeout)
	{
		if (execute())
		{
			return executeMarkLoop(mode, timeout, false);
		}
		return false;
	}

	
	bool DotNet1803::executeMarkLoop(executemode mode, int timeout, bool useEvents)
	{
		if (mode == executemode::EXECMODE_IMMEDIATE)
			return true;
		Monitor::Enter(this);
		bCancelExecWait = false;
		Monitor::Exit(this);

		bool bDone = false;
		bool bCancel = false;
		DateTime dtNow = DateTime::Now;
		if (timeout == 0)
			timeout = executeDefaultTimeout;
		// Wait until marking has started
		do
		{
			int cardstate = get_card_state();
			if (cardstate & CSTATE_MARKING)
				bDone = true;
			else
				Sleep(executeWaitLoopSleep);
			Monitor::Enter(this);
			bCancel = bCancelExecWait;
			Monitor::Exit(this);

		} while (!bDone && !bCancel && DateTime::Now.Subtract(dtNow).TotalMilliseconds < timeout);
		if (bDone && !bCancel)
		{
			if (mode == executemode::EXECMODE_WAITUNTILSTARTMARK)
				return true;
			else if (bDone && mode == executemode::EXECMODE_FINISHMARK)
			{
				// Now we know it's marking, wait for it to be complete
				do
				{
					int cardstate = get_card_state();
					if ((cardstate & CSTATE_MARKING) == 0 && (cardstate & CSTATE_PROCESSING) == 0)
					{
						if (useEvents)
							executeMarkAsyncComplete(true);
						return true;
					}
					else
						Sleep(executeWaitLoopSleep);
					Monitor::Enter(this);
					bCancel = bCancelExecWait;
					Monitor::Exit(this);
				} while (!bCancel && DateTime::Now.Subtract(dtNow).TotalMilliseconds < timeout);
			}
			if (!bCancel)
			{
				if (useEvents)
					executeMarkAsyncComplete(true);
				return true;
			}
		}
		if (useEvents)
			executeMarkAsyncComplete(false);
		return false;
	}

	void DotNet1803::executeAsyncThread(Object^ timeout)
	{
		executeMarkLoop(executemode::EXECMODE_FINISHMARK, (int)timeout, true);
	}
	bool DotNet1803::executeMarkAsync()
	{
		return executeMarkAsync(executeDefaultTimeout);
	}

	bool DotNet1803::executeMarkAsync(int timeout)
	{
		if (execute())
		{
			Thread^ myThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &DotNet1803::executeAsyncThread));
			myThread->Start(timeout);
			return true;
		}
		return false;
	}

	bool bCancelExecWait = false;
	bool DotNet1803::stop_execution()
	{
		Monitor::Enter(this);
		bCancelExecWait = true;
		Monitor::Exit(this);
		bool results = TestReturn(E1803_stop_execution(cardNum));
		return results;
	}



	bool DotNet1803::halt_execution(bool bPause)
	{
		unsigned char hault = 0;
		if (bPause)
			hault = 1;
		return halt_execution(hault);
	}

	bool DotNet1803::halt_execution(unsigned char halt)
	{
		bool results = TestReturn(E1803_halt_execution(cardNum, halt));
		return results;
	}

	bool DotNet1803::delay(double delay)
	{
		bool results = TestReturn(E1803_delay(cardNum, delay));
		return results;
	}

	bool DotNet1803::dynamic_data(struct oapc_bin_struct_dyn_data *dynData)
	{
		bool results = TestReturn(E1803_dynamic_data(cardNum, dynData));
		return results;
	}

	unsigned int DotNet1803::get_startstop_state()
	{
		return E1803_get_startstop_state(cardNum);
	}

	unsigned int DotNet1803::get_card_state()
	{
		return E1803_get_card_state(cardNum);
	}

	unsigned int DotNet1803::get_card_info()
	{
		return E1803_get_card_info(cardNum);
	}

	unsigned int DotNet1803::get_head_state()
	{
		return get_head_state(0);
	}

	unsigned int DotNet1803::get_head_state(unsigned int flags)
	{
		return E1803_get_head_state(cardNum, flags);
	}

	bool DotNet1803::set_laser_timing(double frequency, double pulse)
	{
		bool results = TestReturn(E1803_set_laser_timing(cardNum, frequency, pulse));
		return results;
	}

	bool DotNet1803::set_laserb(double frequency, double pulse)
	{
		bool results = TestReturn(E1803_set_laserb(cardNum, frequency, pulse));
		return results;
	}

	bool DotNet1803::set_standby(double frequency, double pulse)
	{
		bool results = TestReturn(E1803_set_standby(cardNum, frequency, pulse));
		return results;
	}

	bool DotNet1803::set_fpk(double fpk, double yag3QTime)
	{
		bool results = TestReturn(E1803_set_fpk(cardNum, fpk, yag3QTime));
		return results;
	}

	int DotNet1803::get_free_space()
	{
		int p = get_free_space_primary();
		int s = get_free_space_secondary();
		if (p == -1 && s == -1)
			return -1;
		else if (p == -1)
			return -2;
		else if (s == -1)
			return -3;
		return p + s;
	}

	int DotNet1803::get_free_space_primary()
	{
		return get_free_space(E1803_FREE_SPACE_PRIMARY);
	}

	int DotNet1803::get_free_space_secondary()
	{
		return get_free_space(E1803_FREE_SPACE_SECONDARY);
	}

	int DotNet1803::get_free_space(int buffer)
	{
		return E1803_get_free_space(cardNum, buffer);
	}

	void DotNet1803::get_version(unsigned short *hwVersion, unsigned short *fwVersion)
	{
		E1803_get_version(cardNum, hwVersion, fwVersion);
	}

	unsigned short DotNet1803::get_version_hardware()
	{
		unsigned short hwVersion = 0;
		unsigned short fwVersion = 0;
		get_version(&hwVersion, &fwVersion);
		return hwVersion;
	}

	unsigned short DotNet1803::get_version_firmware()
	{
		unsigned short hwVersion = 0;
		unsigned short fwVersion = 0;
		get_version(&hwVersion, &fwVersion);
		return fwVersion;
	}

	unsigned int DotNet1803::get_library_version()
	{
		return E1803_get_library_version();
	}

	String^ DotNet1803::get_library_version_string()
	{
		unsigned int v = get_library_version();
		String^ ver = v.ToString();
		if (ver->Length >= 6)
		{
			return ver->Substring(0, 1) + "." + ver->Substring(1, 3) + "." + ver->Substring(4, 3);
		}
		return ver;
	}

	bool DotNet1803::write(unsigned int flags, unsigned int value)
	{
		bool results = TestReturn(E1803_write(cardNum, flags, value));
		return results;
	}

	bool DotNet1803::lp8_write(commandbehavior behavior, unsigned char value)
	{
		unsigned int flags = E1803_COMMAND_FLAG_STREAM;
		if (behavior == commandbehavior::Direct)
			flags = E1803_COMMAND_FLAG_DIRECT;
		return lp8_write(flags, value);
	}

	bool DotNet1803::lp8_write(unsigned int flags, unsigned char value)
	{
		bool results = TestReturn(E1803_lp8_write(cardNum, flags, value));
		return results;
	}

	bool DotNet1803::lp8_write_latch(unsigned char on, double delay1, unsigned char value, double delay2, double delay3)
	{
		bool results = TestReturn(E1803_lp8_write_latch(cardNum, on, delay1, value, delay2, delay3));
		return results;
	}

	bool DotNet1803::lp8_write_mo(unsigned char on)
	{
		bool results = TestReturn(E1803_lp8_write_mo(cardNum, on));
		return results;
	}

	bool DotNet1803::digi_write(commandbehavior behavior, unsigned int value, unsigned int mask)
	{
		unsigned int flags = E1803_COMMAND_FLAG_STREAM;
		if (behavior == commandbehavior::Direct)
			flags = E1803_COMMAND_FLAG_DIRECT;
		return digi_write(flags, value, mask);
	}

	bool DotNet1803::digi_write(unsigned int flags, unsigned int value, unsigned int mask)
	{
		bool results = TestReturn(E1803_digi_write(cardNum, flags, value, mask));
		return results;
	}

	unsigned int DotNet1803::digi_read()
	{
		return E1803_digi_read(cardNum);
	}

	bool DotNet1803::digi_wait(unsigned long value, unsigned long mask)
	{
		bool results = TestReturn(E1803_digi_wait(cardNum, value, mask));
		return results;
	}

	bool DotNet1803::digi_set_motf(double motfX, double motfY)
	{
		bool results = TestReturn(E1803_digi_set_motf(cardNum, motfX, motfY));
		return results;
	}

	bool DotNet1803::digi_set_motf_sim(double motfX, double motfY)
	{
		bool results = TestReturn(E1803_digi_set_motf_sim(cardNum, motfX, motfY));
		return results;
	}

	bool DotNet1803::digi_wait_motf(unsigned int flags, double dist)
	{
		bool results = TestReturn(E1803_digi_wait_motf(cardNum, flags, dist));
		return results;
	}

	bool DotNet1803::digi_set_mip_output(unsigned int value, unsigned int flags)
	{
		bool results = TestReturn(E1803_digi_set_mip_output(cardNum, value, flags));
		return results;
	}

	bool DotNet1803::digi_set_wet_output(unsigned int value, unsigned int flags)
	{
		bool results = TestReturn(E1803_digi_set_wet_output(cardNum, value, flags));
		return results;
	}

	bool DotNet1803::ana_write(unsigned int flags, unsigned short a)
	{
		bool results = TestReturn(E1803_ana_write(cardNum, flags, a));
		return results;
	}

	bool DotNet1803::uart_write(String^ sendData)
	{
		return(uart_write(sendData, sendData->Length));
	}

	bool DotNet1803::uart_write(String^ sendData, unsigned int in_length)
	{
		unsigned int sentLength = 0;
		return(uart_write(sendData, in_length, &sentLength));
	}

	bool DotNet1803::uart_write(String^ sendData, unsigned int in_length, unsigned int *sentLength)
	{
		StringToPointer stp(sendData);
		bool results = TestReturn(E1803_uart_write(cardNum, E1803_COMMAND_FLAG_UART1 | E1803_COMMAND_FLAG_DIRECT, stp.pointer, in_length, sentLength));
		stp.Free();
		if (results == true)
			*sentLength = in_length;
		return results;
	}

	// User is responsible for making sure memory is not garabage collected.
	bool DotNet1803::uart_read(char *recvData, unsigned int maxLength, unsigned int *receivedLength)
	{
		return uart_read((E1803_COMMAND_FLAG_UART1 | E1803_COMMAND_FLAG_DIRECT), recvData, maxLength, receivedLength);
	}
	// User is responsible for making sure memory is not garabage collected.
	bool DotNet1803::uart_read(unsigned int flags, char *recvData, unsigned int maxLength, unsigned int *receivedLength)
	{
		bool results = TestReturn(E1803_uart_read(cardNum, flags, recvData, maxLength, receivedLength));
		return results;
	}
	unsigned int DotNet1803::send_data(unsigned int flags, char *sendData, unsigned int length, unsigned int *sentLength)
	{
		return E1803_send_data(cardNum, flags, sendData, length, sentLength);
	}
	unsigned int DotNet1803::recv_data(unsigned int flags, char *recvData, unsigned int maxLength)
	{
		return E1803_recv_data(cardNum, flags, recvData, maxLength);
	}

	String^ DotNet1803::ReturnValueToString(int ret)
	{
		if (ret == E1803_OK) // operation could be finished successfully
			return("");
		else if (ret == E1803_ERROR_INVALID_CARD)
			return("Wrong/illegal card number specified");
		else if (ret == E1803_ERROR_NO_CONNECTION)
			return("Could not connect to card");
		else if (ret == E1803_ERROR_NO_MEMORY)
			return("Not enough memory available");
		else if (ret == E1803_ERROR_UNKNOWN_FW)
			return("Unknown/incompatible firmware version");
		else if (ret == E1803_ERROR)
			return("Unknown/unspecified error");
		else if (ret == E1803_ERROR_TRANSMISSION)
			return("Transmission of data failed");
		else if (ret == E1803_ERROR_FILEOPEN)
			return("Opening a file failed");
		else if (ret == E1803_ERROR_FILEWRITE)
			return("Writing data to a file failed");
		else if (ret == E1803_ERROR_INVALID_DATA)
			return("A function was called with invalid data or by using an operation mode where this function is not used/allowed");
		else if (ret == E1803_ERROR_UNKNOWN_BOARD)
			return("Trying to access a board that is not a scanner controller");
		else if (ret == E1803_ERROR_FILENAME)
			return("There is an error with the given filename (too long, too many subdirectories, illegal characters,...)");
		else if (ret == E1803_ERROR_NOT_SUPPORTED)
			return("The requested feature is not supported by the current firmware version");
		else if (ret == E1803_ERROR_NO_DATA_AVAILABLE)
			return("Tried to receive some data but there are none avilable yet");
		return("Unknown error " + ret);
	}

	bool DotNet1803::TestReturn(int ret)
	{
		ErrorDetail = ReturnValueToString(ret);
		if (ErrorDetail->Length == 0) // operation could be finished successfully
			return true;
		if (bOnErrorThrowException)
		{
			throw gcnew Exception(ErrorDetail);
		}
		return false;
	}
}
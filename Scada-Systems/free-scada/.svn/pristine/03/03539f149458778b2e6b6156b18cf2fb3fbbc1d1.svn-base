using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GsmComm.GsmCommunication;
using GsmComm.PduConverter;
using FreeSCADA.Interfaces;
using FreeSCADA.Common;

namespace ScadaShell.GSMLogger
{
    public class GsmLogger:IEventLogger
    {
        GsmCommMain _comm;
        bool _isConnected;
        GsmLoggerSettings _settings;
        public GsmLogger(IEventsSource source,GsmLoggerSettings settings)
        {
            _settings=settings;
            _comm= new GsmCommMain(settings.ComPort, GsmCommMain.DefaultBaudRate);
            
            _comm.PhoneConnected += new EventHandler(comm_PhoneConnected);
            _comm.PhoneDisconnected += new EventHandler(comm_PhoneDisconnected);
            try
            {
                _comm.Open();
            }
            catch (Exception e)
            {
                 throw new Exception("Unable to open the GsmLogger port");

            }

            source.EventChanged += new EventChanged(source_EventChanged);
        }

        void comm_PhoneDisconnected(object sender, EventArgs e)
        {
            Env.Current.Logger.LogInfo("Phone Disconnected");
            _isConnected = false;
        }

        void comm_PhoneConnected(object sender, EventArgs e)
        {

            Env.Current.Logger.LogInfo("Phone Connected");
            _isConnected = true;
            try
            {
                _comm.EnterPin(_settings.PinCode);
            }
            catch(Exception ex)
            {
                Env.Current.Logger.LogInfo("Pin entering error");
                _isConnected = false;
            }
        }

        void source_EventChanged(IEventDefinition ev, IEventState state)
        {
            if (!_isConnected&&!(state is FreeSCADA.Common.Events.States.StartedEventState))
                return;
            try
            {
                SmsSubmitPdu[] pdus = _settings.TelNumbers.Cast<TelNumElement>().Select(t => new SmsSubmitPdu(state.FormatText(ev.Text), t.TelNum, (byte)DataCodingScheme.GeneralCoding.Alpha16Bit)).ToArray();
                
                _comm.SendMessages(pdus);

            }
            catch (Exception e)
            {
                Env.Current.Logger.LogInfo(e.Message);
            }
        
        }

    }
}

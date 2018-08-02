using System;
using System.Collections.Generic;
using System.Threading;
using System.Xml;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;
using System.Linq;
using Modbus;
using System.Net;
using System.Xml.Serialization;
using System.IO.Ports;
using System.Net.Sockets;
using Modbus.Device;
using Modbus.Data;

namespace FreeSCADA.Communication.ModbusServer
{
    public class Plugin : ICommunicationPlug
    {
        public class mMap
        {
            public enum _mType
            {
                holding,
                input,
                coil,
                di
            }
            public string Channel
            {
                get;
                set;
            }

            public _mType mType
            {
                get;
                set;
            }
            public int mAdd
            {
                get;
                set;
            }
        }

        private IEnvironment environment;
        List<mMap> _channels_map;
        Thread channelUpdaterThread;
        Thread serversThread;
        bool isConnected;
        byte _unit_id=1;
        string _comport = "COM3";
        int _baudrate = 9600;
        DataStore _ds;
        ~Plugin()
        {
            if (IsConnected)
                Disconnect();
        }

        #region ICommunicationPlug Members
        public event EventHandler ChannelsChanged;


        public string Name
        {
            get { return StringConstants.PluginName; }
        }

        public string PluginId
        {
            get { return StringConstants.PluginId; }
        }

        public void Initialize(IEnvironment environment)
        {
            this.environment = environment;

            
            LoadSettings();
            _ds=DataStoreFactory.CreateDefaultDataStore();
            if (environment.Mode == EnvironmentMode.Designer)
            {
                ICommandContext context = environment.Commands.GetContext(PredefinedContexts.Communication);
                context.AddCommand(new PropertyCommand(this));
            }
        }

        public bool IsConnected
        {
            get { return isConnected; }
        }

        public bool Connect()
        {
            isConnected = true;
            channelUpdaterThread = new Thread(new ParameterizedThreadStart(ChannelUpdaterThreadProc));
            channelUpdaterThread.Start(this);
            serversThread = new Thread(new ParameterizedThreadStart(ServerProc));
            serversThread.Start(this);
            return IsConnected;
        }

        public void Disconnect()
        {
            isConnected = false;
            _run = false;
            channelUpdaterThread.Join();
            serversThread.Abort();

        }

        public IChannel[] Channels
        {
            get
            {
                throw new NotImplementedException();
            }
        }
        #endregion

        private static void ServerProc(object obj)
        {
            Plugin self = (Plugin)obj;
            /*try
            {
                
	            IPAddress address = new IPAddress(new byte[] { 0, 0, 0, 0 });

			    // create and start the TCP slave
			    TcpListener slaveTcpListener = new TcpListener(address, 502);
                slaveTcpListener.Start();
                ModbusSlave tcpSlave = ModbusTcpSlave.CreateTcp(self._unit_id, slaveTcpListener);
                tcpSlave.DataStore = self._ds;

                tcpSlave.Listen();
            }
            catch (Exception e)
            {
                Env.Current.Logger.LogError(e.Message);
            } */
            while (true)
            {
                var slavePort = new SerialPort(self._comport);
                ModbusSlave serialSlave=null;
                try
                {

                    slavePort.BaudRate = self._baudrate;
                    slavePort.DataBits = 8;
                    slavePort.Parity = Parity.None;
                    slavePort.StopBits = StopBits.One;
                    Env.Current.Logger.LogError(String.Format("try to open port {0}", self._comport));
                    slavePort.Open();

                    // create modbus slave
                    serialSlave = ModbusSerialSlave.CreateRtu(self._unit_id, slavePort);
                    serialSlave.DataStore = self._ds;
                    serialSlave.Listen();
                    
                }
                catch (Exception e)
                {
                    Env.Current.Logger.LogError(e.Message);
                    if (e is ThreadAbortException)
                        return;
                    if (e is System.IO.IOException)
                        return;
                    
                    if (serialSlave != null)
                        serialSlave.Dispose();
                    slavePort.Close();
                    
                    Thread.Sleep(2000);
                }
            }
        }

        
        private static void ChannelUpdaterThreadProc(object obj)
        {
            Plugin self = (Plugin)obj;
            _run = true;
            while (_run)
            {
                foreach (mMap m in self._channels_map)
                {

                    

                    if (m.mType == mMap._mType.input | m.mType == mMap._mType.holding)
                        self._ds.InputRegisters[m.mAdd+1] = (ushort)Convert.ToInt32(Convert.ToDouble(Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value) * 100);
                    if (m.mType == mMap._mType.di | m.mType == mMap._mType.coil)
                        self._ds.InputDiscretes[m.mAdd+1] = Convert.ToBoolean(Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value);



                    //if (m.mType == mMap._mType.holding)
                    //{
                    //    if (ms.ModbusDB.Single(x => x.UnitID == self._unit_id).HoldingRegisters[m.mAdd] != ms.ModbusDB.Single(x => x.UnitID == self._unit_id).InputRegisters[m.mAdd])
                    //        Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value = Convert.ToDouble(ms.ModbusDB.Single(x => x.UnitID == self._unit_id).HoldingRegisters[m.mAdd]) / 100;
                    //}

                    /*if (m.mType == mMap._mType.holding)
                        ms.ModbusDB.Single(x => x.UnitID == _unit_id).HoldingRegisters[m.mAdd] = Convert.ToUInt16(Convert.ToDouble(Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value) * 100);
                        
                    if (m.mType == mMap._mType.coil)
                    {
                        if (ms.ModbusDB.Single(x => x.UnitID == _unit_id).Coils[m.mAdd] != ms.ModbusDB.Single(x => x.UnitID == _unit_id).DiscreteInputs[m.mAdd])
                            Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value = Convert.ToBoolean(ms.ModbusDB.Single(x => x.UnitID == _unit_id).Coils[m.mAdd]);
                    }
                    if (m.mType == mMap._mType.di || m.mType == mMap._mType.coil)
                        ms.ModbusDB.Single(x => x.UnitID == _unit_id).DiscreteInputs[m.mAdd] = Convert.ToBoolean(Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value);
                    if (m.mType == mMap._mType.coil)
                        ms.ModbusDB.Single(x => x.UnitID == _unit_id).Coils[m.mAdd] = Convert.ToBoolean(Env.Current.CommunicationPlugins.GetChannel(m.Channel).Value);
                    */
                }

                Thread.Sleep(1000);
            }
        }
        public IEnvironment Environment
        {
            get { return environment; }
            set { Initialize(value); }
        }

        public void SaveSettings()
        {
            using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
            {
                if (ms.Length != 0)
                {
                    ms.SetLength(0);
                    ms.Seek(0, System.IO.SeekOrigin.Begin);
                }

                _channels_map =CreateChannelsMap();
                var doc = new XmlDocument();
                
                var mbsettings = doc.CreateElement("mbsettings");
                var map = doc.CreateElement("map");
                foreach (mMap m in _channels_map)
                {
                    var element = doc.CreateElement("map_element");
                    element.SetAttribute("type",Enum.GetName(typeof(mMap._mType),m.mType));
                    element.SetAttribute("add", m.mAdd.ToString());
                    element.SetAttribute("channel", m.Channel);
                    map.AppendChild(element);
                }
                                
                mbsettings.AppendChild(map);
                var settings = doc.CreateElement("settings");
                settings.SetAttribute("comport","com3");
                settings.SetAttribute("baud","9600");
                settings.SetAttribute("id","1");
                //settings
                mbsettings.AppendChild(settings);
                doc.AppendChild(mbsettings);
                var xmlDeclaration = doc.CreateXmlDeclaration("1.0", "utf-8", null);
                doc.InsertBefore(xmlDeclaration, doc.DocumentElement);
                doc.Save(ms);
                
                environment.Project["settings/" + StringConstants.PluginId + "_settings"] = ms;
            }
       } 

        public void LoadSettings()
        {
            _channels_map=new List<mMap>();
            using (System.IO.Stream ms = environment.Project["settings/" + StringConstants.PluginId + "_settings"])
            {
                if (ms == null || ms.Length == 0)
                    return;
                XmlDocument doc = new System.Xml.XmlDocument();
                try
                {

                    doc.Load(ms);
                    XmlNodeList  nodes = doc.GetElementsByTagName("map_element");
                    foreach (XmlNode node in nodes)
                    {
                        mMap element=new mMap();
                        element.Channel=node.Attributes["channel"].Value;
                        element.mType=(mMap._mType)Enum.Parse(typeof(mMap._mType),node.Attributes["type"].Value);
                        element.mAdd=int.Parse(node.Attributes["add"].Value);
                        _channels_map.Add(element);
                    }
                    
                    XmlNode  settings_node = doc.GetElementsByTagName("settings")[0];
                    _comport=settings_node.Attributes["comport"].Value;
                    _unit_id=byte.Parse(settings_node.Attributes["id"].Value);
                    _baudrate=int.Parse(settings_node.Attributes["baud"].Value);

                   
                }
                catch
                {
                    Env.Current.Logger.LogWarning("Error loading modbus settings. Applying defaults.");
                    _comport = "COM3";
                    _unit_id = 1;
                    _baudrate = 9600;
                    return;
                }



            }
            FireChannelChangedEvent();
        }



        void FireChannelChangedEvent()
        {
            if (ChannelsChanged != null)
                ChannelsChanged(this, new EventArgs());
        }

        public static bool _run { get; set; }



        List<mMap>  CreateChannelsMap()
        {
            List<mMap> channels_map = new List<mMap>();
            
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Pout.Value", mAdd = 0, mType = mMap._mType.holding});
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Tout.Value", mAdd = 1, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Pin.Value", mAdd = 2, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Tin.Value", mAdd = 3, mType = mMap._mType.holding});
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Auto.Pwork", mAdd = 4, mType = mMap._mType.holding});
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Q1.Value", mAdd = 5, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Q2.Value", mAdd = 6, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Q3.Value", mAdd = 7, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Analog.Q4.Value", mAdd = 8, mType = mMap._mType.holding });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Discrets.Alarm", mAdd = 0, mType = mMap._mType.di});
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran1.Opened", mAdd = 8, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran1.Closed", mAdd = 9, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran2.Opened", mAdd = 10, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran2.Closed", mAdd = 11, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran3.Opened", mAdd = 12, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran3.Closed", mAdd = 13, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran4.Opened", mAdd = 14, mType = mMap._mType.di });
            channels_map.Add(new mMap() { Channel = "opc_connection_plug.Zachist.Kran4.Closed", mAdd = 15, mType = mMap._mType.di });


            



            return channels_map;
        }
    }
}

		
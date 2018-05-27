/**************************************************************************
*                           MIT License
* 
* Copyright (C) 2016 Frederic Chaxel <fchaxel@free.fr>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;
using System.Xml;
using System.Data.SqlServerCe;

namespace EtherCATExplorer
{
    public static class DeviceDescrProvider
    {
        // Value seems to be sometimes in hex #x, sometimes in dec
        private static  UInt32 String_2_Uint(String s)
        {
            if (s[0] == '#')
                return Convert.ToUInt32(s.Substring(2, s.Length - 2), 16);
            else
                return Convert.ToUInt32(s);
        }

        // not in use
        public static System.Drawing.Bitmap GetImage(XmlDocument doc)
        {
            System.Drawing.Bitmap bmp= new System.Drawing.Bitmap(16, 14);
            XmlNode node = doc.SelectSingleNode("/EtherCATInfo/Descriptions/Groups/Group/ImageData16x14");

            int i = 0;
            for (int x=0;x<16;x++)
                for (int y = 0; y < 14; y++)
                {
                        string s = node.InnerText.Substring(i, 2);
                        int couleur = Convert.ToInt32(s, 16);
                        System.Drawing.Color c = System.Drawing.Color.FromArgb(255, couleur, couleur, couleur);

                        bmp.SetPixel(x, y, c);
                        i += 2;
                }

            return bmp;
        }

        // Entry point to parse the XML file
        public static string AddDeviceDescr(String DataBase, String XMLFilename)
        {
          if (!File.Exists(DataBase))
            {
                return "Error : No database file !";
            }

            string connectionString = "Data Source=" + DataBase;
            System.Data.SqlServerCe.SqlCeConnection con = new System.Data.SqlServerCe.SqlCeConnection(connectionString);

            // http://www.codeproject.com/Articles/21208/Store-or-Save-images-in-SQL-Server
            try
            {
                con.Open();

                XmlDocument doc = new XmlDocument();
                doc.Load(XMLFilename);

                // first Get vendor & slave Id

                XmlNode node = doc.SelectSingleNode("/EtherCATInfo/Vendor/Id");
                uint VendorId = String_2_Uint(node.InnerText);

                node = doc.SelectSingleNode("/EtherCATInfo/Descriptions/Devices/Device/Type");

                string device_name = node.InnerText;

                XmlAttribute at = node.Attributes["ProductCode"];
                uint ProductCode = String_2_Uint(at.Value);

                at = node.Attributes["RevisionNo"];
                uint ProductRev = String_2_Uint(at.Value);
                
                if (GetSlaveDatabaseId(con, VendorId, ProductCode, ProductRev) != -1)
                {
                    con.Close();
                    return "Slave Device already into the database";
                }
                
                int SlavePrimaryKey = GetPrimaryKeyValue(con, "SLAVES", "Id");

                string command = @"INSERT INTO SLAVES (Id, VendorId, DeviceId, Rev, Name) VALUES("
                      + SlavePrimaryKey.ToString() + "," + VendorId.ToString() + "," + ProductCode.ToString() + "," + ProductRev.ToString()+",'" + device_name + "');";

                SqlCeCommand cmd = new SqlCeCommand(command, con);
                cmd.ExecuteNonQuery();

                XmlNodeList nodes = doc.DocumentElement.SelectNodes("/EtherCATInfo/Descriptions/Devices/Device/Profile/Dictionary/Objects/Object");

                // Get all Index, it's simple
                foreach (XmlNode node2 in nodes)
                {
                    XmlNode n = node2.SelectSingleNode("Index");
                    uint Idx=String_2_Uint(n.InnerText);
                    n = node2.SelectSingleNode("Name");
                    string Name=n.InnerText;
                    n = node2.SelectSingleNode("Type");
                    string type=n.InnerText;
                    n = node2.SelectSingleNode("BitSize");
                    uint Bitsize=String_2_Uint(n.InnerText);

                    command = "INSERT INTO PDO_Dictionary VALUES(" +
                        SlavePrimaryKey.ToString() + "," + Idx.ToString() + "," + Bitsize.ToString() + ",'" + Name + "','" + type + "')";
                    cmd = new SqlCeCommand(command, con);
                    cmd.ExecuteNonQuery();
                }

                // Get all Datatype : SubIndex, more complex to do (multiple options)
                AddDeviceDataType(con, doc, SlavePrimaryKey);

                con.Close();
            }
            catch
            {
                return "Error with XML File, or Database";
            }
            return "Done";
        }

        private static void TryAddSingleType(SqlCeConnection con, XmlNode node, int DeviceDatabaseId)
        {
            XmlNode n = node.SelectSingleNode("Name");
            string ParentName = n.InnerText;

            XmlNodeList l = node.SelectNodes("SubItem");

            if ((l != null) && (l.Count != 0)) // It's a struct with content
            {
                foreach (XmlNode n2 in l)
                {
                    XmlNode n3;
                    int SubIdx = -1;
                    string NameSubIdx = "";
                    string type = "";
                    uint Bitsize = 0;
                    int RW = (int)PDOAccessLevel.Unknow;

                    try
                    {
                        n3 = n2.SelectSingleNode("SubIdx");
                        SubIdx = (int)String_2_Uint(n3.InnerText);
                    }
                    catch { } // another complex type (with ArrayInfo & BaseType), subidx not present

                    n3 = n2.SelectSingleNode("Name");
                    NameSubIdx = n3.InnerText;

                    n3 = n2.SelectSingleNode("Type");
                    type = n3.InnerText;

                    n3 = n2.SelectSingleNode("BitSize");
                    Bitsize = String_2_Uint(n3.InnerText);

                    n3 = n2.SelectSingleNode("Flags/Access");
                    if (n3.InnerText == "ro") RW = (int)PDOAccessLevel.ReadOnly;
                    if (n3.InnerText == "rw") RW = (int)PDOAccessLevel.ReadWrite;

                    string command = "INSERT INTO PDO_DataType VALUES(" +
                    DeviceDatabaseId.ToString() + ",'" + ParentName + "'," + SubIdx.ToString() + "," + Bitsize.ToString() + ",'" + NameSubIdx + "','" + type + "'," + RW.ToString() + ")" ;
                    SqlCeCommand cmd = new SqlCeCommand(command, con);
                    cmd.ExecuteNonQuery();
                }
            }
            if ((l != null) && (l.Count == 0)) // complex type (with ArrayInfo & BaseType)
            {
                /* basic type such as UDINT 
                     
                    <DataType>
                        <Name>UDINT</Name>
                        <BitSize>32</BitSize>
                    </DataType>
                     
                 or Array description
                      
                        <DataType>
                            <Name>ARRAY[0..26] OF USINT</Name>
                            <BaseType>USINT</BaseType>
                            <BitSize>208</BitSize>
                            <ArrayInfo>
                                <LBound>0</LBound>
                                <Elements>27</Elements>
                            </ArrayInfo>
                        </DataType>
                 */
                uint SubIdx = 0, Bitsize = 0;
                string NameSubIdx = "";
                string basetype = "";

                XmlNode n2;
                n2 = node.SelectSingleNode("BitSize");
                Bitsize = String_2_Uint(n2.InnerText);

                n2 = node.SelectSingleNode("BaseType");
                if ((n2 != null) && (!ParentName.Contains("ARRAY"))) // do not redefine array
                {
                    basetype = n2.InnerText;

                    try
                    {
                        n2 = node.SelectSingleNode("ArrayInfo/LBound");
                        string Lbound = n2.InnerText;
                        n2 = node.SelectSingleNode("ArrayInfo/Elements");
                        uint Elements = String_2_Uint(n2.InnerText);
                        basetype = "ARRAY[" + Lbound + ".." + (Elements - 1).ToString() + "] OF " + basetype;
                    }
                    catch { }
                }

                
                string command = "INSERT INTO PDO_DataType VALUES(" +
                DeviceDatabaseId.ToString() + ",'" + ParentName + "'," + SubIdx.ToString() + "," + Bitsize.ToString() + ",'" + NameSubIdx + "','" + basetype + "',0)";
                SqlCeCommand cmd = new SqlCeCommand(command, con);
                cmd.ExecuteNonQuery();
            }

        }
        // Add all the Data Type present in the XML file
        private static void AddDeviceDataType(SqlCeConnection con, XmlDocument doc, int DeviceDatabaseId)
        {
            XmlNodeList nodes = doc.DocumentElement.SelectNodes("/EtherCATInfo/Descriptions/Devices/Device/Profile/Dictionary/DataTypes/DataType");

            foreach (XmlNode node in nodes)
                try
                {
                    TryAddSingleType(con, node, DeviceDatabaseId);
                }
                catch { }
            
        }

        // find the PrimaryKey
        public static int GetSlaveDatabaseId(SqlCeConnection con, UInt32 VendorId, UInt32 deviceId, UInt32 Rev)
        {

            string command = @"SELECT id FROM SLAVES WHERE VendorId="
                     + VendorId.ToString() + " AND DeviceId=" + deviceId.ToString() + " AND Rev=" + Rev.ToString();

            SqlCeCommand cmd = new SqlCeCommand(command, con);
            SqlCeDataReader reader=cmd.ExecuteReader();

            if (reader.Read() == false) return -1;

            return Convert.ToInt32(reader[0]);
        }
       
        // Don't like Auto increment, need to read back to get the value
        public static int GetPrimaryKeyValue(SqlCeConnection con, string Table, string ColName)
        {
            try
            {
                string command = "SELECT MAX(" + ColName + ") FROM " + Table;
                SqlCeCommand cmd = new SqlCeCommand(command, con);
                SqlCeDataReader reader = cmd.ExecuteReader();

                if (reader.Read())
                    return Convert.ToInt32(reader[0])+1;
            }
            catch { }
            return 1;
        }

        // Creates the SQLServer CE database file with the 3 Tables
        public static void CreateDatabase(String FName)
        {
            File.Delete(FName);

            try
            {
                string connectionString = "Data Source=" + FName;
                SqlCeEngine en = new SqlCeEngine(connectionString);
                en.CreateDatabase();

                SqlCeConnection con = new SqlCeConnection(connectionString);

                con.Open();

                string command = @"CREATE TABLE SLAVES(" +
                                        "Id INTEGER PRIMARY KEY, VendorId BIGINT, DeviceId INTEGER, Rev INTEGER, Name NVARCHAR(255));";

                SqlCeCommand cmd = new SqlCeCommand(command, con);
                cmd.ExecuteNonQuery();

                // SalveID is the Primary Ket in Slaves table, not an identifier of a salve on the network 
                command = @"CREATE TABLE PDO_Dictionary(" +
                                        "SlaveId INTEGER,  Idx INTEGER, BitSize INTEGER , Name NVARCHAR(255),Type NVARCHAR(50));";
                cmd = new SqlCeCommand(command, con);
                cmd.ExecuteNonQuery();

                command = @"CREATE TABLE PDO_DataType(" +
                        "SlaveId INTEGER, ParentType NVARCHAR(50), SubIdx INTEGER, BitSize INTEGER,  Name NVARCHAR(255), Type NVARCHAR(50), RW INTEGER);";
                cmd = new SqlCeCommand(command, con);
                cmd.ExecuteNonQuery();

                con.Close();
            }

            catch
            {
            }
        }
    }

    public enum PDOAccessLevel :int { Unknow, ReadOnly, ReadWrite };

    // On for each object in the dictionnary
    // and SubIdx is for attribut
    public class PDODictionaryEntry
    {
        public int Index;
        public uint Bitsize;
        public string Name;
        public string type;
        public PDOAccessLevel Access; 
        public List<PDODictionaryEntry> SubIdx;

        public PDODictionaryEntry(int Index, uint Bitsize, string Name, string type, PDOAccessLevel ro)
        {
            this.Index = Index;
            this.Bitsize = Bitsize;
            this.Name = Name;
            this.type = type;
            this.Access = ro;
        }

        public static List<PDODictionaryEntry> GetDictionary(String DatabaseName, EthCATDevice slave)
        {

            string connectionString = "Data Source=" + DatabaseName;
            System.Data.SqlServerCe.SqlCeConnection con = new System.Data.SqlServerCe.SqlCeConnection(connectionString);
            try
            {
                con.Open();

                int databaseid = DeviceDescrProvider.GetSlaveDatabaseId(con, slave._ManufacturerId, slave._TypeId, slave.Revision);
                if (databaseid == -1) return null;

                List<PDODictionaryEntry> ret = new List<PDODictionaryEntry>();

                string command = "SELECT * FROM PDO_Dictionary WHERE SlaveId=" + databaseid.ToString();
                SqlCeCommand cmd = new SqlCeCommand(command, con);
                SqlCeDataReader reader = cmd.ExecuteReader();

                while (reader.Read())
                {
                    string FinalType = GetFinalType(con, databaseid, (string)reader[4]);
                    // At this level RW is unknow (not in the XML file)
                    PDODictionaryEntry e = new PDODictionaryEntry(Convert.ToInt32(reader[1]), Convert.ToUInt32(reader[2]),
                                                                    (string)reader[3], FinalType, PDOAccessLevel.Unknow);
                    ret.Add(e);
                    e.AddSubIndexData(con, databaseid);
                }
                con.Close();

                return ret;
            }
            catch
            {
                Trace.WriteLine("Database content Error");
            }

            return null;

        }

        private static string GetFinalType(SqlCeConnection con, int databaseid, String BaseType)
        {
            string command = "SELECT TYPE FROM PDO_DataType WHERE SlaveId=" + databaseid.ToString() + " AND NAME='' AND ParentType='" + BaseType + "'";
            SqlCeCommand cmd = new SqlCeCommand(command, con);
            SqlCeDataReader reader = cmd.ExecuteReader();

            if (reader.Read())
            {
                string s = (string)reader[0];
                if (s != "")
                    return s;
            }
            return BaseType;
        }

        public void AddSubIndexData(SqlCeConnection con, int databaseid)
        {
            string command = "SELECT * FROM PDO_DataType WHERE SlaveId=" + databaseid.ToString() + " AND ParentType='" + this.type + "'";
            SqlCeCommand cmd = new SqlCeCommand(command, con);
            SqlCeDataReader reader = cmd.ExecuteReader();

            while (reader.Read())
            {
                if ((string)reader[4] != "") // It a base type such as UDINT or a array description
                {
                    if (SubIdx == null) SubIdx = new List<PDODictionaryEntry>(); // at least on entry

                    string FinalType = GetFinalType(con, databaseid, (string)reader[5]);

                    SubIdx.Add(new PDODictionaryEntry(Convert.ToInt32(reader[2]), Convert.ToUInt32(reader[3]), (string)reader[4], FinalType, (PDOAccessLevel)Convert.ToInt32(reader[6])));
                }
            }
        }

        public override string ToString()
        {
            return Name;
        }
    }
}

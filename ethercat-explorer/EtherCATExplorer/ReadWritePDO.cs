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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using SOEM;

namespace EtherCATExplorer
{
    public partial class ReadWritePDO : Form
    {
        EthCATDevice slave;

        public ReadWritePDO(EthCATDevice slave)
        {
            this.slave = slave;
            InitializeComponent();
            lbldevice.Text = slave.ToString();

            // Get the dictionnary if exist
            List<PDODictionaryEntry> dico = PDODictionaryEntry.GetDictionary(Properties.Settings.Default.DatabaseFile, slave);

            // Fill the Treeview with it
            if (dico != null)
            {
                foreach (PDODictionaryEntry entry in dico)
                {
                    string name = entry.ToString();
                    if (Properties.Settings.Default.ShowSDOIdx)
                        name=name+" (x"+entry.Index.ToString("X4")+")";
                    TreeNode tn = new TreeNode(name);
                    tn.Tag = entry;
                    tn.SelectedImageIndex = tn.ImageIndex = Type2ico(entry.type);                  
                    
                    dicoTree.Nodes.Add(tn);

                    int ro = 0, rw = 0;
 
                    if (entry.SubIdx != null)
                    {
                        foreach (PDODictionaryEntry entry2 in entry.SubIdx)
                        {
                            TreeNode tn2 = new TreeNode(entry2.ToString());
                            tn2.Tag = entry2;
                            tn2.ToolTipText = entry2.type;
                            tn2.SelectedImageIndex = tn2.ImageIndex = Type2ico(entry2.type);

                            if (entry2.Access == PDOAccessLevel.ReadOnly)
                            {
                                tn2.ForeColor = Properties.Settings.Default.ReadOnlyAttributColor;
                                ro++;
                            }
                            if (entry2.Access == PDOAccessLevel.ReadWrite)
                            {
                                tn2.ForeColor = Properties.Settings.Default.ReadWriteAttributColor;
                                rw++;
                            }
                            tn.Nodes.Add(tn2);
                        }
                    }
                    else
                        tn.ToolTipText = entry.type;

                    if ((ro != 0) && (rw == 0)) tn.ForeColor = Properties.Settings.Default.ReadOnlyAttributColor;
                    if ((rw != 0) && (ro == 0)) tn.ForeColor = Properties.Settings.Default.ReadOnlyAttributColor;
                }
            }
        }

        // don't now if it's OK : FIXME
        // found at http://www.quarcservice.com/ReleaseNotes/files/ethercat_read_block.html
        static string[] EtherCAT_Number = new string[] { "BYTE", "WORD", "DWORD", "SINT", "USINT", "INT", "UINT", "DINT", "UDINT",
                                                        "LINT", "ULINT", "REAL", "LREAL" };
        static int[] EtherCAT_IntNumber_Size = new int[] { 1, 2, 4, 1, 1, 2, 2, 4, 4, 8, 8 };

        static string[] SignedEtherCAT_Int = new string[] { "SINT", "INT", "DINT", "LINT" };
        static string[] EtherCAT_DateTime = new string[] {"TIME", "TIME_OF_DAY", "TOD", "DATE",
                                                        "DATE_AND_TIME", "DT" };


        private int Type2ico(string type)
        {
          
            if ((type == "BOOL")||(type=="BIT")) return 1;
            if (EtherCAT_Number.Contains(type)) return 2;
            if (type.Contains("STRING")) return 3;
            if (EtherCAT_DateTime.Contains(type)) return 4;
            return 0;         
        }

        private string ValueDecode(byte[] buf, ref int offset, int size, String type)
        {

            int beginoffset = offset;
            try
            {
                if (type == "UNICODE_STRING")
                {
                    offset += size;
                    return Encoding.Unicode.GetString(buf, beginoffset, size);
                }

                // STRING & OCTET_STRING (don't now the difference)
                // maybe ASCII & ISO 8859 : FIXME
                if (type == "STRING")
                {
                    offset += size;
                    return Encoding.ASCII.GetString(buf, beginoffset, size);
                }

                // type == "OCTET_STRING" undecoded, byte value already displayed

                // Never tested, FIXME
                if ((type == "TIME") || (type == "TIME_OF_DAY"))
                {
                    offset += 4;
                    uint t = BitConverter.ToUInt32(buf, beginoffset);
                    DateTime dt = new DateTime(1,1,1970);
                    if (type=="TIME")
                        return dt.AddMilliseconds(t).ToString("hh:mm:ss.fff");
                    else
                        return dt.AddSeconds(t).ToString("hh:mm:ss");
                }
                // Never tested, FIXME
                if ((type == "DATE") || (type == "DATE_AND_TIME"))
                {
                    offset += 4;
                    uint t = BitConverter.ToUInt32(buf, beginoffset);
                    DateTime dt = new DateTime(1, 1, 1970);
                    if (type == "DATE")
                        return dt.AddMilliseconds(t).ToString("d");
                    else
                        return dt.AddSeconds(t).ToString("G");
                }

                // to be tested
                if (type == "REAL")
                {
                    offset += 4;
                    return BitConverter.ToSingle(buf, beginoffset).ToString();
                }
                if (type == "LREAL")
                {
                    offset += 8;
                    return BitConverter.ToDouble(buf, beginoffset).ToString();
                }

                if (type == "BOOL")
                {
                    offset += 1;
                    return ((buf[beginoffset] & 1) == 1).ToString();
                }
                if (type.Contains("BIT"))
                {
                    offset += 1;
                    return Convert.ToString(buf[beginoffset], 2);
                }

                if (type == "ULINT")
                {
                    offset += 8;
                    return BitConverter.ToUInt64(buf, beginoffset).ToString();
                }

                // signed or not
                if (EtherCAT_Number.Contains(type))
                {
                    int singleSize = 0;
                    for (int i = 0; i < EtherCAT_Number.Length; i++)
                    {
                        if (EtherCAT_Number[i] == type)
                        {
                            singleSize = EtherCAT_IntNumber_Size[i];
                            break;
                        }
                    }

                    offset += singleSize;

                    Byte[] bufInt64 = new byte[8];
                    byte fill = 0;

                    if ((SignedEtherCAT_Int.Contains(type)) && (buf[size - 1] > 127))
                        fill = 255;

                    for (int i = 0; i < 8; i++) bufInt64[i] = fill;

                    Array.Copy(buf, beginoffset, bufInt64, 0, singleSize);

                    Int64 V = BitConverter.ToInt64(bufInt64, 0);

                    return V.ToString();
                }
            }
            catch { }

            offset += size;
            return "Undecoded";
        }
        public string ValueDecode(byte[] buf, int size, String type)
        {

            int offset = 0;

            if (!type.Contains("ARRAY"))
                return ValueDecode(buf, ref offset, size, type);
            else
            {
                try
                {
                    StringBuilder sb = new StringBuilder("{ ");
                    string basetype = type.Split(' ')[2];

                    while (offset < size)
                    {
                        sb.Append(ValueDecode(buf, ref offset, size, basetype));
                        sb.Append(" - ");
                    }
                    sb.Remove(sb.Length - 3, 3);
                    sb.Append(" }");
                    return sb.ToString();

                }
                catch { }
            }
            return "Undecoded";
        }
        
        private void btRead_Click(object sender, EventArgs e)
        {
            PdoValues.Text = "";
            PdoValueDecodedLabel.Text = "PDO Values Decoded";
            PdoValueDecoded.Text = "Undecoded";

            int size;
            byte[] res=slave.ReadPDO((int)IndexValue.Value, (int)SubIndexValue.Value, out size);

            if (res != null)
            {
                StringBuilder sb = new StringBuilder(size * 3);
                for (int i = 0; i < size; i++)
                {
                    sb.Append(res[i].ToString("X2"));
                    sb.Append(' ');
                }
                PdoValues.Text = sb.ToString();
                if (dicoTree.SelectedNode != null)
                {
                    string type = (dicoTree.SelectedNode.Tag as PDODictionaryEntry).type;
                    PdoValueDecodedLabel.Text = "PDO Values Decoded as "+ type;

                    if (!(type.Contains("ARRAY")&&(SubIndexValue.Value==-1)))
                        PdoValueDecoded.Text = ValueDecode(res, size, type);
                }
            }
        }

        private void btWrite_Click(object sender, EventArgs e)
        {
            List<byte> values = new List<byte>();
            try
            {
                String[] ValStr = PdoValues.Text.Split(new char[] {' ', '\r', '\n'});

                foreach (String s in ValStr)
                    if (s.Length==2)
                        values.Add( Convert.ToByte(s, 16));
            }
            catch
            {
                Trace.TraceWarning("Error when parsing the Values, you must respect the format");
                return;
            }

            byte[] buff=values.ToArray();
            slave.WritePDO((int)IndexValue.Value, (int)SubIndexValue.Value, buff.Length, buff);
        }

        private void dicoTree_AfterSelect(object sender, TreeViewEventArgs e)
        {
            PDODictionaryEntry selected = (PDODictionaryEntry)e.Node.Tag;
            if (e.Node.Parent == null)
            {
                IndexValue.Value = selected.Index;
                SubIndexValue.Value = -1;
            }
            else
            {
                SubIndexValue.Value = selected.Index;
                selected = (PDODictionaryEntry)e.Node.Parent.Tag;
                IndexValue.Value = selected.Index;
            }
        }

        private void dicoTree_DoubleClick(object sender, EventArgs e)
        {
            btRead_Click(null, null);
        }

        private void btExpand_Click(object sender, EventArgs e)
        {
            if (dicoTree.Nodes.Count == 0) return;
            dicoTree.ExpandAll();
            dicoTree.Nodes[0].EnsureVisible();
        }

        private void btUnExpand_Click(object sender, EventArgs e)
        {
            if (dicoTree.Nodes.Count == 0) return;
            dicoTree.CollapseAll();
            dicoTree.Nodes[0].EnsureVisible();
        }

        private void ReadWritePDO_Load(object sender, EventArgs e)
        {

        }
    }
}

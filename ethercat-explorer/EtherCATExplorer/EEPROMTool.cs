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
using System.IO;
using System.Diagnostics;
using SOEM;

namespace EtherCATExplorer
{
    public partial class EEPROMTool : Form
    {
        EthCATDevice slave;
        bool AsciiMode = false;
        byte[] CurrentBuf = null;

        public EEPROMTool(EthCATDevice slave)
        {

            InitializeComponent();

            if (slave == null)
            {
                memoryToolStripMenuItem.Enabled = false;
                Lbldevice.Text = "Offline mode";
            }
            else
            {
                this.slave = (EthCATDevice)slave;
                Lbldevice.Text = slave.ToString();
                
                writeToolStripMenuItem.Enabled=Properties.Settings.Default.EEPROMWriteEnable;
            }
        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void Buf2Display(byte[] buf, Boolean AsciiMode=true)
        {
            try
            {
                int lenght = buf.Length;

                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < lenght; i += 8)
                {
                    sb.Append(i.ToString("X4"));
                    sb.Append(" : ");
                    for (int j = 0; j < 8; j++)
                    {
                        if (((i + j) < lenght))
                        {
                            if (AsciiMode)
                            {
                                char c = (char)buf[i + j];

                                if (!char.IsControl(c))
                                    sb.Append(c);
                                else
                                    sb.Append('.');
                            }
                            else
                                sb.Append(buf[i + j].ToString("X2"));

                            sb.Append(" ");
                        }
                    }
                    sb.Append(Environment.NewLine);
                }
                Mem.Text = sb.ToString();
            }
            catch
            {
                MessageBox.Show("Error with the file", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private byte[] Display2Buf()
        {
            List<byte> values = new List<byte>();

            String[] ValStr = Mem.Text.Split(new char[] { ' ', '\r', '\n' });

            foreach (String s in ValStr)
                if (s.Length == 2)
                    values.Add(Convert.ToByte(s, 16));

            return values.ToArray();
        }
        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                OpenFileDialog dlg = new OpenFileDialog();
                dlg.Filter = "bin|*.bin";
                if (dlg.ShowDialog(this) != System.Windows.Forms.DialogResult.OK) return;

                CurrentBuf = File.ReadAllBytes(dlg.FileName);

                Buf2Display(CurrentBuf, AsciiMode);
            }
            catch
            {
                MessageBox.Show("Error with the file", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (AsciiMode)
            {
                MessageBox.Show("Content must be displayed in Hex mode", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            try
            {
                SaveFileDialog dlg = new SaveFileDialog();
                dlg.Filter = "bin|*.bin";
                dlg.CreatePrompt = true;
                if (dlg.ShowDialog(this) != System.Windows.Forms.DialogResult.OK) return;

                CurrentBuf = Display2Buf();

                File.WriteAllBytes(dlg.FileName, CurrentBuf);

                MessageBox.Show("done", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch
            {
                MessageBox.Show("Error with the file", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void readToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CurrentBuf = new byte[128*256]; // get a big buffer
            int size = 0;

            // Read first 128 bytes
            int ret = SoemInterrop.EEprom_Read(1, 0, 128, CurrentBuf);

            if (ret > 0)
            {
                // size is at 0x7C, 0x7D. Here I only get the less-significant byte
                // up to 32Ko of eeprom is certainly never implemented
                // FIXME if needed
                size = (CurrentBuf[0x7c]+1)*128; // Adjustment

                progress.Maximum = size;
                progress.Visible = true;

                for (int s = 128; s < size; s += 128)
                {
                    ret = SoemInterrop.EEprom_Read(1, s, 128, CurrentBuf);

                    if (ret <= 0)
                    {
                        size = 0;
                        break;
                    }
                    progress.Value = s+128;
                    Application.DoEvents();
                }
            }

            progress.Visible = false;

            if (size == 0)
            {
                Trace.WriteLine("EEPROM read error");
                Mem.Text = "";
                CurrentBuf = null;
            }
            else
            {
                Array.Resize(ref CurrentBuf, size);
                Buf2Display(CurrentBuf, AsciiMode);
            }

        }

        private void writeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (AsciiMode)
            {
                MessageBox.Show("Content must be displayed in Hex mode", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            if (MessageBox.Show("Are you Really sure to do that", "EtherCAT Explorer", MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
                != DialogResult.OK) return;
            
            MessageBox.Show("Not yet implemented", "EtherCAT Explorer", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            AsciiMode = !AsciiMode;
            if (AsciiMode)
                lblmode.Text="Ascii Content :";
            else
                lblmode.Text = "Hex Content :";

            if (CurrentBuf!=null)
                Buf2Display(CurrentBuf, AsciiMode);
        }

    }
}

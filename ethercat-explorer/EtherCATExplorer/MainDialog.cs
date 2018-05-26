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
using System.IO;
using System.Threading;
using SOEM;

namespace EtherCATExplorer
{
    public partial class MainDialog : Form
    {
        public MainDialog()
        {
            InitializeComponent();
            Trace.Listeners.Add(new MyTraceListener(this));
            Size s = Properties.Settings.Default.GUI_FormSize;
            if (s != Size.Empty)
                this.Size = s;
            this.WindowState = Properties.Settings.Default.GUI_State;
            
            if (Properties.Settings.Default.RefreshStatePeriod>0)
                tmrRefreshState.Interval = Properties.Settings.Default.RefreshStatePeriod*1000;

            tmrInputFlow.Interval = Math.Max(100, Properties.Settings.Default.InputRefreshPeriod);

           // writeStateToolStripMenuItem.DropDown.AutoClose = false;
        }

        public static List<Tuple<String, String, String>> GetAvailableInterfaces()
        {
            List<Tuple<String, String, String>> ips = new List<Tuple<String, String, String>>();
            System.Net.NetworkInformation.NetworkInterface[] interfaces = System.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces();
            foreach (System.Net.NetworkInformation.NetworkInterface inf in interfaces)
            {
                if (!inf.IsReceiveOnly && inf.OperationalStatus == System.Net.NetworkInformation.OperationalStatus.Up && inf.SupportsMulticast && inf.NetworkInterfaceType != System.Net.NetworkInformation.NetworkInterfaceType.Loopback)
                {
                    ips.Add(new Tuple<string,string,string>(inf.Description, inf.Name, inf.Id));
                }
            }
            return ips;
        }
        public static int SlaveStatetoIco(SlaveState s)
        {
            if (s == SlaveState.Operational) return 0;
            if (s == SlaveState.Unknow) return 2;
            return 1;
        }

        private void MainDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                Properties.Settings.Default.GUI_FormSize = this.Size;
                Properties.Settings.Default.GUI_State = this.WindowState;
                Properties.Settings.Default.Save();
                if (openInterfaceToolStripMenuItem.Enabled ==false)
                    SoemInterrop.StopActivity();
            }
            catch { }
        }

        private void devicesTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            try
            {
                if (e.Node.Tag is EthCATDevice)
                {
                    SoemInterrop.RefreshSlavesState();

                    EthCATDevice slave = (EthCATDevice)e.Node.Tag;
                    slave.Refresh();

                    e.Node.SelectedImageIndex = e.Node.ImageIndex = SlaveStatetoIco(slave.State);

                    propertyGrid.SelectedObject = slave;
                    propertyGrid.ExpandAllGridItems();

                    propertyInput.SelectedObjects = new object[] { slave.InputData };
                    propertyInput.ExpandAllGridItems();

                    propertyOutput.SelectedObjects = new object[] { slave.OutputData };
                    propertyOutput.ExpandAllGridItems();

                    PDOToolStripMenuItem.Enabled = slave.MailboxProtocol.Contains(MailBoxProto.CoE.ToString());

                }
                else
                    propertyGrid.SelectedObject = null;
            }
            catch { }

        }

        private void tmrRefreshState_Tick(object sender, EventArgs e)
        {
            tmrStart.Enabled = false;

            SoemInterrop.RefreshSlavesState();
            foreach (TreeNode tn in devicesTreeView.Nodes)
            {
                EthCATDevice slave = tn.Tag as EthCATDevice;
                SlaveState s = slave.State;
                slave.Refresh();
                if (s != slave.State)
                {
                    int img = SlaveStatetoIco(slave.State);
                    tn.ImageIndex = tn.SelectedImageIndex = img;
                    devicesTreeView.Refresh();
                }

                if (tn == devicesTreeView.SelectedNode)
                {
                    propertyGrid.Refresh();
                    propertyInput.Refresh();
                }
            }
        }

        private void tmrInputFlow_Tick(object sender, EventArgs e)
        {
            if (devicesTreeView.SelectedNode == null) return;
            EthCATDevice slave = devicesTreeView.SelectedNode.Tag as EthCATDevice;
            slave.ReadInput();
            propertyInput.Refresh();
        }

        private void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (devicesTreeView.SelectedNode.Tag is EthCATDevice)
            {
                (devicesTreeView.SelectedNode.Tag as EthCATDevice).WriteOutput();
                Trace.WriteLine("Write");
            }
            else
                Trace.WriteLine("Modifications are not taken into account at this level");
        }
        // Menu
        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            string readme_path = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(typeof(MainDialog).Assembly.Location), "README.txt");
            try { System.Diagnostics.Process.Start(readme_path); }
            catch { }
        }
        // Menu
        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show(this, "EtherCAT Explorer - EtherCATExplorer\nVersion Beta " + this.GetType().Assembly.GetName().Version + "\nBy Frederic Chaxel - Copyright 2016\n" +
            "\nReferences:\n\t http://openethercatsociety.github.io/" +
            "\n\t https://www.ethercat.org/"+
            "\n\t http://sourceforge.net/projects/EnIPExplorer" +
            "\n\t http://sourceforge.net/projects/yetanotherbacnetexplorer/" +
            "\n\t http://www.famfamfam.com/" +
            "\n\t http://www.jrsoftware.org/isinfo.php"
            , "About", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        // Menu
        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
        // Menu
        private void openInterfaceToolStripMenuItem_Click(object sender, EventArgs e)
        {

            List<Tuple<String, String, String>> interfaces = GetAvailableInterfaces();

            var Input =
                  new GenericInputBox<ComboBox>("Local Interface", "Interfaces",
                       (o) =>
                       {
                           foreach (Tuple<String, String, String> it in interfaces)
                               o.Items.Add(it.Item1);

                           o.Text = o.Items[0].ToString();
                       }, 1.7);

            DialogResult res = Input.ShowDialog();

            if (res != DialogResult.OK) return;
            String userinput = Input.genericInput.Text;

            Cursor Memcurs = this.Cursor;
            this.Cursor = Cursors.WaitCursor;

            Trace.WriteLine("Openning interface " + userinput);
            Application.DoEvents();

            foreach (Tuple<String, String, String> it in interfaces)
            {
                if (it.Item1 == userinput)
                {
                    String PcapInterfaceName = "\\Device\\NPF_" + it.Item3;
                    int NbSlaves = SoemInterrop.StartActivity(PcapInterfaceName, Properties.Settings.Default.DelayUpMs);

                    if (NbSlaves > 0)
                    {
                        for (uint i = 0; i < NbSlaves; i++)
                        {
                            EthCATDevice slave = new EthCATDevice(i + 1);

                            int img = SlaveStatetoIco(slave.State);

                            TreeNode tn = new TreeNode(slave.ToString(), img, img);
                            tn.Tag = slave;
                            devicesTreeView.Nodes.Add(tn);
                        }
                        openInterfaceToolStripMenuItem.Enabled = false;
                        if (tmrRefreshState.Interval >= 1000)
                            tmrRefreshState.Enabled = true;

                        SoemInterrop.Run();

                        tmrStart.Enabled = true;
                        tmrInputFlow.Enabled = true;
                    }
                    else
                        Trace.WriteLine("No slave behind this Interface");
                }
            }

            this.Cursor = Memcurs;
        }
        // Menu
        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SettingsDialog dlg = new SettingsDialog();
            dlg.SelectedObject = Properties.Settings.Default;
            dlg.ShowDialog(this);
        }
        // Menu
        private void readPDOToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (devicesTreeView.SelectedNode == null) return;

            EthCATDevice slave = devicesTreeView.SelectedNode.Tag as EthCATDevice;
            new ReadWritePDO(slave).ShowDialog();

        }
        // Menu
        private void stateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (devicesTreeView.SelectedNode == null) return;
            
            EthCATDevice slave=devicesTreeView.SelectedNode.Tag as EthCATDevice;

            if (sender == stateInitToolStripMenuItem)
                slave.WriteState(SlaveState.Init);
            if (sender==stateBootToolStripMenuItem)
                slave.WriteState(SlaveState.Boot);
            if (sender == stateOperationalToolStripMenuItem)
                slave.WriteState(SlaveState.Operational);
            if (sender == statePreOperationalToolStripMenuItem)
                slave.WriteState(SlaveState.PreOperational);
            if (sender == stateSafeOperationalToolStripMenuItem)
                slave.WriteState(SlaveState.SafeOperational);
            if (sender == sendAckToolStripMenuItem)
                slave.WriteState(SlaveState.Ack);
            if (sender == sendReconfigureToolStripMenuItem)
                slave.Reconfigure();
            // Actual state with bit Ack 1
            if (sender == sendAckToolStripMenuItem)
                slave.WriteState((SlaveState) ((uint)slave.State | (uint)SlaveState.Ack));

            Thread.Sleep(200); // It could take time for the device, so we are waitting a few before reading back

            tmrRefreshState_Tick(null, null);
            
        }
        // Menu
        private void openDatabaseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FileDialog dlg;

            if (sender == createDatabaseToolStripMenuItem)
            {
                SaveFileDialog dlgsave = new SaveFileDialog();
                dlgsave.CreatePrompt = true;
                dlg = dlgsave;
            }
            else
                dlg = new OpenFileDialog();

            dlg.FileName = Properties.Settings.Default.DatabaseFile;
            dlg.Filter = "sdf|*.sdf";
            if (dlg.ShowDialog(this) != System.Windows.Forms.DialogResult.OK) return;
            if (File.Exists(dlg.FileName) || (sender == createDatabaseToolStripMenuItem))
            {
                DeviceDescrProvider.CreateDatabase(dlg.FileName);
                Properties.Settings.Default.DatabaseFile = dlg.FileName;
            }
        }
        // Menu
        private void addSalveToDatabaseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "xml|*.xml";
            if (dlg.ShowDialog(this) != System.Windows.Forms.DialogResult.OK) return;
            string ret=DeviceDescrProvider.AddDeviceDescr(Properties.Settings.Default.DatabaseFile, dlg.FileName);
            if (ret=="Done")
                MessageBox.Show(ret, "EtherCATExplorer", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
                MessageBox.Show(ret, "EtherCATExplorer", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        // Menu
        private void eEPROMReadWriteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (devicesTreeView.SelectedNode == null)
                new EEPROMTool(null).ShowDialog();
            else
                new EEPROMTool((EthCATDevice)devicesTreeView.SelectedNode.Tag).ShowDialog();
        }


    }

    // Coming from Yabe @ Sourceforge, by Morten Kvistgaard
    public class MyTraceListener : TraceListener
    {
        private MainDialog m_form;

        public MyTraceListener(MainDialog form)
        {
            m_form = form;
        }

        public override void Write(string message)
        {
            if (!m_form.IsHandleCreated) return;
            m_form.BeginInvoke((MethodInvoker)delegate { m_form.LogText.AppendText(message); });
        }

        public override void WriteLine(string message)
        {
            if (!m_form.IsHandleCreated) return;
            m_form.BeginInvoke((MethodInvoker)delegate { m_form.LogText.AppendText(message + Environment.NewLine); });
        }
    }
}

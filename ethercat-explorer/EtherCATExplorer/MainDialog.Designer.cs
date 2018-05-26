namespace EtherCATExplorer
{
    partial class MainDialog
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainDialog));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openDatabaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.createDatabaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addSalveToDatabaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.quitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.functionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openInterfaceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.writeStateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stateBootToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stateInitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statePreOperationalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stateSafeOperationalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stateOperationalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.sendAckToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendReconfigureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.PDOToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.eEPROMReadWriteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.devicesTreeView = new System.Windows.Forms.TreeView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.label2 = new System.Windows.Forms.Label();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.propertyGrid = new System.Windows.Forms.PropertyGrid();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.propertyInput = new System.Windows.Forms.PropertyGrid();
            this.InputDataLbl = new System.Windows.Forms.Label();
            this.propertyOutput = new System.Windows.Forms.PropertyGrid();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.LogText = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tmrRefreshState = new System.Windows.Forms.Timer(this.components);
            this.tmrStart = new System.Windows.Forms.Timer(this.components);
            this.tmrInputFlow = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).BeginInit();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.functionsToolStripMenuItem,
            this.optionToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(629, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openDatabaseToolStripMenuItem,
            this.createDatabaseToolStripMenuItem,
            this.addSalveToDatabaseToolStripMenuItem,
            this.quitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openDatabaseToolStripMenuItem
            // 
            this.openDatabaseToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.database_connect;
            this.openDatabaseToolStripMenuItem.Name = "openDatabaseToolStripMenuItem";
            this.openDatabaseToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.openDatabaseToolStripMenuItem.Text = "Open Database";
            this.openDatabaseToolStripMenuItem.Click += new System.EventHandler(this.openDatabaseToolStripMenuItem_Click);
            // 
            // createDatabaseToolStripMenuItem
            // 
            this.createDatabaseToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.database_add;
            this.createDatabaseToolStripMenuItem.Name = "createDatabaseToolStripMenuItem";
            this.createDatabaseToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.createDatabaseToolStripMenuItem.Text = "Create Database";
            this.createDatabaseToolStripMenuItem.Click += new System.EventHandler(this.openDatabaseToolStripMenuItem_Click);
            // 
            // addSalveToDatabaseToolStripMenuItem
            // 
            this.addSalveToDatabaseToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.database_edit;
            this.addSalveToDatabaseToolStripMenuItem.Name = "addSalveToDatabaseToolStripMenuItem";
            this.addSalveToDatabaseToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.addSalveToDatabaseToolStripMenuItem.Text = "Add Slave to Database";
            this.addSalveToDatabaseToolStripMenuItem.Click += new System.EventHandler(this.addSalveToDatabaseToolStripMenuItem_Click);
            // 
            // quitToolStripMenuItem
            // 
            this.quitToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.cross;
            this.quitToolStripMenuItem.Name = "quitToolStripMenuItem";
            this.quitToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.quitToolStripMenuItem.Text = "Quit";
            this.quitToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
            // 
            // functionsToolStripMenuItem
            // 
            this.functionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openInterfaceToolStripMenuItem,
            this.toolStripSeparator2,
            this.writeStateToolStripMenuItem,
            this.PDOToolStripMenuItem,
            this.eEPROMReadWriteToolStripMenuItem});
            this.functionsToolStripMenuItem.Name = "functionsToolStripMenuItem";
            this.functionsToolStripMenuItem.Size = new System.Drawing.Size(71, 20);
            this.functionsToolStripMenuItem.Text = "Functions";
            // 
            // openInterfaceToolStripMenuItem
            // 
            this.openInterfaceToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.add;
            this.openInterfaceToolStripMenuItem.Name = "openInterfaceToolStripMenuItem";
            this.openInterfaceToolStripMenuItem.Size = new System.Drawing.Size(200, 22);
            this.openInterfaceToolStripMenuItem.Text = "Open Interface";
            this.openInterfaceToolStripMenuItem.Click += new System.EventHandler(this.openInterfaceToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(197, 6);
            // 
            // writeStateToolStripMenuItem
            // 
            this.writeStateToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.stateBootToolStripMenuItem,
            this.stateInitToolStripMenuItem,
            this.statePreOperationalToolStripMenuItem,
            this.stateSafeOperationalToolStripMenuItem,
            this.stateOperationalToolStripMenuItem,
            this.toolStripSeparator1,
            this.sendAckToolStripMenuItem,
            this.sendReconfigureToolStripMenuItem});
            this.writeStateToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.monitor;
            this.writeStateToolStripMenuItem.Name = "writeStateToolStripMenuItem";
            this.writeStateToolStripMenuItem.Size = new System.Drawing.Size(200, 22);
            this.writeStateToolStripMenuItem.Text = "State Managment";
            // 
            // stateBootToolStripMenuItem
            // 
            this.stateBootToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.wrench;
            this.stateBootToolStripMenuItem.Name = "stateBootToolStripMenuItem";
            this.stateBootToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.stateBootToolStripMenuItem.Text = "State Boot";
            this.stateBootToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // stateInitToolStripMenuItem
            // 
            this.stateInitToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.control_stop_blue;
            this.stateInitToolStripMenuItem.Name = "stateInitToolStripMenuItem";
            this.stateInitToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.stateInitToolStripMenuItem.Text = "State Init";
            this.stateInitToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // statePreOperationalToolStripMenuItem
            // 
            this.statePreOperationalToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.control_pause_blue;
            this.statePreOperationalToolStripMenuItem.Name = "statePreOperationalToolStripMenuItem";
            this.statePreOperationalToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.statePreOperationalToolStripMenuItem.Text = "State PreOperational";
            this.statePreOperationalToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // stateSafeOperationalToolStripMenuItem
            // 
            this.stateSafeOperationalToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.control_play_blue;
            this.stateSafeOperationalToolStripMenuItem.Name = "stateSafeOperationalToolStripMenuItem";
            this.stateSafeOperationalToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.stateSafeOperationalToolStripMenuItem.Text = "State SafeOperational";
            this.stateSafeOperationalToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // stateOperationalToolStripMenuItem
            // 
            this.stateOperationalToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.control_fastforward_blue;
            this.stateOperationalToolStripMenuItem.Name = "stateOperationalToolStripMenuItem";
            this.stateOperationalToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.stateOperationalToolStripMenuItem.Text = "State Operational";
            this.stateOperationalToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(184, 6);
            // 
            // sendAckToolStripMenuItem
            // 
            this.sendAckToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.accept;
            this.sendAckToolStripMenuItem.Name = "sendAckToolStripMenuItem";
            this.sendAckToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.sendAckToolStripMenuItem.Text = "Acknowledge Error";
            this.sendAckToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // sendReconfigureToolStripMenuItem
            // 
            this.sendReconfigureToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.arrow_rotate_anticlockwise;
            this.sendReconfigureToolStripMenuItem.Name = "sendReconfigureToolStripMenuItem";
            this.sendReconfigureToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
            this.sendReconfigureToolStripMenuItem.Text = "Reconfigure";
            this.sendReconfigureToolStripMenuItem.Click += new System.EventHandler(this.stateToolStripMenuItem_Click);
            // 
            // PDOToolStripMenuItem
            // 
            this.PDOToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.slave;
            this.PDOToolStripMenuItem.Name = "PDOToolStripMenuItem";
            this.PDOToolStripMenuItem.Size = new System.Drawing.Size(200, 22);
            this.PDOToolStripMenuItem.Text = "Acyclic Read/Write SDO";
            this.PDOToolStripMenuItem.Click += new System.EventHandler(this.readPDOToolStripMenuItem_Click);
            // 
            // eEPROMReadWriteToolStripMenuItem
            // 
            this.eEPROMReadWriteToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.drive_disk;
            this.eEPROMReadWriteToolStripMenuItem.Name = "eEPROMReadWriteToolStripMenuItem";
            this.eEPROMReadWriteToolStripMenuItem.Size = new System.Drawing.Size(200, 22);
            this.eEPROMReadWriteToolStripMenuItem.Text = "EEPROM Read/Write";
            this.eEPROMReadWriteToolStripMenuItem.Click += new System.EventHandler(this.eEPROMReadWriteToolStripMenuItem_Click);
            // 
            // optionToolStripMenuItem
            // 
            this.optionToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.settingsToolStripMenuItem});
            this.optionToolStripMenuItem.Name = "optionToolStripMenuItem";
            this.optionToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.optionToolStripMenuItem.Text = "Options";
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.application_form;
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.settingsToolStripMenuItem.Text = "Settings";
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.settingsToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.helpToolStripMenuItem1,
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // helpToolStripMenuItem1
            // 
            this.helpToolStripMenuItem1.Image = global::EtherCATExplorer.Properties.Resources.information;
            this.helpToolStripMenuItem1.Name = "helpToolStripMenuItem1";
            this.helpToolStripMenuItem1.Size = new System.Drawing.Size(107, 22);
            this.helpToolStripMenuItem1.Text = "Help";
            this.helpToolStripMenuItem1.Click += new System.EventHandler(this.helpToolStripMenuItem1_Click);
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Image = global::EtherCATExplorer.Properties.Resources.information;
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 24);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.LogText);
            this.splitContainer1.Panel2.Controls.Add(this.label1);
            this.splitContainer1.Size = new System.Drawing.Size(629, 469);
            this.splitContainer1.SplitterDistance = 356;
            this.splitContainer1.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.devicesTreeView);
            this.splitContainer2.Panel1.Controls.Add(this.label2);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer3);
            this.splitContainer2.Panel2.Controls.Add(this.label3);
            this.splitContainer2.Size = new System.Drawing.Size(629, 356);
            this.splitContainer2.SplitterDistance = 317;
            this.splitContainer2.TabIndex = 0;
            // 
            // devicesTreeView
            // 
            this.devicesTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.devicesTreeView.ImageIndex = 0;
            this.devicesTreeView.ImageList = this.imageList1;
            this.devicesTreeView.Location = new System.Drawing.Point(0, 13);
            this.devicesTreeView.Name = "devicesTreeView";
            this.devicesTreeView.SelectedImageIndex = 0;
            this.devicesTreeView.Size = new System.Drawing.Size(317, 343);
            this.devicesTreeView.TabIndex = 1;
            this.devicesTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.devicesTreeView_AfterSelect);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "Plc.png");
            this.imageList1.Images.SetKeyName(1, "PlcProblem.png");
            this.imageList1.Images.SetKeyName(2, "PlcOffline.png");
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Top;
            this.label2.Location = new System.Drawing.Point(0, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(46, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Devices";
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 13);
            this.splitContainer3.Name = "splitContainer3";
            this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.propertyGrid);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.splitContainer4);
            this.splitContainer3.Size = new System.Drawing.Size(308, 343);
            this.splitContainer3.SplitterDistance = 104;
            this.splitContainer3.TabIndex = 1;
            // 
            // propertyGrid
            // 
            this.propertyGrid.BackColor = System.Drawing.SystemColors.Control;
            this.propertyGrid.CommandsBackColor = System.Drawing.SystemColors.Control;
            this.propertyGrid.CommandsDisabledLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(133)))), ((int)(((byte)(133)))), ((int)(((byte)(133)))));
            this.propertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid.HelpVisible = false;
            this.propertyGrid.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid.Name = "propertyGrid";
            this.propertyGrid.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.propertyGrid.Size = new System.Drawing.Size(308, 104);
            this.propertyGrid.TabIndex = 0;
            this.propertyGrid.ToolbarVisible = false;
            // 
            // splitContainer4
            // 
            this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer4.Location = new System.Drawing.Point(0, 0);
            this.splitContainer4.Name = "splitContainer4";
            this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.propertyInput);
            this.splitContainer4.Panel1.Controls.Add(this.InputDataLbl);
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.propertyOutput);
            this.splitContainer4.Panel2.Controls.Add(this.label4);
            this.splitContainer4.Size = new System.Drawing.Size(308, 235);
            this.splitContainer4.SplitterDistance = 100;
            this.splitContainer4.TabIndex = 0;
            // 
            // propertyInput
            // 
            this.propertyInput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyInput.HelpVisible = false;
            this.propertyInput.Location = new System.Drawing.Point(0, 13);
            this.propertyInput.Name = "propertyInput";
            this.propertyInput.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.propertyInput.Size = new System.Drawing.Size(308, 87);
            this.propertyInput.TabIndex = 1;
            this.propertyInput.ToolbarVisible = false;
            // 
            // InputDataLbl
            // 
            this.InputDataLbl.AutoSize = true;
            this.InputDataLbl.Dock = System.Windows.Forms.DockStyle.Top;
            this.InputDataLbl.Location = new System.Drawing.Point(0, 0);
            this.InputDataLbl.Name = "InputDataLbl";
            this.InputDataLbl.Size = new System.Drawing.Size(54, 13);
            this.InputDataLbl.TabIndex = 0;
            this.InputDataLbl.Text = "InputData";
            // 
            // propertyOutput
            // 
            this.propertyOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyOutput.HelpVisible = false;
            this.propertyOutput.Location = new System.Drawing.Point(0, 13);
            this.propertyOutput.Name = "propertyOutput";
            this.propertyOutput.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.propertyOutput.Size = new System.Drawing.Size(308, 118);
            this.propertyOutput.TabIndex = 1;
            this.propertyOutput.ToolbarVisible = false;
            this.propertyOutput.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid_PropertyValueChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Top;
            this.label4.Location = new System.Drawing.Point(0, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(62, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "OutputData";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Top;
            this.label3.Location = new System.Drawing.Point(0, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(54, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Properties";
            // 
            // LogText
            // 
            this.LogText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LogText.Location = new System.Drawing.Point(0, 13);
            this.LogText.Multiline = true;
            this.LogText.Name = "LogText";
            this.LogText.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.LogText.Size = new System.Drawing.Size(629, 96);
            this.LogText.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(25, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Log";
            // 
            // tmrRefreshState
            // 
            this.tmrRefreshState.Tick += new System.EventHandler(this.tmrRefreshState_Tick);
            // 
            // tmrStart
            // 
            this.tmrStart.Tick += new System.EventHandler(this.tmrRefreshState_Tick);
            // 
            // tmrInputFlow
            // 
            this.tmrInputFlow.Tick += new System.EventHandler(this.tmrInputFlow_Tick);
            // 
            // MainDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(629, 493);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainDialog";
            this.Text = "Simple EtherCAT Explorer";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainDialog_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel1.PerformLayout();
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel1.PerformLayout();
            this.splitContainer4.Panel2.ResumeLayout(false);
            this.splitContainer4.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).EndInit();
            this.splitContainer4.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem functionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem optionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem quitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openInterfaceToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox LogText;
        private System.Windows.Forms.TreeView devicesTreeView;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.Timer tmrRefreshState;
        private System.Windows.Forms.Timer tmrStart;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.PropertyGrid propertyGrid;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.Label InputDataLbl;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PropertyGrid propertyInput;
        private System.Windows.Forms.PropertyGrid propertyOutput;
        private System.Windows.Forms.Timer tmrInputFlow;
        private System.Windows.Forms.ToolStripMenuItem PDOToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeStateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stateInitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem statePreOperationalToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stateOperationalToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stateSafeOperationalToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendReconfigureToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem openDatabaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem createDatabaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addSalveToDatabaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendAckToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem stateBootToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem eEPROMReadWriteToolStripMenuItem;
    }
}


namespace EtherCATExplorer
{
    partial class ReadWritePDO
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ReadWritePDO));
            this.btRead = new System.Windows.Forms.Button();
            this.btWrite = new System.Windows.Forms.Button();
            this.lbldevice = new System.Windows.Forms.Label();
            this.IndexValue = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.SubIndexValue = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.PdoValues = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.dicoTree = new System.Windows.Forms.TreeView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.label4 = new System.Windows.Forms.Label();
            this.PdoValueDecodedLabel = new System.Windows.Forms.Label();
            this.PdoValueDecoded = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.btUnExpand = new System.Windows.Forms.Button();
            this.btExpand = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.IndexValue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SubIndexValue)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btRead
            // 
            this.btRead.Location = new System.Drawing.Point(12, 405);
            this.btRead.Name = "btRead";
            this.btRead.Size = new System.Drawing.Size(65, 24);
            this.btRead.TabIndex = 0;
            this.btRead.Text = "Read";
            this.btRead.UseVisualStyleBackColor = true;
            this.btRead.Click += new System.EventHandler(this.btRead_Click);
            // 
            // btWrite
            // 
            this.btWrite.Location = new System.Drawing.Point(217, 405);
            this.btWrite.Name = "btWrite";
            this.btWrite.Size = new System.Drawing.Size(65, 24);
            this.btWrite.TabIndex = 1;
            this.btWrite.Text = "Write";
            this.btWrite.UseVisualStyleBackColor = true;
            this.btWrite.Click += new System.EventHandler(this.btWrite_Click);
            // 
            // lbldevice
            // 
            this.lbldevice.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.lbldevice.Location = new System.Drawing.Point(147, 13);
            this.lbldevice.Name = "lbldevice";
            this.lbldevice.Size = new System.Drawing.Size(294, 23);
            this.lbldevice.TabIndex = 2;
            this.lbldevice.Text = "label1";
            this.lbldevice.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // IndexValue
            // 
            this.IndexValue.Hexadecimal = true;
            this.IndexValue.Location = new System.Drawing.Point(28, 26);
            this.IndexValue.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.IndexValue.Name = "IndexValue";
            this.IndexValue.Size = new System.Drawing.Size(69, 20);
            this.IndexValue.TabIndex = 3;
            this.IndexValue.Value = new decimal(new int[] {
            4104,
            0,
            0,
            0});
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(108, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Index in Hexadecimal";
            // 
            // SubIndexValue
            // 
            this.SubIndexValue.Location = new System.Drawing.Point(171, 26);
            this.SubIndexValue.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.SubIndexValue.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            this.SubIndexValue.Name = "SubIndexValue";
            this.SubIndexValue.Size = new System.Drawing.Size(73, 20);
            this.SubIndexValue.TabIndex = 5;
            this.SubIndexValue.Value = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(156, 8);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(107, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Sub Index in Decimal";
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.SubIndexValue);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.IndexValue);
            this.panel1.Location = new System.Drawing.Point(12, 64);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(270, 62);
            this.panel1.TabIndex = 7;
            // 
            // PdoValues
            // 
            this.PdoValues.Location = new System.Drawing.Point(12, 168);
            this.PdoValues.Multiline = true;
            this.PdoValues.Name = "PdoValues";
            this.PdoValues.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.PdoValues.Size = new System.Drawing.Size(270, 132);
            this.PdoValues.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 149);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(138, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "SDO Values in hexadecimal";
            // 
            // dicoTree
            // 
            this.dicoTree.ImageIndex = 0;
            this.dicoTree.ImageList = this.imageList1;
            this.dicoTree.Location = new System.Drawing.Point(306, 64);
            this.dicoTree.Name = "dicoTree";
            this.dicoTree.SelectedImageIndex = 0;
            this.dicoTree.ShowNodeToolTips = true;
            this.dicoTree.Size = new System.Drawing.Size(260, 365);
            this.dicoTree.TabIndex = 10;
            this.dicoTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.dicoTree_AfterSelect);
            this.dicoTree.DoubleClick += new System.EventHandler(this.dicoTree_DoubleClick);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "struct.png");
            this.imageList1.Images.SetKeyName(1, "bool.png");
            this.imageList1.Images.SetKeyName(2, "numbers.png");
            this.imageList1.Images.SetKeyName(3, "text_smallcaps.png");
            this.imageList1.Images.SetKeyName(4, "time.png");
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(303, 48);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(93, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Objects Dictionary";
            // 
            // PdoValueDecodedLabel
            // 
            this.PdoValueDecodedLabel.AutoSize = true;
            this.PdoValueDecodedLabel.Location = new System.Drawing.Point(9, 318);
            this.PdoValueDecodedLabel.Name = "PdoValueDecodedLabel";
            this.PdoValueDecodedLabel.Size = new System.Drawing.Size(112, 13);
            this.PdoValueDecodedLabel.TabIndex = 12;
            this.PdoValueDecodedLabel.Text = "SDO Values Decoded";
            // 
            // PdoValueDecoded
            // 
            this.PdoValueDecoded.Location = new System.Drawing.Point(12, 335);
            this.PdoValueDecoded.Multiline = true;
            this.PdoValueDecoded.Name = "PdoValueDecoded";
            this.PdoValueDecoded.ReadOnly = true;
            this.PdoValueDecoded.Size = new System.Drawing.Size(270, 60);
            this.PdoValueDecoded.TabIndex = 13;
            this.PdoValueDecoded.Text = "Undecoded";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 48);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(81, 13);
            this.label5.TabIndex = 14;
            this.label5.Text = "Object Identifier";
            // 
            // btUnExpand
            // 
            this.btUnExpand.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btUnExpand.Location = new System.Drawing.Point(553, 48);
            this.btUnExpand.Name = "btUnExpand";
            this.btUnExpand.Size = new System.Drawing.Size(13, 14);
            this.btUnExpand.TabIndex = 15;
            this.btUnExpand.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.btUnExpand.UseVisualStyleBackColor = true;
            this.btUnExpand.Click += new System.EventHandler(this.btUnExpand_Click);
            // 
            // btExpand
            // 
            this.btExpand.Font = new System.Drawing.Font("Microsoft Sans Serif", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btExpand.Location = new System.Drawing.Point(534, 48);
            this.btExpand.Name = "btExpand";
            this.btExpand.Size = new System.Drawing.Size(13, 14);
            this.btExpand.TabIndex = 16;
            this.btExpand.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.btExpand.UseVisualStyleBackColor = true;
            this.btExpand.Click += new System.EventHandler(this.btExpand_Click);
            // 
            // ReadWritePDO
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(588, 452);
            this.Controls.Add(this.btExpand);
            this.Controls.Add(this.btUnExpand);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.PdoValueDecoded);
            this.Controls.Add(this.PdoValueDecodedLabel);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.dicoTree);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.PdoValues);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.lbldevice);
            this.Controls.Add(this.btWrite);
            this.Controls.Add(this.btRead);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ReadWritePDO";
            this.Text = "Read & Write SDO";
            this.Load += new System.EventHandler(this.ReadWritePDO_Load);
            ((System.ComponentModel.ISupportInitialize)(this.IndexValue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SubIndexValue)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btRead;
        private System.Windows.Forms.Button btWrite;
        private System.Windows.Forms.Label lbldevice;
        private System.Windows.Forms.NumericUpDown IndexValue;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown SubIndexValue;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox PdoValues;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TreeView dicoTree;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.Label PdoValueDecodedLabel;
        private System.Windows.Forms.TextBox PdoValueDecoded;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btUnExpand;
        private System.Windows.Forms.Button btExpand;
    }
}
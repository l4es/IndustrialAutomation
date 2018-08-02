using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using ScadaShell.View;
using ScadaShell.DataAccess;
using FreeSCADA.Interfaces;

namespace ScadaShell.Model
{
    public class MainSchema : Schema
    {
        
        public KranElement Kran1
        {
            get;
            protected set;
        }
        public KranElement Kran2
        {
            get;
            protected set;
        }
        public KranElement Kran3
        {
            get;
            protected set;
        }
        public KranElement Kran4
        {
            get;
            protected set;
        }
        public ComplexParametrElement Pin
        {
            get;
            protected set;
        }
        public ComplexParametrElement Pout
        {
            get;
            protected set;
        }
        public ComplexParametrElement Tin
        {
            get;
            protected set;
        }
        public ComplexParametrElement Tout
        {
            get;
            protected set;
        }
        public IChannel Pwork
        {
            get;
            protected set;
        }
        public ComplexParametrElement Q1
        {
            get;
            protected set;
        }
        public ComplexParametrElement Q2
        {
            get;
            protected set;
        }
        public ComplexParametrElement Q3
        {
            get;
            protected set;
        }
        public ComplexParametrElement Q4
        {
            get;
            protected set;
        }

     
        
        public MainSchema()
        {
            Name = "Мнемосхема";

            Kran1 = new KranElement("Kran1", "Кран №1");
            Kran2 = new KranElement("Kran2", "Кран №2");
            Kran3 = new KranElement("Kran3" ,"Кран №3");
            Kran4 =new KranElement("Kran4","Кран №4");    
            Pin = new ComplexParametrElement("Pin", "Вхiодний тиск");
            Pout = new ComplexParametrElement("Pout", "Вихідний тиск");
            Tin = new ComplexParametrElement("Tin", "Вхідна температура");
            Tout = new ComplexParametrElement("Tout", "Вихідна температура"); 
            Pwork = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.Pwork");
            Q1 = new ComplexParametrElement("Q1", "Загазованість канал 1");
            Q2 = new ComplexParametrElement ("Q2","Загазованість канал 2");
            Q3 = new ComplexParametrElement ("Q3","Загазованість канал 3");
            Q4 = new ComplexParametrElement("Q4", "Загазованість канал 4");
        }
    }
}

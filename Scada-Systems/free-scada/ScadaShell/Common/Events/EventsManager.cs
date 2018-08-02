using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events
{
    public class EventNotExistsException: Exception
    {
        public EventNotExistsException(int id):base(string.Format("Event ID={0} not exisists",id))
        {
        }
    }
    public class EventsManager
    {
        public delegate void EventAddedDelegate(IEventRule er);
        public event EventAddedDelegate EventAdded;

        Dictionary<int, IEventRule> _events = new Dictionary<int, IEventRule>();

        
        public EventsManager()
        {
        }
        public IEventRule this[int id]
        {
            get
            {

                if (_events.ContainsKey(id))
                    return _events[id];
                throw new EventNotExistsException(id);
            }
        }


        public ReadOnlyCollection<IEventRule> Events
        {
            get
            {
                return new ReadOnlyCollection<IEventRule>(_events.Values.ToList());
            }
        }
        public void RegisterEvent(IEventRule rule)
        {
            _events[rule.Definition.Id] = rule;
            if (this.EventAdded != null)
                EventAdded(rule);
        }
        public void LoadSettings()
        {
            _events.Clear();
            try
            {
                RegisterEvent(new Rules.StartRule());
                ///диагностические
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pin.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pin.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика давления Pin")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика давления Pout")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика температуры Tin")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика температуры Tout")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика загазованности Q1")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика загазованности Q2")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика загазованности Q3")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.Break"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Обрыв датчика загазованности Q4")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Power220"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Отсутствие 220В")));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Acc"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        false,
                        new Rules.EqualExpression(),
                        new DiagnosticEventDefinition("Низкое напряж. аккумулятора")));

                ////технологические
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.SpLow"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 1 канал 1", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.SpHigh"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q1.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 2 канал 1", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.SpLow"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 1 канал 2", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.SpHigh"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q2.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 2 канал 2", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.SpLow"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 1 канал 3", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.SpHigh"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q3.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 2 канал 3", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.SpLow"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 1 канал 4", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.SpHigh"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Q4.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Загазванность порог 2 канал 4", AlarmLevel.Red)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.PinErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pin.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Входное давление не норме", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.PoutErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Выходное давление не норме", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.PinBrErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pin.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Входное давление блока ред. не норме", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.PoutBrErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Выходное давление блока ред. не норме", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Perimtr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Perimtr"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Периметральная сигнализация", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Fire"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.Fire"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Сработала пожарная сигнализация", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.OdorErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.OdorErr"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Авария одоризационной установки", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.HeaterErr"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.HeaterErr"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Авария подогревателя", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Discrets.AvarOstanov"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Аварийный останов", AlarmLevel.Red)));
                

                
                
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveOpenning"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveOpenning"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Открытие крана 1", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveClosing"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveClosing"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Закрытие крана 1", AlarmLevel.Yellow)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveOpenning"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveOpenning"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Открытие крана 2", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveClosing"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveClosing"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Закрытие крана 2", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveOpenning"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveOpenning"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Открытие крана 3", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveClosing"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveClosing"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Закрытие крана 3", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveOpenning"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveOpenning"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Открытие крана 4", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveClosing"),
                        Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveClosing"),
                        true,
                        new Rules.EqualExpression(),
                        new TechnologyEventDefinition("Закрытие крана 4", AlarmLevel.Yellow)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.SpMinMin"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Аварийно низкое давление", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.SpMin"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Понижение давления", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.SpMax"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Повышение давления", AlarmLevel.Yellow)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.SpMaxMax"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Аварийно высокое давление", AlarmLevel.Red)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.Mode"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     0,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Переведен в режим наладки", AlarmLevel.Green)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.Mode"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     1,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Переведен в ручной режим", AlarmLevel.Green)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.Mode"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                     2,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Переведен в автоматический режим", AlarmLevel.Green)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Auto.OutFromAuto"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Pout.Value"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Выведен из автомат. режима", AlarmLevel.Red)));
                
                
                
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.SpLow"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.Value"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Температура на входе ГРС ниже нормы", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.SpHigh"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tin.Value"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Температура на входе ГРС выше нормы", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.SpLow"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.Value"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Температура на выходе ГРС ниже нормы", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.SpHigh"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Analog.Tout.Value"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Температура на выходе ГРС выше нормы", AlarmLevel.Orange)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.CoilOpenError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.CoilOpenError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен открытия кр1", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.CoilCloseError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.CoilCloseError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен закрытия кр1", AlarmLevel.Red)));
               RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.ValveError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Кран 1 заблокирован", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.TransposeError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.TransposeError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неудачная перестановка кр1", AlarmLevel.Red)));
               RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.SwitchError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran1.SwitchError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Отказ концевика кр1", AlarmLevel.Red)));
               
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.CoilOpenError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.CoilOpenError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен открытия кр2", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.CoilCloseError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.CoilCloseError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен закрытия кр2", AlarmLevel.Red)));
               RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.ValveError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Кран 2 заблокирован", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.TransposeError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.TransposeError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неудачная перестановка кр2", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.SwitchError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran2.SwitchError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Отказ концевика кр2", AlarmLevel.Red)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.CoilOpenError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.CoilOpenError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен открытия кр3", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.CoilCloseError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.CoilCloseError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен закрытия кр3", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveError"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.ValveError"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Кран 3 заблокирован", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.TransposeError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.TransposeError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неудачная перестановка кр3", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.SwitchError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran3.SwitchError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Отказ концевика кр3", AlarmLevel.Red)));

                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.CoilOpenError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.CoilOpenError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен открытия кр4", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.CoilCloseError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.CoilCloseError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неиспр. солен закрытия кр4", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveError"),
                     Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.ValveError"),
                     true,
                     new Rules.EqualExpression(),
                     new TechnologyEventDefinition("Кран 4 заблокирован", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.TransposeError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.TransposeError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Неудачная перестановка кр4", AlarmLevel.Red)));
                RegisterEvent(new Rules.ChannelConstantComparerRule(
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.SwitchError"),
                    Env.Current.CommunicationPlugins.GetChannel("opc_connection_plug.Zachist.Kran4.SwitchError"),
                    true,
                    new Rules.EqualExpression(),
                    new TechnologyEventDefinition("Отказ концевика кр4", AlarmLevel.Red)));
                 


            }
            catch (ArgumentNullException e)
            {

            }
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events.States;

namespace FreeSCADA.Common.Events.Rules
{
    
    public class ChannelConstantComparerRule:BaseRule
    {
        IChannel _channel;
        IChannel _channelForRegister;
        object _constant;
        IRuleExpression _expression;
        bool _prevVal = false;
        public override IChannel ChannelForRegister
        {
            get { return _channelForRegister; }
        }

        public ChannelConstantComparerRule(IChannel ch,IChannel rch, object cons,IRuleExpression expression,IEventDefinition def):base(def)
        {
            if (ch == null)
                throw new ArgumentNullException();
            if (rch == null)
                throw new ArgumentNullException();
            _channel = ch;
            _channelForRegister = rch;
            _constant = cons;
            _expression = expression;
            _channel.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(_channel_PropertyChanged);

        }

        void _channel_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Value")
            {
                UpdateState();
            }
            
        }

        override public void UpdateState()
        {
            bool val=_expression.Exec(_channel.Value, _constant);
            if ( val && !_prevVal)
                EmitEvent(new StartedEventState(_channelForRegister.Value,DateTime.Now ));
            else if (!val && _prevVal)
                EmitEvent(new EndedEventState(_channelForRegister.Value, DateTime.Now));
            _prevVal = val;
        }
     
    }
}

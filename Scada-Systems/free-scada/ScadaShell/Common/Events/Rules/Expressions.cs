using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Rules
{
    abstract class BaseExpression:IRuleExpression
    {
        protected IComparable _one, _two;
        protected void ToComparable(object one, object two)
        {
            if (!(one is IComparable) || !(two is IComparable))
                throw new Exception("Not comparable objects");
            _one = one as IComparable;
         
            two=Convert.ChangeType(two, _one.GetType());
            _two = two as IComparable;
        }


        abstract public bool Exec(object one, object two);
        
    }

    class LessExpression : BaseExpression
    {
        override public bool Exec(object one, object two)
        {
            ToComparable(one, two);

            if ((_one as IComparable).CompareTo(_two) < 0)
                return true;
            return false;
        }
    }
    class MoreExpression : BaseExpression
    {
        override public bool Exec(object one, object two)
        {
            ToComparable(one, two);

            if ((_one as IComparable).CompareTo(_two) > 0)
                return true;
            return false;
        }
    }
    class EqualExpression : BaseExpression
    {
        override public bool Exec(object one, object two)
        {
            ToComparable(one, two);

            if ((_one as IComparable).CompareTo(_two) == 0)
                return true;
            return false;
        }
    }
    class LessOrEqalExpression : BaseExpression
    {
        override public bool Exec(object one, object two)
        {
            ToComparable(one, two);

            if ((_one as IComparable).CompareTo(_two) == 0 || (_one as IComparable).CompareTo(_two) < 0)
                return true;
            return false;
        }
    }
    class MoreOrEqalExpression : BaseExpression
    {
        override public bool Exec(object one, object two)
        {
            ToComparable(one, two);
            
            if ((_one as IComparable).CompareTo(_two) == 0 || (_one as IComparable).CompareTo(_two) > 0)
                return true;
            return false;
        }
    }

}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC
{
    public class ConstructorParameterDependency
    {
        public Type ArgType { get; private set; }
        public string Name { get; private set; }
        public ConstantExpression Value { get; private set; }
        public int Position { get; set; }

        public ConstructorParameterDependency(Type argType, string name, ConstantExpression value)
        {
            ArgType = argType;
            Name = name;
            Value = value;
        }

        public ConstructorParameterDependency(Type argType, string name, ConstantExpression value, int position)
        {
            ArgType = argType;
            Name = name;
            Value = value;
            Position = position;
        }
    }
}

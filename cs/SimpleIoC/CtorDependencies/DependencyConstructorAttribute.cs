using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC
{
    [AttributeUsage(AttributeTargets.Constructor)]
    public class DependencyConstructorAttribute : Attribute
    {
    }
}

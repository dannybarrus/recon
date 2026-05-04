using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC.Factories
{
    public class SingletonFactory : IFactoryProvider
    {
        private Lazy<object> singletonCreator;

        public SingletonFactory(Delegate objectCreator)
        {
            singletonCreator = new Lazy<object>(() => objectCreator.DynamicInvoke());
        }

        public object Create()
        {
            return singletonCreator.Value;
        }
    }
}

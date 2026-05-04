using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC.Factories
{
    public class TransientFactory : IFactoryProvider
    {
        private Delegate objectCreator;

        public TransientFactory(Delegate objectCreator)
        {
            this.objectCreator = objectCreator;
        }

        public object Create()
        {
            return objectCreator.DynamicInvoke();
        }
    }
}

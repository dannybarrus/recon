using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC.UnitTest
{
    class MicroUSB : ICharger
    {
        public string Charge()
        {
            return "Charging with MicroUSB";
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC.UnitTest
{
    class Usb_C : ICharger
    {
        public string Charge()
        {
            return "Charging with Usb_C";
        }
    }
}

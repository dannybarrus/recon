using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC.UnitTest
{
    class Camera
    {
        private readonly ICharger chargingCable;

        public Camera(ICharger charger)
        {
            this.chargingCable = charger;
        }

        public string Charge()
        {
            var chargeMessage = chargingCable.Charge();

            return chargeMessage;
        }
    }
}

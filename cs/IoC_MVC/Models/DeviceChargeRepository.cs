using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace IoC_MVC.Models
{
    public interface IDeviceChargeRepository
    {
        DeviceChargeData GetData(DateTime date);
        void SetTotalCharge(DateTime date, int value);
        void SetCapacity(DateTime date, int value);
    }
    public class DeviceChargeRepository : IDeviceChargeRepository
    {
        private static DeviceChargeData data = new DeviceChargeData();

        public DeviceChargeRepository()
        {
            data.Capacity = 100;
        }

        public DeviceChargeData GetData(DateTime date)
        {
            return data;
        }

        public void SetTotalCharge(DateTime date, int value)
        {
            data.TotalCharge = value;
        }

        public void SetCapacity(DateTime date, int value)
        {
            data.Capacity = value;
        }
    }
}
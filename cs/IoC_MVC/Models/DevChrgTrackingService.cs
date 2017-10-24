using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace IoC_MVC.Models
{
    public class DevChrgTrackingService : IDevChrgTrackingService
    {
        private IDeviceChargeRepository repository;

        public DevChrgTrackingService(IDeviceChargeRepository repository)
        {
            this.repository = repository;
        }

        public int TotalCharge
        {
            get { return repository.GetData(new DateTime().Date).TotalCharge; }
            set { repository.SetTotalCharge(new DateTime().Date, value); }
        }

        public int Capacity
        {
            get { return repository.GetData(new DateTime().Date).Capacity; }
            set { repository.SetCapacity(new DateTime().Date, value); }
        }

        public void ManuallyAddCharge(int amount)
        {
            TotalCharge += amount;
        }
    }
}
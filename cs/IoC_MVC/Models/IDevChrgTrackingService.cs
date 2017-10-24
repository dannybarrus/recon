using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace IoC_MVC.Models
{
    public interface IDevChrgTrackingService
    {
        int TotalCharge { get; set; }
        int Capacity { get; set; }
        void ManuallyAddCharge(int amount);
    }
}
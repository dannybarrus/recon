using IoC_MVC.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace IoC_MVC.Controllers
{
    public class DevChrgTrackingController : Controller
    {

        private IDevChrgTrackingService devChrgTrackingService;

        public DevChrgTrackingController(IDevChrgTrackingService devChrgTrackingService)
        {
            this.devChrgTrackingService = devChrgTrackingService;
        }
        

        // GET: DevChrgTracking
        public ActionResult Index()
        {
            ViewBag.TotalCharge = devChrgTrackingService.TotalCharge;
            ViewBag.Capacity = devChrgTrackingService.Capacity;

            return View();
        }

        public ActionResult ManuallyAddCharge(int amount)
        {
            devChrgTrackingService.ManuallyAddCharge(amount);

            ViewBag.TotalCharge = devChrgTrackingService.TotalCharge;
            ViewBag.Capacity = devChrgTrackingService.Capacity;

            return View("Index");
        }
    }
}
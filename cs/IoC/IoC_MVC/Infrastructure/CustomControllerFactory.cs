using IoC_MVC.Controllers;
using IoC_MVC.Models;
using SimpleIoC;
using SimpleIoC.Factories;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Routing;
using System.Web.SessionState;

namespace IoC_MVC.Infrastructure
{
    public class CustomControllerFactory : IControllerFactory
    {
        public IController CreateController(RequestContext requestContext, string controllerName)
        {
            if (controllerName.ToLower().StartsWith("devchrgtracking"))
            {
                // show dependecy chain if manually created
                //var repository = new DeviceChargeRepository(100);
                //var service = new DevChrgTrackingService(repository);
                //var controller = new DevChrgTrackingController(service);

                Resolver resolve = new Resolver();

                resolve.RegisterComponents(
                    new Component().For<DeviceChargeRepository>().WithInstanceMode(InstanceMode.Transient),
                    new Component().For<DevChrgTrackingService>().WithInstanceMode(InstanceMode.Transient),
                    new Component().For<DevChrgTrackingController>().WithInstanceMode(InstanceMode.Transient));

                resolve.ResolveDependencies();
                
                var controller = resolve.Resolve<DevChrgTrackingController>();
               

                return controller;
            }

            var defaultFactory = new DefaultControllerFactory();
            return defaultFactory.CreateController(requestContext, controllerName);
        }

        public SessionStateBehavior GetControllerSessionBehavior(RequestContext requestContext, string controllerName)
        {
            return SessionStateBehavior.Default;
        }

        public void ReleaseController(IController controller)
        {
            var disposable = controller as IDisposable;
            if (disposable != null)
                disposable.Dispose();
        }
    }
}

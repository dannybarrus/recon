using SimpleIoC.Factories;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xunit;

namespace SimpleIoC.UnitTest
{
    public class IoCContainerTest
    {
        [Trait("Category", value: "All")]
                
        [Fact]
        public void SmokeTest()
        {
            Resolver resolve = new Resolver();

            resolve.RegisterComponents(
                new Component().ServiceFor<ICharger, MiniUSB>().WithInstanceMode(InstanceMode.Transient),
                new Component().For<Phone>().WithInstanceMode(InstanceMode.Transient));

            resolve.ResolveDependencies();

            Phone test = resolve.Resolve<Phone>();
            string message = test.Charge();

            Assert.Equal(message, "Charging with MiniUSB");
        }

        [Fact]
        public void TestSingleton()
        {
            Resolver resolve = new Resolver();

            resolve.RegisterComponents(
                new Component().ServiceFor<ICharger, MiniUSB>().WithInstanceMode(InstanceMode.Transient),
                new Component().For<Phone>().WithInstanceMode(InstanceMode.Singleton));

            resolve.ResolveDependencies();

            Phone sut1 = resolve.Resolve<Phone>();
            Phone sut2 = resolve.Resolve<Phone>();

            // Dependent is transient, but outer object should be singleton
            Assert.Equal(sut1, sut2);
        }

        [Fact]
        public void TestTransient()
        {
            Resolver resolve = new Resolver();

            resolve.RegisterComponents(
                new Component().ServiceFor<ICharger, MiniUSB>().WithInstanceMode(InstanceMode.Transient),
                new Component().For<Phone>().WithInstanceMode(InstanceMode.Transient));

            resolve.ResolveDependencies();

            Phone sut1 = resolve.Resolve<Phone>();
            Phone sut2 = resolve.Resolve<Phone>();

            // not equal means separate objects
            Assert.NotEqual(sut1, sut2);
        }

        [Fact]
        public void TestRegistrationFailure()
        {
            Resolver resolve = new Resolver();

            resolve.RegisterComponents(
                new Component().ServiceFor<ICharger, MiniUSB>().WithInstanceMode(InstanceMode.Transient),
                new Component().For<Phone>().WithInstanceMode(InstanceMode.Transient));

            resolve.ResolveDependencies();


            // analyze
            var ex = Assert.Throws<Exception>(() =>
            {
                var sut1 = resolve.Resolve<Camera>(); 
            }
                );


            Assert.True(ex.Message.Contains(value: "Could not create instance of"));
            
        }
    }
}

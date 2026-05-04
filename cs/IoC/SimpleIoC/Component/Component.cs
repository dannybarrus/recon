using SimpleIoC.Factories;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC
{
    public class Component
    {
        private ComponentRegistration componentRegistration;

        public Component For<TCOMP>()
        {
            componentRegistration = new ComponentRegistration(typeof(TCOMP));
            return this;
        }

        public Component ServiceFor<TInt, TCOMP>()
        {
            componentRegistration = new ComponentRegistration(typeof(TInt), typeof(TCOMP));
            return this;
        }

        public Component DependsOn<T>(T dependencies)
        {
            if (componentRegistration == null)
            {
                throw new Exception(
                    "Configuration error DependsOn<> MUST be called after For<> or ImplementedBy<>");
            }
            else
            {
                List<ConstructorParameterDependency> constructorDependencies = new List<ConstructorParameterDependency>();
                foreach (string name in typeof(T).GetConstructors()[0].GetParameters()
                                                .Select(p => p.Name))
                {
                    PropertyInfo property = typeof(T).GetProperty(name);
                    ParameterExpression param = Expression.Parameter(typeof(T), "x");
                    Expression propertyAccess = Expression.Property(param, property);
                    Expression convert = Expression.Convert(propertyAccess, typeof(object));
                    Func<T, object> lambda = Expression.Lambda<Func<T, object>>(convert, param).Compile();
                    var result = lambda(dependencies);
                    constructorDependencies.Add(new ConstructorParameterDependency(
                        property.PropertyType, name, Expression.Constant(result)));
                }

                if (constructorDependencies.Any())
                {
                    componentRegistration.HasManualConstructorParameters = true;
                    componentRegistration.DependsOnValues = constructorDependencies;
                }
                else
                {
                    componentRegistration.HasManualConstructorParameters = false;
                }
                return this;
            }
        }

        public ComponentRegistration WithInstanceMode(InstanceMode instanceMode)
        {
            if (componentRegistration == null)
            {
                throw new Exception ("Configuration error WithInstanceMode<> MUST be last");
            }
            else
            {
                componentRegistration.InstanceMode = instanceMode;
                return componentRegistration;
            }
        }

    }
}

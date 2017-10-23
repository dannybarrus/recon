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
    public class Resolver
    {
        private Dictionary<Type, Type> dependencyMap = new Dictionary<Type, Type>();
        private object syncLock = new object();
        private Dictionary<ComponentRegistration, IFactoryProvider> components =
            new Dictionary<ComponentRegistration, IFactoryProvider>();

        public T Resolve<T>()
        {
            lock (syncLock)
            {
                IFactoryProvider creator = components.Where(x => x.Key.TypeToCreate == typeof(T)).Select(x => x.Value).SingleOrDefault();

                if (creator != null)
                {
                    T newlyCreatedObject = (T)creator.Create();
                    ResolveProperties<T>(newlyCreatedObject);
                    return newlyCreatedObject;
                }
                else
                {
                    throw new Exception(string.Format(
                        "Could not create instance of {0} could not find correct IFactoryProvider. This may be down to missing component registration",
                        typeof(T).FullName));
                }
            }
        }
                
        public void RegisterComponents(params ComponentRegistration[] registrations)
        {
            lock (syncLock)
            {
                foreach (ComponentRegistration componentRegistration in registrations.ToList())
                {
                    components.Add(componentRegistration, null);
                }
            }
        }

        public void ResolveDependencies()
        {
            foreach (ComponentRegistration key in components.Where(c => c.Value == null).Select(c => c.Key).ToList())
            {
                CreateFactory(key, GetConstructorDelegateForType(key.TypeToCreate), key.InstanceMode);
            }
        }

        private void CreateFactory(ComponentRegistration key, Delegate @delegate, InstanceMode instanceMode)
        {
            IFactoryProvider factoryProvider = null;

            if (instanceMode == InstanceMode.Transient)
            {
                factoryProvider = new TransientFactory(@delegate);
            }

            if (instanceMode == InstanceMode.Singleton)
            {
                factoryProvider = new SingletonFactory(@delegate);
            }

            lock (syncLock)
            {
                components[key] = factoryProvider;
            }
        }

        private Delegate GetConstructorDelegateForType(Type type)
        {
            ComponentRegistration componentRegistration = null;

            //look for constructor that is marked with DependencyConstructorAttribute, 
            //if there is none just try and go for the default constructor
            ConstructorInfo ctor = type.GetConstructors()
                .Where(x => x.GetCustomAttributes(typeof(DependencyConstructorAttribute), false).Count() > 0).SingleOrDefault();

            if (ctor == null)
            {
                ctor = type.GetConstructors()[0];
            }

            foreach (var ctorArg in ctor.GetParameters())
            {

                componentRegistration = GetComponentRegistration(ctorArg.ParameterType);
                if (components[componentRegistration] == null)
                {
                    Delegate delegateForType = GetConstructorDelegateForType(componentRegistration.TypeToCreate);
                    CreateFactory(componentRegistration, delegateForType, componentRegistration.InstanceMode);
                }
            }

            List<ConstructorParameterDependency> args = new List<ConstructorParameterDependency>();
            foreach (var ctorArg in ctor.GetParameters())
            {

                componentRegistration = GetComponentRegistration(ctorArg.ParameterType);
                args.Add(new ConstructorParameterDependency(
                    ctorArg.ParameterType,
                    ctorArg.Name,
                    Expression.Constant(components[componentRegistration].Create()),
                    ctorArg.Position));
            }

            componentRegistration = GetComponentRegistration(type);
            if (componentRegistration != null)
            {
                if (componentRegistration.DependsOnValues.Any())
                {
                    args.AddRange(componentRegistration.DependsOnValues);
                }
            }

            return Expression.Lambda(Expression.New(ctor, args.OrderBy(x => x.Position)
                .Select(x => x.Value).ToArray())).Compile();
        }

        private ComponentRegistration GetComponentRegistration(Type typeToLookFor)
        {
            ComponentRegistration componentRegistration =
                components.Single(x => x.Key.TypeToCreate == typeToLookFor ||
                    typeToLookFor.IsAssignableFrom(x.Key.TypeToLookFor)).Key;
            return componentRegistration;
        }

        private void ResolveProperties<T>(T newlyCreatedObject)
        {
            foreach (PropertyInfo prop in newlyCreatedObject.GetType().GetProperties()
                .Where(x => x.GetCustomAttributes(typeof(DependencyConstructorAttribute), false).Count() > 0))
            {
                IFactoryProvider factoryProvider = components.Single(x => x.Key.TypeToCreate == prop.PropertyType ||
                    prop.PropertyType.IsAssignableFrom(x.Key.TypeToLookFor)).Value;

                if (factoryProvider != null)
                {
                    prop.SetValue(newlyCreatedObject, factoryProvider.Create(), null);
                }
                else
                {
                    throw new Exception (string.Format(
                        "Couldn't find instance of {0} to use for property injection", prop.PropertyType.FullName));
                }
            }
        }
    }
}

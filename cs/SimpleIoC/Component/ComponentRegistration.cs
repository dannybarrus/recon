using SimpleIoC.Factories;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleIoC
{
    public class ComponentRegistration
    {
        public Type TypeToLookFor { get; private set; }
        public Type TypeToCreate { get; private set; }
        public InstanceMode InstanceMode { get; set; }
        public bool HasManualConstructorParameters { get; set; }
        public List<ConstructorParameterDependency> DependsOnValues { get; set; }

        public ComponentRegistration(Type typeToCreate) : this(typeToCreate, typeToCreate)
        {

        }

        public ComponentRegistration(Type typeToLookFor, Type typeToCreate)
        {
            TypeToLookFor = typeToLookFor;
            TypeToCreate = typeToCreate;
            DependsOnValues = new List<ConstructorParameterDependency>();
        }
    }
}

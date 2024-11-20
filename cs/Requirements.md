Requirements:

 

1. Write an IoC (Inversion of Control) container, also known as a Dependency Injection container.
2. The container must allow you to register types.
   1. Example: container.Register<ICalculator, Calculator>()
3. The container must be aware and control object lifecycle for transient objects (a new instance is created for each request), and singleton objects (the same instance is returned for each request).
   1. Example: container.Register<ICalculator, Calculator>(LifecycleType.Transient), or container.Register<ICalculator, Calculator>(LifecycleType.Singleton)
4. The default lifecycle for an object should be transient
5. You must be able to resolve registered types through the container
   1. Example: container.Resolve<ICalculator>()
6. If you try to resolve a type that the container is unaware of it should throw an informative exception
7. When resolving from the container for a registered type, if that type has constructor arguments which are also registered types the container should inject the instances into the constructor appropriately (this is where dependency injection applies).
   1. Example Constructor: public UsersController(ICalculator calculator,  IEmailService emailService). If all 3 types for the controller, calculator, and email service are registered you should be able to resolve an instance of the UsersController.
8. You must write tests for all of this behavior using xUnit.
9. With a simple new ASP.NET MVC Web Application, wire things up so your Controllers can be constructed using your containers. There are many well documented ways to accomplish this with other containers.
10. You must use git for source control and push your code to github.com. Please send me the link to your repository at least 1 day before the second interview.
11. General Suggestion: Don't let the overall tasks overwhelm you. Break everything into smaller pieces that build up to the larger solution.
12. Be prepared to answer a question along the lines of… How would your code change if given the requirement to add a new lifecycle (ThreadStatic for instance)? Would you be required to add new code, or modify existing code?

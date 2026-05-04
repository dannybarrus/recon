namespace Recon.SOLID
{
    // VIOLATION: one fat interface forces RobotWorker to implement Eat()
    public interface IWorkerViolation
    {
        void Work();
        void Eat();
        void Sleep();
    }

    public class RobotWorkerViolation : IWorkerViolation
    {
        public void Work() { }
        public void Eat()  => throw new NotImplementedException();  // robots don't eat
        public void Sleep() => throw new NotImplementedException();
    }

    // CORRECT: segregated interfaces — consumers depend only on what they use
    public interface IWorkable  { void Work(); }
    public interface IFeedable  { void Eat(); }
    public interface IRestable  { void Sleep(); }

    public class HumanWorker : IWorkable, IFeedable, IRestable
    {
        public void Work()  { }
        public void Eat()   { }
        public void Sleep() { }
    }

    public class RobotWorker : IWorkable
    {
        public void Work() { }
        // no forced coupling to biological needs
    }

    // Split read/write repository — common real-world application
    public interface IReadableRepository<T>  { Task<T?> FindAsync(int id); }
    public interface IWritableRepository<T>  { Task SaveAsync(T entity); }

    public interface IRepository<T> : IReadableRepository<T>, IWritableRepository<T> { }
}

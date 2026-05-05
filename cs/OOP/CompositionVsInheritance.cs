namespace Recon.OOP
{
    // ─── INHERITANCE — "is-a" relationship ───────────────────────────────────
    //
    // Works well when the subtype genuinely is a specialisation of the base type
    // and the Liskov Substitution Principle holds.

    public abstract class Animal
    {
        public string Name { get; }

        protected Animal(string name) => Name = name;

        public abstract string Speak();
        public string Describe() => $"{Name} says: {Speak()}";
    }

    public class Dog : Animal
    {
        public Dog(string name) : base(name) { }
        public override string Speak() => "Woof";
    }

    public class Cat : Animal
    {
        public Cat(string name) : base(name) { }
        public override string Speak() => "Meow";
    }

    // ─── WHERE INHERITANCE BREAKS DOWN ───────────────────────────────────────
    //
    // Adding a Robot that can Describe() itself but cannot Speak() in the same
    // sense forces an awkward override or a thrown exception — LSP violation.
    //
    // public class Robot : Animal {
    //     public override string Speak() => throw new NotSupportedException();
    // }

    // ─── COMPOSITION — "has-a" relationship ──────────────────────────────────
    //
    // Behaviours are expressed as interfaces. Types compose the behaviours they
    // need. No forced coupling to a base class hierarchy.

    public interface ISpeakable  { string Speak(); }
    public interface IDescribable { string Describe(); }
    public interface IChargeable  { void Charge(); }

    public class SpeakBehaviour : ISpeakable
    {
        private readonly string _sound;
        public SpeakBehaviour(string sound) => _sound = sound;
        public string Speak() => _sound;
    }

    public class ComposedDog : ISpeakable, IDescribable
    {
        private readonly ISpeakable _voice;
        public string Name { get; }

        public ComposedDog(string name)
        {
            Name = name;
            _voice = new SpeakBehaviour("Woof");
        }

        public string Speak() => _voice.Speak();
        public string Describe() => $"{Name} says: {Speak()}";
    }

    public class Robot : IDescribable, IChargeable
    {
        public string Name { get; }
        public Robot(string name) => Name = name;

        // Robot can Describe itself without being forced to Speak
        public string Describe() => $"{Name} is operational";
        public void Charge() { }
    }

    // ─── RULE OF THUMB ───────────────────────────────────────────────────────
    //
    // Prefer composition when:
    //   - The relationship is "has-a" or "can-do" rather than "is-a"
    //   - Behaviour needs to vary at runtime
    //   - The type hierarchy would grow to accommodate combinations of behaviour
    //   - You want to avoid the fragile base class problem
    //
    // Inheritance is appropriate when:
    //   - The LSP holds cleanly — every subtype can substitute for the base
    //   - The shared behaviour is genuinely invariant across all subtypes
    //   - The hierarchy is shallow and stable
}

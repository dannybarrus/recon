namespace Recon.SOLID
{
    // VIOLATION: Square overrides Rectangle's setters and breaks the area contract
    public class Rectangle
    {
        public virtual int Width { get; set; }
        public virtual int Height { get; set; }
        public int Area() => Width * Height;
    }

    public class SquareViolation : Rectangle
    {
        public override int Width  { set { base.Width = base.Height = value; } }
        public override int Height { set { base.Width = base.Height = value; } }
        // Area() now silently breaks for callers who set Width and Height independently
    }

    // CORRECT: shared abstraction; each shape honours its own contract
    public abstract class Shape
    {
        public abstract int Area();
    }

    public class RectangleShape : Shape
    {
        public int Width { get; }
        public int Height { get; }

        public RectangleShape(int width, int height)
            => (Width, Height) = (width, height);

        public override int Area() => Width * Height;
    }

    public class SquareShape : Shape
    {
        public int Side { get; }

        public SquareShape(int side)
            => Side = side;

        public override int Area() => Side * Side;
    }

    // Any Shape can substitute for another — no surprises
    public class AreaCalculator
    {
        public int TotalArea(IEnumerable<Shape> shapes)
            => shapes.Sum(s => s.Area());
    }
}

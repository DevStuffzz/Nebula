using System;

namespace Nebula
{
    public struct Vec2i
    {
        public int X, Y;

        public Vec2i(int x, int y)
        {
            X = x;
            Y = y;
        }

        public static Vec2i Zero => new Vec2i(0, 0);
        public static Vec2i One => new Vec2i(1, 1);

        public static Vec2i Up => new Vec2i(0, 1);
        public static Vec2i Down => new Vec2i(0, -1);
        public static Vec2i Right => new Vec2i(1, 0);
        public static Vec2i Left => new Vec2i(-1, 0);

        // ----------------------------
        // Math
        // ----------------------------

        public int LengthSquared()
        {
            return X * X + Y * Y;
        }

        public int ManhattanLength()
        {
            return Math.Abs(X) + Math.Abs(Y);
        }

        // ----------------------------
        // Conversions
        // ----------------------------

        public Vec2 ToVec2()
        {
            return new Vec2(X, Y);
        }

        public static Vec2i FromVec2(Vec2 v)
        {
            return new Vec2i(
                (int)MathF.Floor(v.X),
                (int)MathF.Floor(v.Y)
            );
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static int Dot(Vec2i a, Vec2i b)
        {
            return a.X * b.X + a.Y * b.Y;
        }

        public static int DistanceSquared(Vec2i a, Vec2i b)
        {
            return (a - b).LengthSquared();
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vec2i operator +(Vec2i a, Vec2i b)
            => new Vec2i(a.X + b.X, a.Y + b.Y);

        public static Vec2i operator -(Vec2i a, Vec2i b)
            => new Vec2i(a.X - b.X, a.Y - b.Y);

        public static Vec2i operator -(Vec2i v)
            => new Vec2i(-v.X, -v.Y);

        public static Vec2i operator *(Vec2i v, int scalar)
            => new Vec2i(v.X * scalar, v.Y * scalar);

        public static Vec2i operator *(int scalar, Vec2i v)
            => v * scalar;

        public static Vec2i operator /(Vec2i v, int scalar)
            => new Vec2i(v.X / scalar, v.Y / scalar);

        public static bool operator ==(Vec2i a, Vec2i b)
            => a.X == b.X && a.Y == b.Y;

        public static bool operator !=(Vec2i a, Vec2i b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vec2i v)
                return false;

            return this == v;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(X, Y);
        }

        public override string ToString()
        {
            return $"({X}, {Y})";
        }
    }
}

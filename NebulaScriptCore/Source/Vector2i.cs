using System;

namespace Nebula
{
    public struct Vector2i
    {
        public int X, Y;

        public Vector2i(int x, int y)
        {
            X = x;
            Y = y;
        }

        public static Vector2i Zero => new Vector2i(0, 0);
        public static Vector2i One => new Vector2i(1, 1);

        public static Vector2i Up => new Vector2i(0, 1);
        public static Vector2i Down => new Vector2i(0, -1);
        public static Vector2i Right => new Vector2i(1, 0);
        public static Vector2i Left => new Vector2i(-1, 0);

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

        public Vector2 ToVec2()
        {
            return new Vector2(X, Y);
        }

        public static Vector2i FromVec2(Vector2 v)
        {
            return new Vector2i(
                (int)MathF.Floor(v.X),
                (int)MathF.Floor(v.Y)
            );
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static int Dot(Vector2i a, Vector2i b)
        {
            return a.X * b.X + a.Y * b.Y;
        }

        public static int DistanceSquared(Vector2i a, Vector2i b)
        {
            return (a - b).LengthSquared();
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vector2i operator +(Vector2i a, Vector2i b)
            => new Vector2i(a.X + b.X, a.Y + b.Y);

        public static Vector2i operator -(Vector2i a, Vector2i b)
            => new Vector2i(a.X - b.X, a.Y - b.Y);

        public static Vector2i operator -(Vector2i v)
            => new Vector2i(-v.X, -v.Y);

        public static Vector2i operator *(Vector2i v, int scalar)
            => new Vector2i(v.X * scalar, v.Y * scalar);

        public static Vector2i operator *(int scalar, Vector2i v)
            => v * scalar;

        public static Vector2i operator /(Vector2i v, int scalar)
            => new Vector2i(v.X / scalar, v.Y / scalar);

        public static bool operator ==(Vector2i a, Vector2i b)
            => a.X == b.X && a.Y == b.Y;

        public static bool operator !=(Vector2i a, Vector2i b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vector2i v)
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

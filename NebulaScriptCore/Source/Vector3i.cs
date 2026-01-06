using System;

namespace Nebula
{
    public struct Vector3i
    {
        public int X, Y, Z;

        public Vector3i(int x, int y, int z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3i Zero => new Vector3i(0, 0, 0);
        public static Vector3i One => new Vector3i(1, 1, 1);

        public static Vector3i Up => new Vector3i(0, 1, 0);
        public static Vector3i Down => new Vector3i(0, -1, 0);
        public static Vector3i Right => new Vector3i(1, 0, 0);
        public static Vector3i Left => new Vector3i(-1, 0, 0);
        public static Vector3i Forward => new Vector3i(0, 0, 1);
        public static Vector3i Backward => new Vector3i(0, 0, -1);

        // ----------------------------
        // Math
        // ----------------------------

        public int LengthSquared()
        {
            return X * X + Y * Y + Z * Z;
        }

        public int ManhattanLength()
        {
            return Math.Abs(X) + Math.Abs(Y) + Math.Abs(Z);
        }

        // ----------------------------
        // Conversions
        // ----------------------------

        public Vector3 ToVector3()
        {
            return new Vector3(X, Y, Z);
        }

        public static Vector3i FromVector3(Vector3 v)
        {
            return new Vector3i(
                (int)MathF.Floor(v.X),
                (int)MathF.Floor(v.Y),
                (int)MathF.Floor(v.Z)
            );
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static int Dot(Vector3i a, Vector3i b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        public static int DistanceSquared(Vector3i a, Vector3i b)
        {
            return (a - b).LengthSquared();
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vector3i operator +(Vector3i a, Vector3i b)
            => new Vector3i(a.X + b.X, a.Y + b.Y, a.Z + b.Z);

        public static Vector3i operator -(Vector3i a, Vector3i b)
            => new Vector3i(a.X - b.X, a.Y - b.Y, a.Z - b.Z);

        public static Vector3i operator -(Vector3i v)
            => new Vector3i(-v.X, -v.Y, -v.Z);

        public static Vector3i operator *(Vector3i v, int scalar)
            => new Vector3i(v.X * scalar, v.Y * scalar, v.Z * scalar);

        public static Vector3i operator *(int scalar, Vector3i v)
            => v * scalar;

        public static Vector3i operator /(Vector3i v, int scalar)
            => new Vector3i(v.X / scalar, v.Y / scalar, v.Z / scalar);

        public static bool operator ==(Vector3i a, Vector3i b)
            => a.X == b.X && a.Y == b.Y && a.Z == b.Z;

        public static bool operator !=(Vector3i a, Vector3i b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vector3i v)
                return false;

            return this == v;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(X, Y, Z);
        }

        public override string ToString()
        {
            return $"({X}, {Y}, {Z})";
        }
    }
}

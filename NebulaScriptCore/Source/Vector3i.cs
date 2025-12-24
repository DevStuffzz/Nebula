using System;

namespace Nebula
{
    public struct Vec3i
    {
        public int X, Y, Z;

        public Vec3i(int x, int y, int z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vec3i Zero => new Vec3i(0, 0, 0);
        public static Vec3i One => new Vec3i(1, 1, 1);

        public static Vec3i Up => new Vec3i(0, 1, 0);
        public static Vec3i Down => new Vec3i(0, -1, 0);
        public static Vec3i Right => new Vec3i(1, 0, 0);
        public static Vec3i Left => new Vec3i(-1, 0, 0);
        public static Vec3i Forward => new Vec3i(0, 0, 1);
        public static Vec3i Backward => new Vec3i(0, 0, -1);

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

        public static Vec3i FromVector3(Vector3 v)
        {
            return new Vec3i(
                (int)MathF.Floor(v.X),
                (int)MathF.Floor(v.Y),
                (int)MathF.Floor(v.Z)
            );
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static int Dot(Vec3i a, Vec3i b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        public static int DistanceSquared(Vec3i a, Vec3i b)
        {
            return (a - b).LengthSquared();
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vec3i operator +(Vec3i a, Vec3i b)
            => new Vec3i(a.X + b.X, a.Y + b.Y, a.Z + b.Z);

        public static Vec3i operator -(Vec3i a, Vec3i b)
            => new Vec3i(a.X - b.X, a.Y - b.Y, a.Z - b.Z);

        public static Vec3i operator -(Vec3i v)
            => new Vec3i(-v.X, -v.Y, -v.Z);

        public static Vec3i operator *(Vec3i v, int scalar)
            => new Vec3i(v.X * scalar, v.Y * scalar, v.Z * scalar);

        public static Vec3i operator *(int scalar, Vec3i v)
            => v * scalar;

        public static Vec3i operator /(Vec3i v, int scalar)
            => new Vec3i(v.X / scalar, v.Y / scalar, v.Z / scalar);

        public static bool operator ==(Vec3i a, Vec3i b)
            => a.X == b.X && a.Y == b.Y && a.Z == b.Z;

        public static bool operator !=(Vec3i a, Vec3i b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vec3i v)
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

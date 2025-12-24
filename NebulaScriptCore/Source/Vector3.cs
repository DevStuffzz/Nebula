using System;

namespace Nebula
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 Zero => new Vector3(0, 0, 0);
        public static Vector3 One => new Vector3(1, 1, 1);

        public static Vector3 Up => new Vector3(0, 1, 0);
        public static Vector3 Down => new Vector3(0, -1, 0);
        public static Vector3 Right => new Vector3(1, 0, 0);
        public static Vector3 Left => new Vector3(-1, 0, 0);
        public static Vector3 Forward => new Vector3(0, 0, 1);
        public static Vector3 Backward => new Vector3(0, 0, -1);

        // ----------------------------
        // Length
        // ----------------------------

        public float Length()
        {
            return MathF.Sqrt(X * X + Y * Y + Z * Z);
        }

        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z;
        }

        public Vector3 Normalized()
        {
            float length = Length();
            if (length == 0f)
                return Zero;

            return this / length;
        }

        public void Normalize()
        {
            float length = Length();
            if (length == 0f)
                return;

            X /= length;
            Y /= length;
            Z /= length;
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static float Dot(Vector3 a, Vector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        public static Vector3 Cross(Vector3 a, Vector3 b)
        {
            return new Vector3(
                a.Y * b.Z - a.Z * b.Y,
                a.Z * b.X - a.X * b.Z,
                a.X * b.Y - a.Y * b.X
            );
        }

        public static float Distance(Vector3 a, Vector3 b)
        {
            return (a - b).Length();
        }

        public static float DistanceSquared(Vector3 a, Vector3 b)
        {
            return (a - b).LengthSquared();
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            t = Math.Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        public static Vector3 ClampMagnitude(Vector3 v, float maxLength)
        {
            float length = v.Length();
            if (length > maxLength)
                return v.Normalized() * maxLength;

            return v;
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vector3 operator +(Vector3 a, Vector3 b)
            => new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);

        public static Vector3 operator -(Vector3 a, Vector3 b)
            => new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);

        public static Vector3 operator -(Vector3 v)
            => new Vector3(-v.X, -v.Y, -v.Z);

        public static Vector3 operator *(Vector3 a, float scalar)
            => new Vector3(a.X * scalar, a.Y * scalar, a.Z * scalar);

        public static Vector3 operator *(float scalar, Vector3 a)
            => a * scalar;

        public static Vector3 operator /(Vector3 a, float scalar)
            => new Vector3(a.X / scalar, a.Y / scalar, a.Z / scalar);

        public static bool operator ==(Vector3 a, Vector3 b)
            => a.X == b.X && a.Y == b.Y && a.Z == b.Z;

        public static bool operator !=(Vector3 a, Vector3 b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vector3 v)
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

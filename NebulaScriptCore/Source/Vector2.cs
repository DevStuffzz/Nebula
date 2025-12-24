using System;

namespace Nebula
{
    public struct Vec2
    {
        public float X, Y;

        public Vec2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vec2 Zero => new Vec2(0f, 0f);
        public static Vec2 One => new Vec2(1f, 1f);

        public static Vec2 Up => new Vec2(0f, 1f);
        public static Vec2 Down => new Vec2(0f, -1f);
        public static Vec2 Right => new Vec2(1f, 0f);
        public static Vec2 Left => new Vec2(-1f, 0f);

        // ----------------------------
        // Length
        // ----------------------------

        public float Length()
        {
            return MathF.Sqrt(X * X + Y * Y);
        }

        public float LengthSquared()
        {
            return X * X + Y * Y;
        }

        public Vec2 Normalized()
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
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static float Dot(Vec2 a, Vec2 b)
        {
            return a.X * b.X + a.Y * b.Y;
        }

        public static float Distance(Vec2 a, Vec2 b)
        {
            return (a - b).Length();
        }

        public static float DistanceSquared(Vec2 a, Vec2 b)
        {
            return (a - b).LengthSquared();
        }

        public static Vec2 Lerp(Vec2 a, Vec2 b, float t)
        {
            t = Math.Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        public static Vec2 ClampMagnitude(Vec2 v, float maxLength)
        {
            float length = v.Length();
            if (length > maxLength)
                return v.Normalized() * maxLength;

            return v;
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vec2 operator +(Vec2 a, Vec2 b)
            => new Vec2(a.X + b.X, a.Y + b.Y);

        public static Vec2 operator -(Vec2 a, Vec2 b)
            => new Vec2(a.X - b.X, a.Y - b.Y);

        public static Vec2 operator -(Vec2 v)
            => new Vec2(-v.X, -v.Y);

        public static Vec2 operator *(Vec2 v, float scalar)
            => new Vec2(v.X * scalar, v.Y * scalar);

        public static Vec2 operator *(float scalar, Vec2 v)
            => v * scalar;

        public static Vec2 operator /(Vec2 v, float scalar)
            => new Vec2(v.X / scalar, v.Y / scalar);

        public static bool operator ==(Vec2 a, Vec2 b)
            => a.X == b.X && a.Y == b.Y;

        public static bool operator !=(Vec2 a, Vec2 b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vec2 v)
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

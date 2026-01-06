using System;

namespace Nebula
{
    public static class MathUtil
    {
        // ----------------------------
        // Clamp
        // ----------------------------

        public static float Clamp(float value, float min, float max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        public static int Clamp(int value, int min, int max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        // ----------------------------
        // Reflect
        // v - 2 * dot(v, n) * n
        // ----------------------------

        public static Vector2 Reflect(Vector2 v, Vector2 normal)
        {
            Vector2 n = normal.Normalized();
            return v - 2f * Vector2.Dot(v, n) * n;
        }

        public static Vector3 Reflect(Vector3 v, Vector3 normal)
        {
            Vector3 n = normal.Normalized();
            return v - 2f * Vector3.Dot(v, n) * n;
        }

        public static Vector2i Reflect(Vector2i v, Vector2i normal)
        {
            // Integer reflection: convert to float, reflect, round back
            Vector2 result = Reflect(v.ToVec2(), normal.ToVec2());
            return Vector2i.FromVec2(result);
        }

        public static Vector3i Reflect(Vector3i v, Vector3i normal)
        {
            Vector3 result = Reflect(v.ToVector3(), normal.ToVector3());
            return Vector3i.FromVector3(result);
        }

        // ----------------------------
        // Project
        // ----------------------------

        public static Vector2 Project(Vector2 v, Vector2 onNormal)
        {
            float denom = Vector2.Dot(onNormal, onNormal);
            if (denom == 0f)
                return Vector2.Zero;

            return onNormal * (Vector2.Dot(v, onNormal) / denom);
        }

        public static Vector3 Project(Vector3 v, Vector3 onNormal)
        {
            float denom = Vector3.Dot(onNormal, onNormal);
            if (denom == 0f)
                return Vector3.Zero;

            return onNormal * (Vector3.Dot(v, onNormal) / denom);
        }

        // ----------------------------
        // MoveTowards
        // ----------------------------

        public static float MoveTowards(float current, float target, float maxDelta)
        {
            if (Math.Abs(target - current) <= maxDelta)
                return target;

            return current + Math.Sign(target - current) * maxDelta;
        }

        public static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDelta)
        {
            Vector2 delta = target - current;
            float dist = delta.Length();

            if (dist <= maxDelta || dist == 0f)
                return target;

            return current + delta / dist * maxDelta;
        }

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDelta)
        {
            Vector3 delta = target - current;
            float dist = delta.Length();

            if (dist <= maxDelta || dist == 0f)
                return target;

            return current + delta / dist * maxDelta;
        }

        // ----------------------------
        // Lerp (centralized)
        // ----------------------------

        public static float Lerp(float a, float b, float t)
        {
            t = Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
        {
            t = Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            t = Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        // ----------------------------
        // Abs
        // ----------------------------

        public static Vector2i Abs(Vector2i v)
        {
            return new Vector2i(Math.Abs(v.X), Math.Abs(v.Y));
        }

        public static Vector3i Abs(Vector3i v)
        {
            return new Vector3i(Math.Abs(v.X), Math.Abs(v.Y), Math.Abs(v.Z));
        }

        // ----------------------------
        // Min / Max
        // ----------------------------

        public static Vector2 Min(Vector2 a, Vector2 b)
        {
            return new Vector2(
                MathF.Min(a.X, b.X),
                MathF.Min(a.Y, b.Y)
            );
        }

        public static Vector2 Max(Vector2 a, Vector2 b)
        {
            return new Vector2(
                MathF.Max(a.X, b.X),
                MathF.Max(a.Y, b.Y)
            );
        }

        public static Vector3 Min(Vector3 a, Vector3 b)
        {
            return new Vector3(
                MathF.Min(a.X, b.X),
                MathF.Min(a.Y, b.Y),
                MathF.Min(a.Z, b.Z)
            );
        }

        public static Vector3 Max(Vector3 a, Vector3 b)
        {
            return new Vector3(
                MathF.Max(a.X, b.X),
                MathF.Max(a.Y, b.Y),
                MathF.Max(a.Z, b.Z)
            );
        }

        public static Vector2i Min(Vector2i a, Vector2i b)
        {
            return new Vector2i(
                Math.Min(a.X, b.X),
                Math.Min(a.Y, b.Y)
            );
        }

        public static Vector2i Max(Vector2i a, Vector2i b)
        {
            return new Vector2i(
                Math.Max(a.X, b.X),
                Math.Max(a.Y, b.Y)
            );
        }

        public static Vector3i Min(Vector3i a, Vector3i b)
        {
            return new Vector3i(
                Math.Min(a.X, b.X),
                Math.Min(a.Y, b.Y),
                Math.Min(a.Z, b.Z)
            );
        }

        public static Vector3i Max(Vector3i a, Vector3i b)
        {
            return new Vector3i(
                Math.Max(a.X, b.X),
                Math.Max(a.Y, b.Y),
                Math.Max(a.Z, b.Z)
            );
        }
    }
}

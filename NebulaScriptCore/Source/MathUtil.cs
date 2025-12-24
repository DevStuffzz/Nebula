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

        public static Vec2 Reflect(Vec2 v, Vec2 normal)
        {
            Vec2 n = normal.Normalized();
            return v - 2f * Vec2.Dot(v, n) * n;
        }

        public static Vector3 Reflect(Vector3 v, Vector3 normal)
        {
            Vector3 n = normal.Normalized();
            return v - 2f * Vector3.Dot(v, n) * n;
        }

        public static Vec2i Reflect(Vec2i v, Vec2i normal)
        {
            // Integer reflection: convert to float, reflect, round back
            Vec2 result = Reflect(v.ToVec2(), normal.ToVec2());
            return Vec2i.FromVec2(result);
        }

        public static Vec3i Reflect(Vec3i v, Vec3i normal)
        {
            Vector3 result = Reflect(v.ToVector3(), normal.ToVector3());
            return Vec3i.FromVector3(result);
        }

        // ----------------------------
        // Project
        // ----------------------------

        public static Vec2 Project(Vec2 v, Vec2 onNormal)
        {
            float denom = Vec2.Dot(onNormal, onNormal);
            if (denom == 0f)
                return Vec2.Zero;

            return onNormal * (Vec2.Dot(v, onNormal) / denom);
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

        public static Vec2 MoveTowards(Vec2 current, Vec2 target, float maxDelta)
        {
            Vec2 delta = target - current;
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

        public static Vec2 Lerp(Vec2 a, Vec2 b, float t)
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

        public static Vec2i Abs(Vec2i v)
        {
            return new Vec2i(Math.Abs(v.X), Math.Abs(v.Y));
        }

        public static Vec3i Abs(Vec3i v)
        {
            return new Vec3i(Math.Abs(v.X), Math.Abs(v.Y), Math.Abs(v.Z));
        }

        // ----------------------------
        // Min / Max
        // ----------------------------

        public static Vec2 Min(Vec2 a, Vec2 b)
        {
            return new Vec2(
                MathF.Min(a.X, b.X),
                MathF.Min(a.Y, b.Y)
            );
        }

        public static Vec2 Max(Vec2 a, Vec2 b)
        {
            return new Vec2(
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

        public static Vec2i Min(Vec2i a, Vec2i b)
        {
            return new Vec2i(
                Math.Min(a.X, b.X),
                Math.Min(a.Y, b.Y)
            );
        }

        public static Vec2i Max(Vec2i a, Vec2i b)
        {
            return new Vec2i(
                Math.Max(a.X, b.X),
                Math.Max(a.Y, b.Y)
            );
        }

        public static Vec3i Min(Vec3i a, Vec3i b)
        {
            return new Vec3i(
                Math.Min(a.X, b.X),
                Math.Min(a.Y, b.Y),
                Math.Min(a.Z, b.Z)
            );
        }

        public static Vec3i Max(Vec3i a, Vec3i b)
        {
            return new Vec3i(
                Math.Max(a.X, b.X),
                Math.Max(a.Y, b.Y),
                Math.Max(a.Z, b.Z)
            );
        }
    }
}

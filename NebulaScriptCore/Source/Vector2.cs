using System;

namespace Nebula
{
    public struct Vector2
    {
        public float x, y;

        // Uppercase aliases for compatibility
        public float X { get => x; set => x = value; }
        public float Y { get => y; set => y = value; }

        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public static Vector2 zero => new Vector2(0f, 0f);
        public static Vector2 one => new Vector2(1f, 1f);
        public static Vector2 Zero => zero;
        public static Vector2 One => one;

        public static Vector2 up => new Vector2(0f, 1f);
        public static Vector2 down => new Vector2(0f, -1f);
        public static Vector2 right => new Vector2(1f, 0f);
        public static Vector2 left => new Vector2(-1f, 0f);

        // ----------------------------
        // Length
        // ----------------------------

        public float magnitude => MathF.Sqrt(x * x + y * y);
        public float sqrMagnitude => x * x + y * y;
        
        // Uppercase aliases
        public float Length() => magnitude;
        public float SqrMagnitude() => sqrMagnitude;

        public Vector2 normalized
        {
            get
            {
                float length = magnitude;
                if (length == 0f)
                    return zero;
                return this / length;
            }
        }
        
        public Vector2 Normalized() => normalized;

        public void Normalize()
        {
            float length = magnitude;
            if (length == 0f)
                return;

            x /= length;
            y /= length;
        }

        // ----------------------------
        // Static helpers
        // ----------------------------

        public static float Dot(Vector2 a, Vector2 b)
        {
            return a.x * b.x + a.y * b.y;
        }

        public static float Distance(Vector2 a, Vector2 b)
        {
            return (a - b).magnitude;
        }

        public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
        {
            t = Math.Clamp(t, 0f, 1f);
            return a + (b - a) * t;
        }

        public static Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t)
        {
            return a + (b - a) * t;
        }

        public static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta)
        {
            Vector2 delta = target - current;
            float dist = delta.magnitude;

            if (dist <= maxDistanceDelta || dist == 0f)
                return target;

            return current + delta / dist * maxDistanceDelta;
        }

        public static Vector2 ClampMagnitude(Vector2 v, float maxLength)
        {
            float length = v.magnitude;
            if (length > maxLength)
                return v.normalized * maxLength;

            return v;
        }

        public static Vector2 Scale(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * b.x, a.y * b.y);
        }

        public static Vector2 Reflect(Vector2 v, Vector2 normal)
        {
            Vector2 n = normal.normalized;
            return v - 2f * Dot(v, n) * n;
        }

        public static Vector2 Perpendicular(Vector2 v)
        {
            return new Vector2(-v.y, v.x);
        }

        public static float Angle(Vector2 from, Vector2 to)
        {
            float dot = Dot(from.normalized, to.normalized);
            return MathF.Acos(Math.Clamp(dot, -1f, 1f)) * 180f / MathF.PI;
        }

        public static float SignedAngle(Vector2 from, Vector2 to)
        {
            float angle = Angle(from, to);
            float sign = MathF.Sign(from.x * to.y - from.y * to.x);
            return angle * sign;
        }

        // ----------------------------
        // Operators
        // ----------------------------

        public static Vector2 operator +(Vector2 a, Vector2 b)
            => new Vector2(a.x + b.x, a.y + b.y);

        public static Vector2 operator -(Vector2 a, Vector2 b)
            => new Vector2(a.x - b.x, a.y - b.y);

        public static Vector2 operator -(Vector2 v)
            => new Vector2(-v.x, -v.y);

        public static Vector2 operator *(Vector2 v, float scalar)
            => new Vector2(v.x * scalar, v.y * scalar);

        public static Vector2 operator *(float scalar, Vector2 v)
            => v * scalar;

        public static Vector2 operator /(Vector2 v, float scalar)
            => new Vector2(v.x / scalar, v.y / scalar);

        public static bool operator ==(Vector2 a, Vector2 b)
        {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            return (dx * dx + dy * dy) < 0.0000001f;
        }

        public static bool operator !=(Vector2 a, Vector2 b)
            => !(a == b);

        public override bool Equals(object obj)
        {
            if (obj is not Vector2 v)
                return false;

            return this == v;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(x, y);
        }

        public override string ToString()
        {
            return $"({x}, {y})";
        }

        public string ToString(string format)
        {
            return $"({x.ToString(format)}, {y.ToString(format)})";
        }
    }
}

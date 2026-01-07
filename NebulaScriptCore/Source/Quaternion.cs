using System;

namespace Nebula
{
    /// <summary>
    /// Quaternion representation for rotations
    /// </summary>
    public struct Quaternion
    {
        // Helper methods for Mono compatibility
        private static float Sqrt(float value)
        {
            return (float)System.Math.Sqrt(value);
        }

        private static float Sin(float value)
        {
            return (float)System.Math.Sin(value);
        }

        private static float Cos(float value)
        {
            return (float)System.Math.Cos(value);
        }

        private static float Acos(float value)
        {
            return (float)System.Math.Acos(value);
        }

        private static float Asin(float value)
        {
            return (float)System.Math.Asin(value);
        }

        private static float Atan2(float y, float x)
        {
            return (float)System.Math.Atan2(y, x);
        }

        private const float PI = 3.14159265359f;

        private static float Clamp(float value, float min, float max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        private static float CopySign(float value, float sign)
        {
            float absValue = value >= 0 ? value : -value;
            return sign >= 0 ? absValue : -absValue;
        }

        private static float Min(float a, float b)
        {
            return a < b ? a : b;
        }

        private static float Abs(float value)
        {
            return value >= 0 ? value : -value;
        }

        public float x, y, z, w;

        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public static Quaternion identity => new Quaternion(0f, 0f, 0f, 1f);

        /// <summary>
        /// Returns the length of the quaternion
        /// </summary>
        public float magnitude => Sqrt(x * x + y * y + z * z + w * w);

        /// <summary>
        /// Returns a normalized version of this quaternion
        /// </summary>
        public Quaternion normalized
        {
            get
            {
                float mag = magnitude;
                if (mag < 0.00001f)
                    return identity;
                return new Quaternion(x / mag, y / mag, z / mag, w / mag);
            }
        }

        /// <summary>
        /// Converts this quaternion to Euler angles (in degrees)
        /// </summary>
        public Vector3 eulerAngles
        {
            get { return ToEulerAngles(this); }
            set { this = FromEulerAngles(value); }
        }

        /// <summary>
        /// Creates a rotation from Euler angles (in degrees)
        /// </summary>
        public static Quaternion Euler(float x, float y, float z)
        {
            return FromEulerAngles(new Vector3(x, y, z));
        }

        /// <summary>
        /// Creates a rotation from Euler angles (in degrees)
        /// </summary>
        public static Quaternion Euler(Vector3 euler)
        {
            return FromEulerAngles(euler);
        }

        /// <summary>
        /// Creates a rotation which rotates from fromDirection to toDirection
        /// </summary>
        public static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection)
        {
            Vector3 axis = Vector3.Cross(fromDirection, toDirection);
            float angle = Acos(Vector3.Dot(fromDirection.Normalized(), toDirection.Normalized()));
            return AngleAxis(angle * 180f / PI, axis);
        }

        /// <summary>
        /// Creates a rotation with the specified forward and upwards directions
        /// </summary>
        public static Quaternion LookRotation(Vector3 forward, Vector3 up)
        {
            forward = forward.Normalized();
            Vector3 right = Vector3.Cross(up, forward).Normalized();
            up = Vector3.Cross(forward, right);

            float m00 = right.X;
            float m01 = right.Y;
            float m02 = right.Z;
            float m10 = up.X;
            float m11 = up.Y;
            float m12 = up.Z;
            float m20 = forward.X;
            float m21 = forward.Y;
            float m22 = forward.Z;

            float trace = m00 + m11 + m22;
            Quaternion q = new Quaternion();

            if (trace > 0f)
            {
                float s = Sqrt(trace + 1f) * 2f;
                q.w = 0.25f * s;
                q.x = (m21 - m12) / s;
                q.y = (m02 - m20) / s;
                q.z = (m10 - m01) / s;
            }
            else if (m00 > m11 && m00 > m22)
            {
                float s = Sqrt(1f + m00 - m11 - m22) * 2f;
                q.w = (m21 - m12) / s;
                q.x = 0.25f * s;
                q.y = (m01 + m10) / s;
                q.z = (m02 + m20) / s;
            }
            else if (m11 > m22)
            {
                float s = Sqrt(1f + m11 - m00 - m22) * 2f;
                q.w = (m02 - m20) / s;
                q.x = (m01 + m10) / s;
                q.y = 0.25f * s;
                q.z = (m12 + m21) / s;
            }
            else
            {
                float s = Sqrt(1f + m22 - m00 - m11) * 2f;
                q.w = (m10 - m01) / s;
                q.x = (m02 + m20) / s;
                q.y = (m12 + m21) / s;
                q.z = 0.25f * s;
            }

            return q;
        }

        /// <summary>
        /// Creates a rotation of angle degrees around axis
        /// </summary>
        public static Quaternion AngleAxis(float angle, Vector3 axis)
        {
            axis = axis.Normalized();
            float halfAngle = angle * PI / 360f;
            float s = Sin(halfAngle);

            return new Quaternion(
                axis.X * s,
                axis.Y * s,
                axis.Z * s,
                Cos(halfAngle)
            );
        }

        /// <summary>
        /// Spherically interpolates between two quaternions
        /// </summary>
        public static Quaternion Slerp(Quaternion a, Quaternion b, float t)
        {
            t = Clamp(t, 0f, 1f);

            float dot = Dot(a, b);

            if (dot < 0f)
            {
                b = new Quaternion(-b.x, -b.y, -b.z, -b.w);
                dot = -dot;
            }

            if (dot > 0.9995f)
            {
                return Lerp(a, b, t);
            }

            float theta = Acos(dot);
            float sinTheta = Sin(theta);

            float wa = Sin((1f - t) * theta) / sinTheta;
            float wb = Sin(t * theta) / sinTheta;

            return new Quaternion(
                wa * a.x + wb * b.x,
                wa * a.y + wb * b.y,
                wa * a.z + wb * b.z,
                wa * a.w + wb * b.w
            );
        }

        /// <summary>
        /// Linearly interpolates between two quaternions
        /// </summary>
        public static Quaternion Lerp(Quaternion a, Quaternion b, float t)
        {
            t = Clamp(t, 0f, 1f);
            return new Quaternion(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            ).normalized;
        }

        /// <summary>
        /// Dot product of two quaternions
        /// </summary>
        public static float Dot(Quaternion a, Quaternion b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        /// <summary>
        /// Returns the angle in degrees between two rotations
        /// </summary>
        public static float Angle(Quaternion a, Quaternion b)
        {
            float dot = Dot(a, b);
            return Acos(Min(Abs(dot), 1f)) * 2f * 180f / PI;
        }

        // Operators
        public static Quaternion operator *(Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion(
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
                lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
                lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
            );
        }

        public static Vector3 operator *(Quaternion rotation, Vector3 point)
        {
            float x2 = rotation.x * 2f;
            float y2 = rotation.y * 2f;
            float z2 = rotation.z * 2f;
            float xx2 = rotation.x * x2;
            float yy2 = rotation.y * y2;
            float zz2 = rotation.z * z2;
            float xy2 = rotation.x * y2;
            float xz2 = rotation.x * z2;
            float yz2 = rotation.y * z2;
            float wx2 = rotation.w * x2;
            float wy2 = rotation.w * y2;
            float wz2 = rotation.w * z2;

            return new Vector3(
                (1f - (yy2 + zz2)) * point.X + (xy2 - wz2) * point.Y + (xz2 + wy2) * point.Z,
                (xy2 + wz2) * point.X + (1f - (xx2 + zz2)) * point.Y + (yz2 - wx2) * point.Z,
                (xz2 - wy2) * point.X + (yz2 + wx2) * point.Y + (1f - (xx2 + yy2)) * point.Z
            );
        }

        public static bool operator ==(Quaternion lhs, Quaternion rhs)
        {
            return Dot(lhs, rhs) > 0.999999f;
        }

        public static bool operator !=(Quaternion lhs, Quaternion rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj)
        {
            if (obj is not Quaternion q)
                return false;
            return this == q;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(x, y, z, w);
        }

        public override string ToString()
        {
            return $"({x:F3}, {y:F3}, {z:F3}, {w:F3})";
        }

        // Helper methods for Euler conversion
        private static Quaternion FromEulerAngles(Vector3 euler)
        {
            // Convert degrees to radians
            float xRad = euler.X * PI / 180f;
            float yRad = euler.Y * PI / 180f;
            float zRad = euler.Z * PI / 180f;

            float cx = Cos(xRad * 0.5f);
            float sx = Sin(xRad * 0.5f);
            float cy = Cos(yRad * 0.5f);
            float sy = Sin(yRad * 0.5f);
            float cz = Cos(zRad * 0.5f);
            float sz = Sin(zRad * 0.5f);

            return new Quaternion(
                sx * cy * cz + cx * sy * sz,
                cx * sy * cz - sx * cy * sz,
                cx * cy * sz + sx * sy * cz,
                cx * cy * cz - sx * sy * sz
            );
        }

        private static Vector3 ToEulerAngles(Quaternion q)
        {
            Vector3 euler = new Vector3();

            // Roll (x-axis rotation)
            float sinr_cosp = 2f * (q.w * q.x + q.y * q.z);
            float cosr_cosp = 1f - 2f * (q.x * q.x + q.y * q.y);
            euler.X = Atan2(sinr_cosp, cosr_cosp);

            // Pitch (y-axis rotation)
            float sinp = 2f * (q.w * q.y - q.z * q.x);
            if (Abs(sinp) >= 1f)
                euler.Y = CopySign(PI / 2f, sinp);
            else
                euler.Y = Asin(sinp);

            // Yaw (z-axis rotation)
            float siny_cosp = 2f * (q.w * q.z + q.x * q.y);
            float cosy_cosp = 1f - 2f * (q.y * q.y + q.z * q.z);
            euler.Z = Atan2(siny_cosp, cosy_cosp);

            // Convert to degrees
            euler.X *= 180f / PI;
            euler.Y *= 180f / PI;
            euler.Z *= 180f / PI;

            return euler;
        }
    }
}

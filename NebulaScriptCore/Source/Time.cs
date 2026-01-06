using System.Runtime.CompilerServices;

namespace Nebula
{
    /// <summary>
    /// Unity-like Time class for accessing time-related values
    /// </summary>
    public static class Time
    {
        /// <summary>
        /// The time in seconds it took to complete the last frame (Read Only)
        /// </summary>
        public static float deltaTime
        {
            get { return GetDeltaTime(); }
        }

        /// <summary>
        /// The time at the beginning of this frame (Read Only)
        /// </summary>
        public static float time
        {
            get { return GetTime(); }
        }

        /// <summary>
        /// The scale at which time passes (can be used for slow motion effects)
        /// </summary>
        public static float timeScale
        {
            get { return GetTimeScale(); }
            set { SetTimeScale(value); }
        }

        /// <summary>
        /// The interval in seconds at which physics and other fixed frame rate updates are performed
        /// </summary>
        public static float fixedDeltaTime
        {
            get { return GetFixedDeltaTime(); }
            set { SetFixedDeltaTime(value); }
        }

        /// <summary>
        /// The real time in seconds since the game started (Read Only). Not affected by timeScale
        /// </summary>
        public static float unscaledTime
        {
            get { return GetUnscaledTime(); }
        }

        /// <summary>
        /// The timeScale-independent interval in seconds from the last frame to the current one (Read Only)
        /// </summary>
        public static float unscaledDeltaTime
        {
            get { return GetUnscaledDeltaTime(); }
        }

        /// <summary>
        /// The total number of frames that have passed (Read Only)
        /// </summary>
        public static int frameCount
        {
            get { return GetFrameCount(); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTimeScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTimeScale(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetFixedDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFixedDeltaTime(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetUnscaledTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetUnscaledDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int GetFrameCount();
    }
}

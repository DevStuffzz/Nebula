using System.Runtime.CompilerServices;

namespace Nebula
{
    public static class Input
    {
        // Keyboard input
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyPressed(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyDown(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyReleased(KeyCode key);

        // Mouse button input
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonPressed(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonDown(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonReleased(int button);

        // Mouse position and movement
        public static Vector2 mousePosition
        {
            get
            {
                GetMousePosition(out Vector2 pos);
                return pos;
            }
        }

        public static Vector2 mouseDelta
        {
            get
            {
                GetMouseDelta(out Vector2 delta);
                return delta;
            }
        }

        public static Vector2 mouseScrollDelta
        {
            get
            {
                GetMouseScrollDelta(out Vector2 scroll);
                return scroll;
            }
        }

        // Axis input (for input mapping)
        public static float GetAxis(string axisName)
        {
            return GetAxisValue(axisName);
        }

        public static float GetAxisRaw(string axisName)
        {
            return GetAxisRawValue(axisName);
        }

        // Internal calls
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetMousePosition(out Vector2 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetMouseDelta(out Vector2 delta);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetMouseScrollDelta(out Vector2 scroll);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetAxisValue(string axisName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetAxisRawValue(string axisName);
    }

    /// <summary>
    /// Mouse button constants
 /// </summary>
    public static class MouseButton
    {
        public const int Left = 0;
        public const int Right = 1;
        public const int Middle = 2;
    }
}

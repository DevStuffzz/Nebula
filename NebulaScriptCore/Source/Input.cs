using System.Runtime.CompilerServices;

namespace Nebula
{
    public static class Input
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyPressed(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyDown(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyReleased(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonPressed(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonDown(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonReleased(int button);
    }
}

using System.Runtime.CompilerServices;

namespace Nebula
{
    public static class Console
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogError(string message);
    }
}

using System.Runtime.CompilerServices;

namespace Nebula
{
    public static class Log
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogInfo(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogError(string message);
    }
}

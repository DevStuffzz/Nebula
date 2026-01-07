using System.Runtime.CompilerServices;

namespace Nebula
{
    public class Application
    {

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Exit(int code);
    }
}

using System.Runtime.CompilerServices;

namespace Nebula
{
    public enum CursorLockMode
    {
        None = 0,       // Cursor is free to move and visible
        Locked = 1,     // Cursor is locked to center and hidden
        Confined = 2    // Cursor is confined to game window but visible
    }

    public static class Cursor
    {
        private static CursorLockMode _lockMode = CursorLockMode.None;
        private static bool _visible = true;

        public static CursorLockMode lockMode
        {
            get { return _lockMode; }
            set
            {
                if (_lockMode != value)
                {
                    _lockMode = value;
                    SetCursorLockMode((int)value);
                }
            }
        }

        public static bool visible
        {
            get { return _visible; }
            set
            {
                if (_visible != value)
                {
                    _visible = value;
                    SetCursorVisible(value);
                }
            }
        }

        // Internal calls
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCursorLockMode(int mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCursorVisible(bool visible);
    }
}

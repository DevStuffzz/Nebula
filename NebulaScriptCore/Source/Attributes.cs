using System;

namespace Nebula
{
    /// <summary>
    /// Makes a field serializable and visible in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class SerializeFieldAttribute : Attribute
    {
    }

    /// <summary>
    /// Makes a public field hidden in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class HideInInspectorAttribute : Attribute
    {
    }

    /// <summary>
    /// Adds a header above a field in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class HeaderAttribute : Attribute
    {
        public string Header { get; }

        public HeaderAttribute(string header)
        {
            Header = header;
        }
    }

    /// <summary>
    /// Adds a tooltip to a field in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class TooltipAttribute : Attribute
    {
        public string Tooltip { get; }

        public TooltipAttribute(string tooltip)
        {
            Tooltip = tooltip;
        }
    }

    /// <summary>
    /// Adds spacing above a field in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class SpaceAttribute : Attribute
    {
        public float Height { get; }

        public SpaceAttribute(float height = 8f)
        {
            Height = height;
        }
    }

    /// <summary>
    /// Restricts a float or int field to a specific range in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class RangeAttribute : Attribute
    {
        public float Min { get; }
        public float Max { get; }

        public RangeAttribute(float min, float max)
        {
            Min = min;
            Max = max;
        }
    }

    /// <summary>
    /// Shows a text area for editing long strings in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class TextAreaAttribute : Attribute
    {
        public int MinLines { get; }
        public int MaxLines { get; }

        public TextAreaAttribute(int minLines = 3, int maxLines = 10)
        {
            MinLines = minLines;
            MaxLines = maxLines;
        }
    }

    /// <summary>
    /// Makes a field read-only in the inspector (shows the value but can't be edited)
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class ReadOnlyAttribute : Attribute
    {
    }

    /// <summary>
    /// Allows selection of a color in the inspector
    /// </summary>
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public class ColorUsageAttribute : Attribute
    {
        public bool ShowAlpha { get; }
        public bool HDR { get; }

        public ColorUsageAttribute(bool showAlpha = true, bool hdr = false)
        {
            ShowAlpha = showAlpha;
            HDR = hdr;
        }
    }
}

using System;
namespace FreeSCADA.Interfaces
{
    public enum AlarmLevel
    {
        Green,
        Yellow,
        Orange,
        Red
    }
    public interface IEventDefinition
    {
        string Text { get; }
        AlarmLevel Level { get; }
        string Type { get; }
        int Id { get; }
    }
}

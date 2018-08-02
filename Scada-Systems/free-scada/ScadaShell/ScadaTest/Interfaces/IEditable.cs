using System;
namespace ScadaShell.Interfaces
{
    public interface IEditable
    {
        object Value { get; set; }
        string Description { get; }
    }
}
   
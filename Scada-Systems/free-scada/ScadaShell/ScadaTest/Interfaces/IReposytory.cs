using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ScadaShell.Interfaces
{
    public interface IReposytory<T>
    {
        T Getentity(string id);
        List<T> GetAll();
    } 
}

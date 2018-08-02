using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.EnterpriseLibrary.Logging;
using Microsoft.Practices.EnterpriseLibrary.Common.Configuration;



namespace FreeSCADA.Common.Logging
{
    public class ELLogger:ILogger
    {
        public ELLogger()
        {
        }


        public void Log(Severity severity, string message)
        {
            LogWriter writer = EnterpriseLibraryContainer.Current.GetInstance<LogWriter>();
            LogEntry log = new LogEntry();
            log.Message = message;
            log.Categories.Add(Category.General);
            log.Priority = (int)severity;
            writer.Write(log);
        }

        public void LogError(string message)
        {
            Log(Severity.Error, message);
        }

        public void LogInfo(string message)
        {
            Log(Severity.Information, message);
        }

        public void LogWarning(string message)
        {
            Log(Severity.Warning, message);
        }
    }

    public struct Category
    {
        public const string General = "General";
        public const string Critical = "Critical";
        public const string Trace = "Trace";
    }
}

using System;
using System.Windows.Input;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Infrastructure;
using FreeSCADA.Archiver;

namespace ScadaShell.ViewModel
{
    public class ServiceViewModel:WorkspaceViewModel
    {
        

        public ServiceViewModel()
        {
            CopyDatabase= new CommandViewModel("Копіювати базу",new RelayCommand(
                p => CopyDabase(),
                p => { return true; },
                "Tune"));
            DeleteDatabase= new CommandViewModel("Видалити базу",new RelayCommand(
                p => DeleteDabase(),
                p => { return true; },
                "Tune"));
          
        }
        public CommandViewModel DeleteDatabase
        {
            get;
            protected set;
        }
        public CommandViewModel CopyDatabase
        {
            get;
            protected set;
        }
     
        private void DeleteDabase()
        {
            ScadaShell.Infrastructure.Shell.InteropSHFileOperation ish = new ScadaShell.Infrastructure.Shell.InteropSHFileOperation();
            ish.wFunc = ScadaShell.Infrastructure.Shell.InteropSHFileOperation.FO_Func.FO_DELETE;
            ArchiverMain.Current.Stop();
            ish.pFrom = @"d:\\archive.db3";
            ish.lpszProgressTitle = "Delete database...";
            ish.Execute();
            ArchiverMain.Current.Start();
        }

        private void CopyDabase() 
        {

            ScadaShell.Infrastructure.Shell.BrowseForFolder bff = new ScadaShell.Infrastructure.Shell.BrowseForFolder();
            string res = bff.SelectFolder("Виберите теку", "", new IntPtr(0));

            if (res != null)
            {
                ScadaShell.Infrastructure.Shell.InteropSHFileOperation ish = new ScadaShell.Infrastructure.Shell.InteropSHFileOperation();
                ish.pFrom = @"d:\\archive.db3";
                ish.pTo = res + @"\\archive.db3";
                ish.lpszProgressTitle = "Copying database...";
                ish.Execute();
            }
        }

    }
}

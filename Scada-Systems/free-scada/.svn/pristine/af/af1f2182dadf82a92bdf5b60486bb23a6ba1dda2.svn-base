using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;


namespace FreeSCADA.Archiver
{

    class DbAccessor
    {
        DbProviderFactory dbProviderFactory;
        DbConnection dbConnection;
        DatabaseSettings _settings;
        DbTransaction _tr;
        public DbAccessor(DatabaseSettings settings)
        {
            _settings = settings;
            dbProviderFactory = DatabaseFactory.Get(settings.DbProvider);
            dbConnection = dbProviderFactory.CreateConnection();
            dbConnection.ConnectionString = _settings.CreateConnectionString();
        }

        public bool Open()
        {
            try
            {
                
                dbConnection.Open();
            }
            catch (System.Data.Common.DbException e)
            {
                dbConnection.Close();
                Env.Current.Logger.LogError(e.Message);
                return false;
            }


            return true;
        }

        public bool IsTableExists(string tableName)
        {

            DbCommand cmd = dbConnection.CreateCommand();
            cmd.CommandText = String.Format("SELECT * FROM {0};", tableName);
            try
            {
                cmd.ExecuteNonQuery();
            }
            catch (System.Data.Common.DbException e)
            {
                return false;
            }
            return true;
        }

        public void Close()
        {
            if (dbConnection != null)
            {
                dbConnection.Close();
                dbConnection = null;
                dbProviderFactory = null;
            }
        }

        public void BeginTransaction()
        {
             _tr= dbConnection.BeginTransaction();
        }
        public void EndTransaction()
        {
            _tr.Commit();
        }
        public void ExeсCommand(string commandStr)
        {
            DataTable data = new DataTable();

            DbCommand command = dbConnection.CreateCommand();
            
            command.CommandText = commandStr;
            command.ExecuteNonQuery();
        }
        public DataTable ExeсSelectCommand(string selectCommand)
        {
            DataTable data = new DataTable();
            DbDataAdapter dataAdapter = dbProviderFactory.CreateDataAdapter();
            DbCommand command = dbConnection.CreateCommand();
            command.CommandText = selectCommand;
            dataAdapter.MissingMappingAction = MissingMappingAction.Passthrough;
            dataAdapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            dataAdapter.SelectCommand = command;
            dataAdapter.Fill(data);

            return data;
        }

    }
}

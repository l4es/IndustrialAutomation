package br.org.scadabr.db.spring;

import java.sql.Connection;
import java.sql.Connection;
import java.sql.SQLException;

@Deprecated
public interface ConnectionCallbackVoid {

    public void doInConnection(Connection cnctn) throws SQLException;
}

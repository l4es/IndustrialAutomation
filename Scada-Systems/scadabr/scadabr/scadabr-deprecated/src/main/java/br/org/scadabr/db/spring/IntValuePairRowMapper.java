package br.org.scadabr.db.spring;

import br.org.scadabr.db.IntValuePair;
import java.sql.ResultSet;
import java.sql.SQLException;
import org.springframework.jdbc.core.RowMapper;

public class IntValuePairRowMapper implements RowMapper<IntValuePair> {

    private int intIndex;
    private int valueIndex;

    public IntValuePairRowMapper() {
        this(1, 2);
    }

    public IntValuePairRowMapper(int intIndex, int valueIndex) {
        this.intIndex = intIndex;
        this.valueIndex = valueIndex;
    }

    @Override
    public IntValuePair mapRow(ResultSet rs, int rowNum) throws SQLException {
        return new IntValuePair(rs.getInt(intIndex), rs.getString(valueIndex));
    }
}

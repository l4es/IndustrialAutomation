/*
 * $Id$
 */

/*
 * Table with variable information.
 */
CREATE TABLE variables (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	path TEXT,
	type INTEGER,
	size INTEGER
);

/*
 * Index for selecting IDs for a certain variable.
 */
CREATE INDEX path_idx on variables(path);

/*
 * Table with data.
 */
CREATE TABLE data (
	id INTEGER,
	time INTEGER,
	value BLOB
);

/*
 * Index for querying data for a certain time.
 *
 * Significantly improves the performance of queryLast(), used during initial
 * value selection and the snapshot command. So this is the most important
 * index!
 */
CREATE INDEX time_idx ON data(id, time);

/*
 * Index for history command.
 */
CREATE INDEX history_idx on data(time);

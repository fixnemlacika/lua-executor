SQLite3 =
{
	Init = function(database, flags)
		return sqlite3_init(tostring(database), tonumber(flags));
	end,

	Deinit = function(sqlite3)
		sqlite3_deinit(sqlite3);
	end,

	IsOpen = function(sqlite3)
		return sqlite3_is_open(sqlite3);
	end,

	Open = function(sqlite3)
		return sqlite3_open(sqlite3);
	end,

	Close = function(sqlite3)
		sqlite3_close(sqlite3);
	end,

	ExecuteQuery = function(sqlite3, query, callback)
		return sqlite3_execute_query(sqlite3, tostring(query), callback);
	end,

	ExecuteNonQuery = function(sqlite3, query)
		return sqlite3_execute_non_query(sqlite3, tostring(query));
	end,

	Flags =
	{
		None         = SQLITE3_DATABASE_FLAG_NONE,
		URI          = SQLITE3_DATABASE_FLAG_URI,
		Create       = SQLITE3_DATABASE_FLAG_CREATE,
		ReadOnly     = SQLITE3_DATABASE_FLAG_READ_ONLY,
		ReadWrite    = SQLITE3_DATABASE_FLAG_READ_WRITE,
		Memory       = SQLITE3_DATABASE_FLAG_MEMORY,

		NoMutex      = SQLITE3_DATABASE_FLAG_NO_MUTEX,
		FullMutex    = SQLITE3_DATABASE_FLAG_FULL_MUTEX,

		NoFollow     = SQLITE3_DATABASE_FLAG_NO_FOLLOW,

		SharedCache  = SQLITE3_DATABASE_FLAG_SHARED_CACHE,
		PrivateCache = SQLITE3_DATABASE_FLAG_PRIVATE_CACHE
	},

	QueryResultRow =
	{
		GetColumnCount = function(query_result_row)
			return sqlite3_query_result_row_get_column_count(query_result_row);
		end,

		GetColumnName = function(query_result_row, index)
			return sqlite3_query_result_row_get_column_name(query_result_row, tonumber(index));
		end,

		GetColumnValue = function(query_result_row, index)
			return sqlite3_query_result_row_get_column_value(query_result_row, tonumber(index));
		end
	}
};

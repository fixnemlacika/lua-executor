#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#include <AL/SQLite3/Database.hpp>

struct _sqlite3
{
	AL::SQLite3::Database database;
};

struct _sqlite3_query_result_row
{
	AL::SQLite3::DatabaseQueryResultRow* row;
};

typedef typename AL::Get_Enum_Or_Integer_Base<AL::SQLite3::DatabaseFlags>::Type SQLITE3_DATABASE_FLAG;

enum SQLITE3_DATABASE_FLAGS : SQLITE3_DATABASE_FLAG
{
	SQLITE3_DATABASE_FLAG_NONE          = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::None),

	SQLITE3_DATABASE_FLAG_URI           = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::URI),
	SQLITE3_DATABASE_FLAG_CREATE        = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::Create),
	SQLITE3_DATABASE_FLAG_READ_ONLY     = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::ReadOnly),
	SQLITE3_DATABASE_FLAG_READ_WRITE    = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::ReadWrite),
	SQLITE3_DATABASE_FLAG_MEMORY        = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::Memory),

	SQLITE3_DATABASE_FLAG_NO_MUTEX      = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::NoMutex),
	SQLITE3_DATABASE_FLAG_FULL_MUTEX    = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::FullMutex),

	SQLITE3_DATABASE_FLAG_NO_FOLLOW     = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::NoFollow),

	SQLITE3_DATABASE_FLAG_SHARED_CACHE  = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::SharedCache),
	SQLITE3_DATABASE_FLAG_PRIVATE_CACHE = static_cast<SQLITE3_DATABASE_FLAG>(AL::SQLite3::DatabaseFlags::PrivateCache)
};

typedef AL::Lua54::Function::LuaCallback<void(_sqlite3* sqlite3, _sqlite3_query_result_row* query_result_row, AL::uint32 query_result_row_index)> _sqlite3_query_callback;

bool        _sqlite3_is_open(_sqlite3* sqlite3);

void        _sqlite3_close(_sqlite3* sqlite3);

_sqlite3*   _sqlite3_init(const char* database_path, SQLITE3_DATABASE_FLAGS flags)
{
	auto _sqlite3 = new ::_sqlite3
	{
		.database = AL::SQLite3::Database(AL::FileSystem::Path(database_path), static_cast<AL::SQLite3::DatabaseFlags>(flags))
	};

	return _sqlite3;
}
void        _sqlite3_deinit(_sqlite3* sqlite3)
{
	if (sqlite3 != nullptr)
	{
		if (_sqlite3_is_open(sqlite3))
			_sqlite3_close(sqlite3);

		delete sqlite3;
	}
}

bool        _sqlite3_is_open(_sqlite3* sqlite3)
{
	return sqlite3 && sqlite3->database.IsOpen();
}

bool        _sqlite3_open(_sqlite3* sqlite3)
{
	if (_sqlite3_is_open(sqlite3))
		return false;

	try
	{
		sqlite3->database.Open();
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}
void        _sqlite3_close(_sqlite3* sqlite3)
{
	if (_sqlite3_is_open(sqlite3))
	{
		sqlite3->database.Close();
	}
}

bool        _sqlite3_execute_query(_sqlite3* sqlite3, const char* query, _sqlite3_query_callback callback)
{
	if (!_sqlite3_is_open(sqlite3))
		return false;

	AL::SQLite3::DatabaseQueryResult result;

	try
	{
		result = sqlite3->database.Query(query);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	_sqlite3_query_result_row query_result_row;
	AL::uint32                query_result_row_index = 0;

	for (auto& result_row : result)
	{
		query_result_row.row = &result_row;

		callback(sqlite3, &query_result_row, query_result_row_index++);
	}

	return true;
}
bool        _sqlite3_execute_non_query(_sqlite3* sqlite3, const char* query)
{
	if (!_sqlite3_is_open(sqlite3))
		return false;

	try
	{
		sqlite3->database.Query(query);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}

AL::uint32  _sqlite3_query_result_row_get_column_count(_sqlite3_query_result_row* query_result_row)
{
	return static_cast<AL::uint32>(query_result_row ? query_result_row->row->Columns.GetSize() : 0);
}
const char* _sqlite3_query_result_row_get_column_name(_sqlite3_query_result_row* query_result_row, AL::uint32 index)
{
	return (query_result_row && (index > 0) && (index <= query_result_row->row->Columns.GetSize())) ? query_result_row->row->Columns[index - 1].GetCString() : nullptr;
}
const char* _sqlite3_query_result_row_get_column_value(_sqlite3_query_result_row* query_result_row, AL::uint32 index)
{
	return (query_result_row && (index > 0) && (index <= query_result_row->row->Values.GetSize())) ? query_result_row->row->Values[index - 1].GetCString() : nullptr;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_NONE);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_URI);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_CREATE);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_READ_ONLY);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_READ_WRITE);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_MEMORY);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_NO_MUTEX);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_FULL_MUTEX);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_NO_FOLLOW);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_SHARED_CACHE);
	LUA_APRS_IS_RegisterGlobal(SQLITE3_DATABASE_FLAG_PRIVATE_CACHE);

	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_init,                              "sqlite3_init");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_deinit,                            "sqlite3_deinit");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_is_open,                           "sqlite3_is_open");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_open,                              "sqlite3_open");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_close,                             "sqlite3_close");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_execute_query,                     "sqlite3_execute_query");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_execute_non_query,                 "sqlite3_execute_non_query");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_query_result_row_get_column_count, "sqlite3_query_result_row_get_column_count");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_query_result_row_get_column_name,  "sqlite3_query_result_row_get_column_name");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_sqlite3_query_result_row_get_column_value, "sqlite3_query_result_row_get_column_value");
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_NONE);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_URI);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_CREATE);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_READ_ONLY);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_READ_WRITE);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_MEMORY);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_NO_MUTEX);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_FULL_MUTEX);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_NO_FOLLOW);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_SHARED_CACHE);
	LUA_APRS_IS_UnregisterGlobal(SQLITE3_DATABASE_FLAG_PRIVATE_CACHE);

	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_init");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_deinit");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_is_open");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_open");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_close");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_execute_query");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_execute_non_query");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_query_result_row_get_column_count");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_query_result_row_get_column_name");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("sqlite3_query_result_row_get_column_value");
});

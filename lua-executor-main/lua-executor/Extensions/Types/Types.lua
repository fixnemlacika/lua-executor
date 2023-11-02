Types =
{
	IsNil = function(value)
		return type(value) == 'nil';
	end,

	IsNull = function(value)
		return is_null(value);
	end,

	IsTable = function(value)
		return type(value) == 'table';
	end,

	IsNumber = function(value)
		return type(value) == 'number';
	end,

	IsString = function(value)
		return type(value) == 'string';
	end,

	IsBoolean = function(value)
		return type(value) == 'boolean';
	end,

	IsFunction = function(value)
		return type(value) == 'function';
	end
};


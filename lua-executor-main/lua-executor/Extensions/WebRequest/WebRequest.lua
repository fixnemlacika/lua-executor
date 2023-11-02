WebRequest =
{
	HTTP_Methods =
	{
		Get  = WEB_REQUEST_HTTP_METHOD_GET,
		Post = WEB_REQUEST_HTTP_METHOD_POST
	},

	HTTP_StatusCodes =
	{
		OK = WEB_REQUEST_HTTP_STATUS_CODE_OK,
	},

	Init = function(method)
		return web_request_init(method);
	end,

	Deinit = function(web_request)
		web_request_deinit(web_request);
	end,

	GetHeader = function(web_request, key)
		return web_request_get_header(web_request, tostring(key));
	end,

	SetHeader = function(web_request, key, value)
		return web_request_set_header(web_request, tostring(key), tostring(value));
	end,

	Execute = function(web_request, uri)
		return web_request_execute(web_request, tostring(uri));
	end,

	-- @return status_code, content
	DownloadString = function(uri)
		local web_request = WebRequest.Init(WebRequest.HTTP_Methods.Get);

		if not web_request then
			return 0, nil;
		end

		local web_response             = WebRequest.Execute(web_request, uri);
		local web_response_status_code = WebRequest.Response.GetStatusCode(web_response);
		local web_response_content     = WebRequest.Response.GetContent(web_response);

		WebRequest.Deinit(web_request);

		return web_response_status_code, web_response_content;
	end,

	Response =
	{
		GetContent = function(web_response)
			return web_response_get_content(web_response);
		end,

		GetStatusCode = function(web_response)
			return web_response_get_status_code(web_response);
		end
	}
};

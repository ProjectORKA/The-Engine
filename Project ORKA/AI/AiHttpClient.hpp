#pragma once

#include "../OrkaTypes.hpp"

struct AiHttpResponse
{
	Int statusCode = 0;
	String body;
	String error;
};

[[nodiscard]] AiHttpResponse aiHttpRequest(
	const String& host,
	U16 port,
	const String& method,
	const String& path,
	const String& body,
	const Vector<std::pair<String, String>>& headers = {});

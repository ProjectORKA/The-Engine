#include "AiToolRegistry.hpp"

#include "../Agent/AgentValue.hpp"
#include "../File.hpp"
#include "AiToolSandbox.hpp"

#include <filesystem>

namespace
{
	AgentValue::Object stringProperty(const String& description)
	{
		return {
			{"type", AgentValue("string")},
			{"description", AgentValue(description)}
			};
	}

	AgentValue toolFunction(const String& name, const String& description, AgentValue::Object properties, AgentValue::Array required)
	{
		return AgentValue::Object{
			{"type", AgentValue("function")},
			{"function", AgentValue(AgentValue::Object{
				{"name", AgentValue(name)},
				{"description", AgentValue(description)},
				{"parameters", AgentValue(AgentValue::Object{
					{"type", AgentValue("object")},
					{"properties", AgentValue(std::move(properties))},
					{"required", AgentValue(std::move(required))}
					})}
				})}
			};
	}

	String readStringArg(const AgentValue::Object& args, const String& key, String& error)
	{
		const auto iterator = args.find(key);
		const AgentValue* value = iterator == args.end() ? nullptr : &iterator->second;
		if (!value || !value->isString())
		{
			error = "Missing or invalid argument: " + key;
			return {};
		}
		return value->asString();
	}

	Int readIntArg(const AgentValue::Object& args, const String& key, const Int fallback)
	{
		const auto iterator = args.find(key);
		if (iterator == args.end()) return fallback;
		const AgentValue* value = &iterator->second;
		if (value->isInteger()) return static_cast<Int>(value->asInteger(fallback));
		if (value->isNumber()) return static_cast<Int>(value->asNumber(static_cast<Double>(fallback)));
		return fallback;
	}

	AiToolExecutionResult readFileTool(const AgentValue::Object& args)
	{
		AiToolExecutionResult result;
		String error;
		const String pathArg = readStringArg(args, "path", error);
		if (!error.empty())
		{
			result.error = error;
			return result;
		}

		Path resolved;
		if (!aiResolveToolPath(pathArg, resolved, result.error)) return result;
		if (!std::filesystem::exists(resolved) || !std::filesystem::is_regular_file(resolved))
		{
			result.error = "File not found.";
			return result;
		}
		if (!aiPathIsTextFile(resolved))
		{
			result.error = "Only text source files can be read via this tool.";
			return result;
		}
		if (std::filesystem::file_size(resolved) > 256 * 1024)
		{
			result.error = "File exceeds 256 KB read limit.";
			return result;
		}

		InFile file(resolved);
		if (!file.isOpen)
		{
			result.error = "Could not open file.";
			return result;
		}

		String content;
		String line;
		while (file.readLine(line)) content += line + "\n";

		result.succeeded = true;
		result.output = content;
		return result;
	}

	AiToolExecutionResult writeFileTool(const AgentValue::Object& args)
	{
		AiToolExecutionResult result;
		String error;
		const String pathArg = readStringArg(args, "path", error);
		const String content = readStringArg(args, "content", error);
		if (!error.empty())
		{
			result.error = error;
			return result;
		}
		if (content.size() > 128 * 1024)
		{
			result.error = "Content exceeds 128 KB write limit.";
			return result;
		}

		Path resolved;
		if (!aiResolveToolPath(pathArg, resolved, result.error)) return result;
		if (!aiPathIsTextFile(resolved))
		{
			result.error = "Only text source files can be edited via this tool.";
			return result;
		}

		std::filesystem::create_directories(resolved.parent_path());
		OutFile file(resolved);
		if (!file.isOpen)
		{
			result.error = "Could not write file.";
			return result;
		}
		file.write(content.c_str(), content.size());

		result.succeeded = true;
		result.output = "Wrote " + toString(content.size()) + " bytes to " + resolved.generic_string();
		return result;
	}

	AiToolExecutionResult listDirectoryTool(const AgentValue::Object& args)
	{
		AiToolExecutionResult result;
		String error;
		String pathArg = readStringArg(args, "path", error);
		if (!error.empty())
		{
			result.error = error;
			return result;
		}
		if (pathArg.empty()) pathArg = ".";

		Path resolved;
		if (!aiResolveToolPath(pathArg, resolved, result.error)) return result;
		if (!std::filesystem::exists(resolved) || !std::filesystem::is_directory(resolved))
		{
			result.error = "Directory not found.";
			return result;
		}

		const Int limit = std::clamp(readIntArg(args, "limit", 100), 1, 500);
		String listing;
		Int count = 0;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(resolved))
		{
			listing += (entry.is_directory() ? "[dir]  " : "[file] ") + entry.path().filename().string() + "\n";
			if (++count >= limit) break;
		}

		result.succeeded = true;
		result.output = listing.empty() ? "(empty directory)" : listing;
		return result;
	}

	AiToolExecutionResult describeImageTool(const AgentValue::Object& args)
	{
		AiToolExecutionResult result;
		String error;
		const String pathArg = readStringArg(args, "path", error);
		if (!error.empty())
		{
			result.error = error;
			return result;
		}

		Path resolved;
		if (!aiResolveToolPath(pathArg, resolved, result.error)) return result;
		if (!std::filesystem::exists(resolved) || !std::filesystem::is_regular_file(resolved))
		{
			result.error = "Image file not found.";
			return result;
		}

		const String ext = resolved.extension().string();
		if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".webp" && ext != ".bmp")
		{
			result.error = "Unsupported image type.";
			return result;
		}

		result.succeeded = true;
		result.output =
			"Image metadata only (vision not sent to model yet): "
			+ resolved.filename().string()
			+ ", " + toString(static_cast<ULL>(std::filesystem::file_size(resolved))) + " bytes.";
		return result;
	}
}

Vector<AiToolDescriptor> aiRegisteredTools()
{
	return {
		{"read_file", "Read a text file under the Project ORKA workspace.", true},
		{"write_file", "Write text content to an allowed source file.", true},
		{"list_directory", "List files in a workspace directory.", true},
		{"describe_image", "Return basic metadata for PNG/JPEG images (vision hook).", true},
		{"speech_input", "Transcribe microphone audio (not implemented).", false},
		{"speech_output", "Synthesize speech from text (not implemented).", false}
	};
}

AgentValue aiOpenAiToolDefinitions()
{
	AgentValue::Array tools;
	tools.push_back(toolFunction(
		"read_file",
		"Read a UTF-8 text file relative to Project ORKA.",
		{{"path", AgentValue(stringProperty("Relative or absolute path inside Project ORKA."))}},
		{AgentValue("path")}));
	tools.push_back(toolFunction(
		"write_file",
		"Overwrite a text file with new content.",
		{
			{"path", AgentValue(stringProperty("Target file path."))},
			{"content", AgentValue(stringProperty("Full new file contents."))}
			},
		{AgentValue("path"), AgentValue("content")}));
	tools.push_back(toolFunction(
		"list_directory",
		"List entries in a directory.",
		{
			{"path", AgentValue(stringProperty("Directory path (default .)."))},
			{"limit", AgentValue(AgentValue::Object{{"type", AgentValue("integer")}, {"description", AgentValue("Max entries (1-500).")}})}
			},
		{}));
	tools.push_back(toolFunction(
		"describe_image",
		"Inspect image file metadata before vision is wired to the model.",
		{{"path", AgentValue(stringProperty("Image path (.png, .jpg, .webp)."))}},
		{AgentValue("path")}));
	return AgentValue(std::move(tools));
}

AiToolExecutionResult executeAiTool(const String& name, const String& argumentsJson)
{
	AiToolExecutionResult result;
	const AgentJsonParseResult parsed = parseAgentJson(argumentsJson);
	if (!parsed.succeeded() || !parsed.value.isObject())
	{
		result.error = "Tool arguments must be a JSON object.";
		return result;
	}

	const AgentValue::Object& args = parsed.value.asObject();
	if (name == "read_file") return readFileTool(args);
	if (name == "write_file") return writeFileTool(args);
	if (name == "list_directory") return listDirectoryTool(args);
	if (name == "describe_image") return describeImageTool(args);

	result.error = "Unknown or unavailable tool: " + name;
	return result;
}

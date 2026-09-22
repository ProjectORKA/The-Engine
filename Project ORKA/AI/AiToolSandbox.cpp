#include "AiToolSandbox.hpp"

#include "../ResourceManager.hpp"

#include <algorithm>

Path aiToolWorkspaceRoot()
{
	return std::filesystem::absolute(resourceManager.getOrkaDataPath().parent_path()).lexically_normal();
}

Bool aiResolveToolPath(const String& requestedPath, Path& resolved, String& error)
{
	if (requestedPath.empty())
	{
		error = "Path is empty.";
		return false;
	}

	const Path root = aiToolWorkspaceRoot();
	Path candidate = Path(requestedPath);
	if (candidate.is_relative()) candidate = root / candidate;
	candidate = std::filesystem::absolute(candidate).lexically_normal();

	String candidateText = candidate.generic_string();
	std::replace(candidateText.begin(), candidateText.end(), '\\', '/');
	String rootText = root.generic_string();
	std::replace(rootText.begin(), rootText.end(), '\\', '/');

	if (candidateText.rfind(rootText, 0) != 0)
	{
		error = "Path is outside the allowed Project ORKA workspace.";
		return false;
	}

	resolved = candidate;
	return true;
}

Bool aiPathIsTextFile(const Path& path)
{
	const String ext = path.extension().string();
	return ext == ".txt" || ext == ".md" || ext == ".html" || ext == ".scss" || ext == ".hpp"
		|| ext == ".cpp" || ext == ".json" || ext == ".glsl" || ext == ".vert" || ext == ".frag"
		|| ext == ".comp" || ext == ".ui" || ext == ".csv" || ext == ".ini" || ext == ".yaml" || ext == ".yml";
}

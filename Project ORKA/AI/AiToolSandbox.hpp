#pragma once

#include "../OrkaTypes.hpp"

[[nodiscard]] Path aiToolWorkspaceRoot();
[[nodiscard]] Bool aiResolveToolPath(const String& requestedPath, Path& resolved, String& error);
[[nodiscard]] Bool aiPathIsTextFile(const Path& path);

#pragma once

#include "../OrkaTypes.hpp"

[[nodiscard]] Bool extractZipArchive(const Path& zipPath, const Path& destinationDirectory);
[[nodiscard]] Path findFirstFileNamed(const Path& root, const String& fileName);

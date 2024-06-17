#pragma once


#include "Basics.hpp"

#define SQLITE_DEBUG
#include "sqlite_modern_cpp.h"


extern Vector<String> imageTags;

struct ImageViewerDatabase
{
	ImageViewerDatabase();
	void addTag(const String& tag);
	void add(const String& hash, const Path& filePath);
	void setValue(const String& hash, const String& tag, Float value);

	[[nodiscard]] Int   entryCount();
	[[nodiscard]] Bool  doesColumnExist(const String& columnName);
	[[nodiscard]] Float getTagValue(const String& hash, const String& tag);

private:
	sqlite::database dataBase;
};
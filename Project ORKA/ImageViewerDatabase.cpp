#include "ImageViewerDatabase.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "Hash.hpp"

String imageTableName = "Images";

Vector<String> imageTags = {
	//generic tags (auto applicable)
	"aspect", "size", "pixel_count", "hue", "brightness", "saturation", "grayscale",
	//
	"ai", "photo", "painting",
	//content tags
	"person", "animal", "man", "woman",
};

Int ImageViewerDatabase::entryCount()
{
	Int count = 0;
	try
	{
		dataBase << "SELECT COUNT(*) FROM " + imageTableName >> count;
	}
	catch (const std::exception& e)
	{
		logError(e.what());
	}
	return count;
}

ImageViewerDatabase::ImageViewerDatabase() : dataBase("ORKAImageViewer.db")
{
	dataBase << "CREATE TABLE IF NOT EXISTS " + imageTableName + " (Hash TEXT PRIMARY KEY UNIQUE NOT NULL, Name TEXT, Path TEXT);";
}

void ImageViewerDatabase::addTag(const String& tag)
{
	if (!doesColumnExist(tag))
	{
		try
		{
			dataBase << "ALTER TABLE " + imageTableName + " ADD COLUMN " + tag + " REAL DEFAULT NULL;";
		}
		catch (const std::exception& e)
		{
			logError(e.what());
		}
	}
}

Bool ImageViewerDatabase::doesColumnExist(const String& columnName)
{
	Vector<String> schema;

	dataBase << "PRAGMA table_info(" + imageTableName + ");" >> [&](Int cid, const String& name, String type, Int notnull, String dflt_value, Int pk)
	{
		schema.push_back(name);
	};

	for (const auto& column : schema) if (column == columnName) return true;
	return false;
}

void ImageViewerDatabase::add(const String& hash, const Path& filePath)
{
	try
	{
		const String name = getFileName(filePath);
		const String path = toString(filePath);

		dataBase << "INSERT OR REPLACE INTO " + imageTableName + " (Hash, Name, Path) values (?,?,?);" << hash << name << path;

		logDebug("The new record got assigned id " + toString(dataBase.last_insert_rowid()));
	}
	catch (const std::exception& e)
	{
		logError(e.what());
	}
}

Float ImageViewerDatabase::getTagValue(const String& hash, const String& tag)
{
	Float value = 0.0f;
	try
	{
		dataBase << "SELECT " + tag + " FROM " + imageTableName + " WHERE Hash = ? ;" << hash >> [& ](const Float& v)
		{
			value = v;
		};
	}
	catch (const std::exception& e)
	{
		logError(e.what());
	}
	return value;
}

void ImageViewerDatabase::setValue(const String& hash, const String& tag, const Float value)
{
	if (!doesColumnExist(tag))
	{
		logError("Tag (" + tag + ") does not exist and will be created.");
		addTag(tag);
	}

	try
	{
		dataBase << "UPDATE " + imageTableName + " SET " + tag + " = ? WHERE hash = ?;" << value << hash;
		logDebug("Value for tag " + tag + " updated successfully.");
	}
	catch (const std::exception& e)
	{
		logError(e.what());
	}
}

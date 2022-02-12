
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Image.hpp"
#include "File.hpp"

String loadString(Path path);
Bool doesPathExist(Path path);
void copyFile(Path source, Path destination);
Image loadImage(Path path, Int bitcount, Bool inverted);
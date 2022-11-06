
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include "Platform.hpp"

#if defined(OS_WINDOWS)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	#include <WindowsX.h>
//	#include <GL/GL.h>
#elif defined(OS_LINUX)
	#include <X11/Xlib.h>
//	#include <GL/gl.h>
//	#include <GL/glx.h>
	#include <sys/time.h>
#endif

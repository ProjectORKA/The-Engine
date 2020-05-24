#pragma once

#ifdef GAMEMODULE_EXPORTS
#define MODULE __declspec(dllexport)
#else
#define MODULE __declspec(dllimport)
#endif


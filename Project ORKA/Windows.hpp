#pragma once

#pragma comment(lib,"user32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>  // For IFileDialog
#undef near
#undef far

#include <filesystem>

inline std::wstring stringFromWide(WCHAR* wideStr)
{
	return wideStr ? std::wstring(wideStr) : std::wstring();
}

inline std::filesystem::path pickFolder()
{
	std::filesystem::path selectedFolder;

	// Initialize COM
	if (HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); SUCCEEDED(hr))
	{
		IFileDialog* pFileDialog = nullptr;

		// Create the FileOpenDialog object
		hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
		if (SUCCEEDED(hr) && pFileDialog != nullptr)
		{
			// Set the dialog to pick folders
			DWORD dwOptions = 0;
			if (SUCCEEDED(pFileDialog->GetOptions(&dwOptions))) pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

			// Show the dialog
			hr = pFileDialog->Show(nullptr);
			if (SUCCEEDED(hr))
			{
				IShellItem* pShellItem = nullptr;
				hr                     = pFileDialog->GetResult(&pShellItem);
				if (SUCCEEDED(hr) && pShellItem != nullptr)
				{
					// Retrieve the selected folder path
					PWSTR pszFilePath = nullptr;
					hr                = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr) && pszFilePath != nullptr)
					{
						// Assign directly to std::filesystem::path
						selectedFolder = pszFilePath;
						// Free the allocated memory
						CoTaskMemFree(pszFilePath);
					}
					pShellItem->Release();
				}
			}
			pFileDialog->Release();
		}
		CoUninitialize();
	}

	return selectedFolder;
}

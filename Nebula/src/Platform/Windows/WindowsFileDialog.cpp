#include "nbpch.h"
#include "WindowsFileDialog.h"
#include <Windows.h>
#include <commdlg.h>

namespace Nebula {

	std::optional<std::string> WindowsFileDialog::OpenFile(const std::string& filter, const std::string& initialDir)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return std::string(ofn.lpstrFile);
		}

		return std::nullopt;
	}

	std::optional<std::string> WindowsFileDialog::SaveFile(const std::string& filter, const std::string& defaultExtension, const std::string& initialDir)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
		ofn.lpstrDefExt = defaultExtension.empty() ? NULL : defaultExtension.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return std::string(ofn.lpstrFile);
		}

		return std::nullopt;
	}

	FileDialog* FileDialog::Create()
	{
		return new WindowsFileDialog();
	}

}

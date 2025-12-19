#pragma once

#include "Nebula/Core/FileDialog.h"

namespace Nebula {

	class WindowsFileDialog : public FileDialog
	{
	public:
		WindowsFileDialog() = default;
		virtual ~WindowsFileDialog() = default;

		virtual std::optional<std::string> OpenFile(const std::string& filter, const std::string& initialDir = "") override;
		virtual std::optional<std::string> SaveFile(const std::string& filter, const std::string& defaultExtension = "", const std::string& initialDir = "") override;
	};

}

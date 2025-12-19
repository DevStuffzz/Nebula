#pragma once

#include "Nebula/Core/FileDialog.h"

namespace Nebula {

	class MacOSFileDialog : public FileDialog
	{
	public:
		MacOSFileDialog() = default;
		virtual ~MacOSFileDialog() = default;

		virtual std::optional<std::string> OpenFile(const std::string& filter, const std::string& initialDir = "") override;
		virtual std::optional<std::string> SaveFile(const std::string& filter, const std::string& defaultExtension = "", const std::string& initialDir = "") override;
	};

}

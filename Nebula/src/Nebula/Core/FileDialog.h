#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"

#include <string>
#include <optional>

namespace Nebula {

	struct FileDialogFilter
	{
		std::string Name;
		std::string Pattern;
	};

	class NEBULA_API FileDialog
	{
	public:
		virtual ~FileDialog() = default;

		// Opens a file open dialog and returns the selected file path
		// Returns empty optional if user cancels
		virtual std::optional<std::string> OpenFile(const std::string& filter, const std::string& initialDir = "") = 0;

		// Opens a file save dialog and returns the selected file path
		// Returns empty optional if user cancels
		virtual std::optional<std::string> SaveFile(const std::string& filter, const std::string& defaultExtension = "", const std::string& initialDir = "") = 0;

		// Factory method to create platform-specific file dialog
		static FileDialog* Create();
	};

}

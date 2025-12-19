#include "nbpch.h"
#include "MacOSFileDialog.h"

#import <Cocoa/Cocoa.h>

namespace Nebula {

	std::optional<std::string> MacOSFileDialog::OpenFile(const std::string& filter, const std::string& initialDir)
	{
		NSOpenPanel* openPanel = [NSOpenPanel openPanel];
		[openPanel setCanChooseFiles:YES];
		[openPanel setCanChooseDirectories:NO];
		[openPanel setAllowsMultipleSelection:NO];
		
		if (!initialDir.empty())
		{
			NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:initialDir.c_str()]];
			[openPanel setDirectoryURL:url];
		}
		
		if ([openPanel runModal] == NSModalResponseOK)
		{
			NSURL* url = [[openPanel URLs] objectAtIndex:0];
			NSString* path = [url path];
			return std::string([path UTF8String]);
		}
		
		return std::nullopt;
	}

	std::optional<std::string> MacOSFileDialog::SaveFile(const std::string& filter, const std::string& defaultExtension, const std::string& initialDir)
	{
		NSSavePanel* savePanel = [NSSavePanel savePanel];
		
		if (!defaultExtension.empty())
		{
			[savePanel setAllowedFileTypes:@[[NSString stringWithUTF8String:defaultExtension.c_str()]]];
		}
		
		if (!initialDir.empty())
		{
			NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:initialDir.c_str()]];
			[savePanel setDirectoryURL:url];
		}
		
		if ([savePanel runModal] == NSModalResponseOK)
		{
			NSURL* url = [savePanel URL];
			NSString* path = [url path];
			return std::string([path UTF8String]);
		}
		
		return std::nullopt;
	}

	FileDialog* FileDialog::Create()
	{
		return new MacOSFileDialog();
	}

}

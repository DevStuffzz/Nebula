# Quick Start Guide

Get Nebula Engine running on your machine in just a few minutes!

## 🚀 Prerequisites

### Windows
- **Visual Studio 2019/2022** with C++ Desktop Development workload
- **Git** for cloning the repository
- **Premake5** (automatically downloaded by build script)

### macOS  
- **Xcode Command Line Tools**: `xcode-select --install`
- **Git**: Usually pre-installed or via Homebrew
- **Premake5**: `brew install premake5`

## 📥 Clone and Build

### 1. Clone the Repository
```bash
git clone https://github.com/DevStuffzz/Nebula.git
cd Nebula
```

### 2. Generate Project Files

**Windows:**
```bash
Win-GenerateProjects.bat
```
This will automatically download Premake5 if needed and generate Visual Studio solution files.

**macOS:**
```bash
./Mac-GenerateProjects.sh
```
This generates Makefiles for building with GCC/Clang.

### 3. Build the Engine

**Windows:**
1. Open `Nebula.sln` in Visual Studio
2. Set `Sandbox` as the startup project  
3. Build and run (F5)

**macOS:**
```bash
make                    # Debug build
make config=release     # Release build
```

### 4. Run the Sandbox
**Windows:** Press F5 in Visual Studio

**macOS:**
```bash
./bin/Debug-macosx-x86_64/Sandbox/Sandbox
```

## ✅ Verify Installation

You should see:
- A window opens with the title "Nebula Engine"
- Purple/magenta clear color in the window
- Console output showing mouse movement events
- No error messages in the console

## 🎯 What You'll Get

After following this guide, you'll have:
- A working Nebula Engine build
- A test window that responds to mouse input
- Console logging showing events
- A foundation ready for further development

## 🔧 Current Engine Features

- ✅ **Cross-platform windowing** (GLFW)
- ✅ **Event system** (mouse, keyboard, window events)
- ✅ **Logging system** (spdlog integration)
- ✅ **Application framework** (main loop, event handling)

## 🚧 Next Steps

Once you have Nebula running, you can:
- Explore the source code to understand the current architecture
- Follow along as new features are implemented
- Contribute to development (see [CONTRIBUTING.md](../../CONTRIBUTING.md))

The engine is in active development - layer system and rendering are coming next!

## 🐛 Troubleshooting

### Common Issues

**Build fails on Windows:**
- Ensure Visual Studio has C++ Desktop Development workload installed
- Try regenerating project files: `Win-GenerateProjects.bat`

**Build fails on macOS:**
- Install Xcode Command Line Tools: `xcode-select --install` 
- Install Premake5: `brew install premake5`
- Try regenerating: `./Mac-GenerateProjects.sh`

**Window doesn't appear:**
- Check console for error messages
- Ensure graphics drivers are up to date
- Try running from command line to see full output

Need help? [Open an issue](https://github.com/DevStuffzz/Nebula/issues) on GitHub!
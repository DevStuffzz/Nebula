# Nebula Game Engine

A cross-platform C++ game engine built from scratch, focusing on modern architecture and performance.

## 🚀 Current Features

### Platform Support
- ✅ **Windows** - Full support with Visual Studio project generation
- ✅ **macOS** - Native support with Makefile generation
- 🎯 **Cross-platform windowing** via GLFW

### Core Systems
- ✅ **Window Management** - Cross-platform window creation and event handling
- ✅ **Event System** - Flexible event-driven architecture
- ✅ **Logging System** - Multi-level logging with spdlog integration
- ✅ **Application Framework** - Base application class with main loop
- ✅ **Input Handling** - Mouse and keyboard event processing

### Development Tools
- ✅ **Premake5 Build System** - Automated project generation
- ✅ **Cross-platform Compilation** - Works on both Windows and macOS
- ✅ **Debug Support** - Assertions and debug break functionality

## 🎯 Planned Features

### Rendering (Next Priority)
- 🔄 **Layer Stack System** - Efficient rendering layer management
- 🔄 **Abstract Renderer Interface** - Platform-agnostic rendering API
- 📋 **DirectX 11/12** - Windows native rendering backend
- 📋 **Metal** - macOS native rendering backend  
- 📋 **Vulkan** - Cross-platform high-performance rendering
- 📋 **OpenGL** - Fallback cross-platform rendering

### Engine Architecture
- 📋 **Entity Component System (ECS)** - Modern game object architecture
- 📋 **Scene Management** - Hierarchical scene graph system
- 📋 **Asset Pipeline** - Efficient asset loading and management
- 📋 **Memory Management** - Custom allocators and memory pools

### Advanced Features
- 📋 **Physics Integration** - 2D/3D physics simulation
- 📋 **Audio System** - 3D spatial audio support
- 📋 **Scripting** - Lua/C# scripting integration
- 📋 **Editor Tools** - ImGui-based development tools

### Platforms
- 📋 **Linux** - Ubuntu/Debian support
- 📋 **Mobile** - iOS and Android deployment

## 🛠️ Building the Engine

### Windows
```bash
# Generate Visual Studio projects
Win-GenerateProjects.bat

# Open Nebula.sln in Visual Studio and build
```

### macOS
```bash
# Generate Makefiles
./Mac-GenerateProjects.sh

# Build with make
make                    # Debug build
make config=release     # Release build

# Run the sandbox
./bin/Debug-macosx-x86_64/Sandbox/Sandbox
```

## 📋 Requirements

- **C++17** compatible compiler
- **Premake5** for project generation
- **Git** for source control

### Platform-Specific Requirements

**Windows:**
- Visual Studio 2019/2022
- Windows SDK

**macOS:**
- Xcode Command Line Tools
- macOS 10.15+ (Catalina or later)

## 🏗️ Architecture Overview

```
Nebula Engine
├── Core Systems
│   ├── Application Framework
│   ├── Window Management (GLFW)
│   ├── Event System
│   └── Logging (spdlog)
├── Platform Layer
│   ├── Windows (DirectX/Vulkan)
│   └── macOS (Metal/OpenGL)
├── Rendering (Planned)
│   ├── Layer Stack
│   ├── Renderer API
│   └── Platform Backends
└── Game Framework (Planned)
    ├── ECS
    ├── Scene Management
    └── Asset Pipeline
```

## 🤝 Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed contribution guidelines.

## 📄 License

[License information to be added]

---

**Status:** Early Development - Core systems functional, rendering layer in progress

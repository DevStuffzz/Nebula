# Nebula Game Engine

<p align="center">
  <img src="branding/logo.png" alt="Nebula Game Engine" width="400"/>
</p>

<p align="center">
  <strong>A cross-platform C++ game engine built from scratch, focusing on modern architecture and performance.</strong>
</p>

## 🚀 Current Features

### Rendering System
- ✅ **OpenGL Renderer** - Hardware-accelerated rendering
- ✅ **Shader System** - Abstract shader interface with OpenGL implementation
- ✅ **File-based Shaders** - Load shaders from .glsl files with #type directives
- ✅ **Vertex Arrays & Buffers** - Efficient geometry management
- ✅ **Render Commands** - Platform-agnostic rendering API

### Platform Support
- ✅ **Windows** - Full support with Visual Studio project generation
- ✅ **macOS** - Native support with Makefile generation
- 🎯 **Cross-platform windowing** via GLFW

### Core Systems
- ✅ **Window Management** - Cross-platform window creation and event handling
- ✅ **Event System** - Flexible event-driven architecture
- ✅ **Logging System** - Multi-level logging with spdlog integration
- ✅ **Application Framework** - Base application class with main loop
- ✅ **Layer System** - Efficient layer stack for rendering and updates
- ✅ **Input Handling** - Mouse and keyboard event processing
- ✅ **ImGui Integration** - Debug UI and editor tools

### Development Tools
- ✅ **Premake5 Build System** - Automated project generation
- ✅ **Cross-platform Compilation** - Works on both Windows and macOS
- ✅ **Debug Support** - Assertions and debug break functionality
- ✅ **Asset Pipeline** - Automatic asset copying to build output

## 🎯 Planned Features

### Rendering Enhancements
- 📋 **Camera System** - 2D/3D camera controls
- 📋 **Texture Support** - Image loading and texture binding
- 📋 **Batch Rendering** - Optimized draw call batching
- 📋 **DirectX 11/12** - Windows native rendering backend
- 📋 **Metal** - macOS native rendering backend  
- 📋 **Vulkan** - Cross-platform high-performance rendering

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

**Status:** Active Development - Core systems and OpenGL rendering functional

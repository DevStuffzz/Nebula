# Contributing to Nebula Game Engine

Thank you for your interest in contributing to Nebula! This document outlines how to contribute to the project effectively.

## 🚀 Current Development Status

Nebula is in **early development** with the following priorities:

1. **Core Architecture** - Building fundamental engine systems
2. **Layer Stack System** - Rendering layer management (in progress)
3. **Renderer Interface** - Abstract rendering API design
4. **Platform-Specific Rendering** - DirectX (Windows) and Metal (macOS) backends

## 🛠️ Development Setup

### Prerequisites

- **C++17** compatible compiler
- **Git** for version control
- **Premake5** for project generation
  - Windows: Download from [premake.github.io](https://premake.github.io/)
  - macOS: `brew install premake5`

### Getting Started

1. **Fork and Clone**
   ```bash
   git clone https://github.com/YOUR_USERNAME/Nebula.git
   cd Nebula
   ```

2. **Generate Project Files**
   ```bash
   # Windows
   Win-GenerateProjects.bat
   
   # macOS
   ./Mac-GenerateProjects.sh
   ```

3. **Build and Test**
   ```bash
   # Windows: Open Nebula.sln in Visual Studio
   
   # macOS
   make
   ./bin/Debug-macosx-x86_64/Sandbox/Sandbox
   ```

## 📋 How to Contribute

### 1. Choose Your Contribution Type

**🏗️ Core Systems (High Priority)**
- Layer stack implementation
- Renderer interface design
- Cross-platform compatibility fixes

**🎨 Platform-Specific Features**
- DirectX renderer (Windows)
- Metal renderer (macOS)
- Vulkan support (cross-platform)

**🔧 Tools & Infrastructure**
- Build system improvements
- Documentation
- Testing framework

**🐛 Bug Fixes**
- Cross-platform compatibility issues
- Performance optimizations
- Memory leak fixes

### 2. Development Workflow

1. **Create a Feature Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Follow Coding Standards**
   - Use C++17 features where appropriate
   - Follow existing naming conventions
   - Add comments for complex logic
   - Keep platform-specific code in Platform/ directories

3. **Test Your Changes**
   - Build on your target platform
   - Test the Sandbox application
   - Ensure no regressions

4. **Submit a Pull Request**
   - Clear description of changes
   - Reference any related issues
   - Include testing steps

## 🏗️ Code Organization

```
Nebula/
├── src/
│   ├── Nebula/           # Core engine code
│   │   ├── Application.* # Main application framework
│   │   ├── Core.h        # Platform definitions
│   │   ├── Events/       # Event system
│   │   └── ...
│   └── Platform/         # Platform-specific implementations
│       ├── GLFW/         # Cross-platform window management
│       └── Windows/      # Windows-specific code (legacy)
├── vendor/               # Third-party libraries
├── Sandbox/              # Test application
└── premake5.lua          # Build configuration
```

## 📝 Coding Guidelines

### Naming Conventions
- **Classes:** PascalCase (`WindowsWindow`, `Application`)
- **Functions/Methods:** PascalCase (`OnUpdate`, `GetWidth`)
- **Variables:** 
  - Local variables: camelCase (`windowWidth`, `isRunning`)
  - Private member variables: `m_` prefix + PascalCase (`m_Window`, `m_Data`)
  - Static member variables: `s_` prefix + PascalCase (`s_GLFWInitialized`)
  - Public member variables: PascalCase (`Width`, `Height`)
- **Constants:** UPPER_SNAKE_CASE (`NB_PLATFORM_WINDOWS`)
- **Macros:** UPPER_SNAKE_CASE (`NEBULA_API`, `EVENT_CLASS_TYPE`)

### Class Structure
Classes should be organized with visibility sections in the following order:
1. **public:** methods
2. **public:** members  
3. **private:** methods
4. **private:** members

Use separate visibility specifiers to clearly separate methods and members.

### Variable Examples
```cpp
class WindowsWindow : public Window {
public:
    // Public methods
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();
    void OnUpdate() override;
    inline unsigned int GetWidth() const override { return m_Data.Width; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;
    
public:
    // Public members
    struct WindowData {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
    
private:
    // Private methods
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();
    
private:
    // Private members
    GLFWwindow* m_Window;
    WindowData m_Data;
    
    static bool s_GLFWInitialized;
};
```

### Platform-Specific Code
```cpp
#ifdef NB_PLATFORM_WINDOWS
    // Windows-specific implementation
#elif defined(NB_PLATFORM_MACOS)
    // macOS-specific implementation
#else
    #error "Platform not supported!"
#endif
```

### Memory Management
- Use smart pointers where appropriate
- Avoid raw `new`/`delete` in favor of RAII
- Consider memory alignment for performance-critical code

### Error Handling
- Use the built-in assertion macros (`NEB_ASSERT`, `NEB_CORE_ASSERT`)
- Log errors using the integrated logging system
- Handle platform-specific error codes appropriately

## 🚦 Pull Request Guidelines

### Before Submitting
- [ ] Code builds successfully on your platform
- [ ] No compiler warnings introduced
- [ ] Sandbox application runs without crashes
- [ ] Code follows project style guidelines
- [ ] Changes are documented in commit messages

### PR Description Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tested on Windows
- [ ] Tested on macOS
- [ ] Sandbox runs successfully

## Additional Notes
Any additional context or considerations
```

## 🎯 Current Focus Areas

### High Priority (Actively Seeking Contributors)
1. **Layer Stack System** - Core rendering architecture
2. **Renderer Interface** - Abstract graphics API
3. **Input System Improvements** - Enhanced keyboard/mouse handling

### Medium Priority
1. **Asset Pipeline** - Basic asset loading system
2. **Math Library** - Vector/matrix operations
3. **Memory Allocators** - Custom memory management

### Future Considerations
1. **Entity Component System** - Modern game object architecture
2. **Physics Integration** - 2D/3D physics support
3. **Audio System** - Spatial audio implementation

## 🤝 Communication

- **Issues:** Use GitHub Issues for bug reports and feature requests
- **Discussions:** GitHub Discussions for general questions
- **Code Review:** All changes go through PR review process

## 📄 License

By contributing to Nebula, you agree that your contributions will be licensed under the same license as the project (to be determined).

---

**Questions?** Feel free to open an issue or start a discussion. We're here to help!
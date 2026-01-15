# Vibe Coding Rules

## C++ Core Rules

### Code Style
- **Namespaces**: 
    - Use `OCTK_BEGIN_NAMESPACE` and `OCTK_END_NAMESPACE` to wrap all code in files starting with octk_
    - If the included header file does not define the OCTK_BEGIN_NAMESPACE macro, include <octk_global.hpp>
    - Place implementation details in `namespace detail {} // namespace detail`
    - No indentation for namespace code
- **Naming Conventions**:
    - Classes: PascalCase (e.g., `EventLoop`, `AbstractEventDispatcher`)
    - Member variables: prefixed with m (e.g., `mDispatcher`, `mLoop`)
    - Functions: camelCase (e.g., `initialize()`, `runOnce()`)
    - Constants: UPPER_SNAKE_CASE
- **Header File Rules**:
    - Format: `#pragma once` at the beginning of the file
    - Macro definition: `#ifndef _OCTK_<FILE>_HPP` as an alternative (only for special requirements)
    - Inclusion: When including octk_ prefixed headers in source directory files, use absolute path `<...>`
    - Prefix: If including octk_ prefixed headers ending with `.ipp` or `.tpp`, add the `detail/` prefix, e.g., <detail/octk_meta_type.tpp>
    - Private: If including octk_ prefixed headers in the format `*_p.*` which indicates private implementation files, add the `private/` prefix, e.g., <private/octk_platform_thread_p.hpp>
- **Pointers/References**: Prefer smart pointers `std::shared_ptr<T>` and `std::unique_ptr<T>`
- **Thread Safety**: Use `std::mutex` and `std::lock_guard` to protect shared resources

### Architectural Patterns
- **Abstract Interfaces**: Use pure virtual classes to define extension points (e.g., `AbstractEventDispatcher`)
- **Factory Pattern**: Implement backend adaptation through `EventBackendFactory`
- **RAII**: Ensure automatic resource release, destructor calls `finalize()`

### Cross-Platform Support
- **Compiler Detection**: `#if defined(_MSVC_) || defined(__GNUC__) || defined(__clang__)`
- **Platform Macros**: `OCTK_OS_WINDOWS`, `OCTK_OS_LINUX`, `OCTK_OS_MACOS`
- **Export Symbols**: `OCTK_<MODULE>_API` for DLL/SO symbol visibility
- **Alignment**: `OCTK_ALIGNED(N)` macro handles alignment syntax for different compilers

### Error Handling
- **Error Codes**: Use `std::error_code` and `std::error_category`
- **Logging**: Uniformly use OCTK_DEBUG, OCTK_INFO, OCTK_WARN, OCTK_ERROR macros

### API Documentation
- **Tools**: Doxygen for C++ documentation, Sphinx for Python documentation
- **Format**: C++ uses `//` style comments, Python uses docstrings
- **Output**: Generated HTML/PDF stored in `docs/api/` directory

### Continuous Integration
- **Code Checking**: clang-format (C++), black (Python), eslint (JavaScript)
- **Static Analysis**: clang-analyzer, cppcheck, SonarQube
- **Compilation**: Support for Debug/Release/RelWithDebInfo/MinSizeRel
- **Platforms**: Linux (GCC/Clang), macOS (Clang), Windows (MSVC)

### Module Division and Dependencies
- **core**: Infrastructure (event loop, thread pool, memory management)
- **media**: Audio/Video processing (encoding, decoding, format conversion like ARGB→I420)
- **network**: Network transmission (protocol stack, TCP/UDP, WebSocket, WebRTC)
- **service**: High-level services (OSGI, microservices)
- **Dependency Direction**: service → network → core (strict one-way dependency)

### File Organization

### Agent Restrictions
- **Restriction**: Modification of *instructions.md files is not allowed
- **Prohibition**: Creation of *instructions.md files is not allowed
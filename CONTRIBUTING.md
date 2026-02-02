# Contributing to AI Debugger

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to the AI Debugger project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Submitting Changes](#submitting-changes)
- [Reporting Bugs](#reporting-bugs)
- [Feature Requests](#feature-requests)

## Code of Conduct

By participating in this project, you agree to maintain a respectful and inclusive environment for all contributors.

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.15 or later
- Git

### Setting Up Development Environment

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/LLVM-MLOpt_wp.git
   cd LLVM-MLOpt_wp
   ```

3. Build the project:
   ```bash
   # Linux/Mac
   ./build.sh

   # Windows
   build.bat
   ```

4. Run tests to ensure everything works:
   ```bash
   cd build
   ctest --output-on-failure
   ```

## Development Workflow

1. Create a new branch for your work:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. Make your changes following our coding standards
3. Add tests for new functionality
4. Ensure all tests pass
5. Commit your changes with clear messages
6. Push to your fork and submit a pull request

## Coding Standards

### C++ Guidelines

- **Standard**: Use C++17 features where appropriate
- **Formatting**: Follow existing code style (use clang-format if available)
- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `StackTraceParser`)
  - Functions/Methods: `camelCase` (e.g., `parseStackTrace`)
  - Variables: `snake_case` (e.g., `frame_count`)
  - Constants: `UPPER_CASE` (e.g., `MAX_FRAMES`)
  - Private members: prefix with `m_` (e.g., `m_impl`)

- **Best Practices**:
  - Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers
  - Prefer `const` correctness
  - Use RAII for resource management
  - Document complex algorithms and heuristics
  - Handle errors gracefully with informative messages

### Documentation

- Add Doxygen-style comments for public APIs:
  ```cpp
  /**
   * @brief Brief description of the function
   * @param param1 Description of parameter
   * @return Description of return value
   */
  ```
- Update relevant documentation files when adding features
- Include usage examples for new functionality

## Testing Guidelines

### Writing Tests

- Add unit tests for all new functionality in `tests/`
- Use Google Test framework
- Test edge cases and error conditions
- Aim for high code coverage

### Running Tests

```bash
# Build and run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/test_stack_trace_parser
```

### Test Structure

```cpp
TEST(ComponentName, TestScenario) {
    // Arrange
    auto parser = std::make_unique<StackTraceParser>();

    // Act
    auto result = parser->parse(input);

    // Assert
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->frameCount(), 5);
}
```

## Submitting Changes

### Pull Request Process

1. **Update Documentation**: Ensure README and relevant docs reflect your changes
2. **Add Tests**: Include tests that verify your changes work correctly
3. **Update Changelog**: Add an entry to CHANGELOG.md under "Unreleased"
4. **Clear Description**: Write a clear PR description explaining:
   - What problem does this solve?
   - How does it solve it?
   - Any breaking changes?
   - Related issues (use "Fixes #123" to auto-close issues)

### Commit Messages

Write clear, concise commit messages:

```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain the problem this commit solves and why this approach
was chosen.

- Bullet points are okay
- Use present tense ("add feature" not "added feature")
- Reference issues: Fixes #123
```

Examples:
- `add null pointer detection heuristic`
- `improve stack trace parsing for LLDB format`
- `fix memory leak in call graph analyzer`

## Reporting Bugs

### Before Submitting

- Check if the bug has already been reported in Issues
- Verify you're using the latest version
- Collect relevant information (stack traces, error messages, platform details)

### Bug Report Template

Use the bug report template when creating an issue. Include:

- **Description**: Clear description of the bug
- **Steps to Reproduce**: Minimal steps to reproduce the issue
- **Expected Behavior**: What should happen
- **Actual Behavior**: What actually happens
- **Environment**:
  - OS and version
  - Compiler and version
  - CMake version
  - AI Debugger version
- **Additional Context**: Stack traces, error messages, sample code

## Feature Requests

We welcome feature suggestions! When requesting a feature:

1. Check if it's already been requested
2. Clearly describe the use case
3. Explain why it would benefit the project
4. Consider if it aligns with project goals
5. Be open to discussion and alternative approaches

## Project Structure

```
LLVM-MLOpt_wp/
├── include/ai_debugger/    # Public API headers
├── src/                    # Implementation files
├── tests/                  # Test files
├── examples/               # Example programs
├── docs/                   # Documentation
└── demo/                   # Demo crash files
```

## Areas for Contribution

Looking for where to start? Here are some ideas:

- **Documentation**: Improve docs, add examples, fix typos
- **Testing**: Add tests, improve coverage
- **Bug Fixes**: Pick an issue labeled "good first issue"
- **Performance**: Optimize hot paths, reduce memory usage
- **Features**: Implement items from `ideasNext.txt`
- **Platform Support**: Test and improve support for different platforms
- **Integration**: Add IDE plugins, debugger extensions

## Questions?

- Open an issue with the "question" label
- Check existing documentation in `docs/`
- Review FAQ.md for common questions

Thank you for contributing to AI Debugger! Your efforts help make debugging easier for everyone.

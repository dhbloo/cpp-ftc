## Fundamental Template Collection (FTC)

A header-only template library.

This library serves as an additional collection of some useful C++ template utility to enhance the functionality of STL. These utility template classes are aimed to improve the code design, enhance code reuse for recurring patterns, and provide types, data structure and algorithms that are useful but not in STL.

Code is split into different modules, each of them focus on some functionality. Each class in a module is designed to have a minimized dependency, so it can be easily reused separately.

### Table of Module

(WIP)

1. **Algorithm**: Runtime algorithms
2. **Concept**: Template constraint predicates
3. **Container**: Runtime data structure that are not provided in STL
4. **Debug**: Debug helpers, such as assertion, log, etc.
5. **Function**: Function container, delegates, etc.
6. **Memory**: Memory Management, such as allocator, pmr, etc.
7. **Meta**: Template meta programming stuff, such as compile time calculation, compile time DS, etc.
8. **Reflection**: Code introspection, static reflection and runtime reflection
9. **String**: String utilities, such as safe format, string manipulation, etc.
10. **Traits**: Enhanced type traits based on STL type traits.
11. **Utility**: Miscellaneous template utilities
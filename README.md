# Memory manager simulator
Memory manager simulator is capable of allocating and editing variables and arrays for different processes. Memory is represented as a static vector of bytes.
**test** contains unit tests made with catch2
**ConsoleApplication** provides a terminal-based UI for testing different scenarios

Main classes are doxygen-style documented.

Project also contains my implementation of std::unordered_map (check HashTable.h) with a forward iterator.

## Types of memory units
- Variable
- Array
- Shared Memory (array that can be used by several processes)
- Reference

## Operations available in ConsoleApplication
- Add/Remove process
- Add/Remove/Edit memory element for each process
- Defragmentation
- Print list of all errors that occurred during execution
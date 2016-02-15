# Keskern v0.1
*A simple language based on JavaScript's Object Syntax.*

**This Repository is home too the Keskern C++ header files. Documentation can be found in this Repo's Wiki**

# Usage Example
```
#include "kesk.hpp"

int main() {

  kesk::runtime* r = keskern();
  kesk::logger log;
  
  r->execute("x = 1;", &log);
}
```

# Features

**Implemented Kesk Library Features:**
- Keskern Script to Keskern Bytecode Compiler
- Modular Bytecode Handlers
- Keskern Bytecode powered Runtime
- Tree based virtual memory structure
- Modular Compiler Types
- Recursive Decent Parser
- Standard Library of Compiler Types and Bytecode Handlers

**Implemented Standard Library Features:**
- Collections
- Lists
- Strings
- References
- Numbers

**Planned Kesk Library Features:**
- Runtime Types
  ( *There are more, just cant think of them. -Izach* )

**Planned Standard Library Features:**
- Functions ( *Executable Containers* )
- Pointers ( *Should only take a day to add. -Izach* )
- Proper Type Conversion for all types, using the yet to be added 'Runtime Types'

# Examples of STD Keskern Script

```
Box: {
  Color: [ 0, 0, 255, 255 ];
  Position: { X: 64, Y: 64 };
  Size: { Width: 128, Height: 128 };
};
```


Umo is a modeling interface for mathematical programming solvers. It provides a simple interface to various LP, MIP and SAT solvers from multiple languages.

## Using UMO

UMO currently supports a few languages and solvers, with the aim to add many more.

The following languages are supported:
* C, C++
* Python 

You can use UMO with the following solvers:
* CBC
* Minisat

### Python
```python
import umo

m = umo.Model()
a = m.bool_var()
b = m.bool_var()

umo.constraint(a + b <= 1)
umo.maximize(a + 2*b)

m.solve()

print(a.value)
print(b.value)
```

### C++
```c++
#include "api/umo.hpp"
// ...

umo::Model m;
BoolExpression a = m.boolVar();
BoolExpression b = m.boolVar();

umo::constraint(a + b <= 1);
umo::maximize(a + 2*b);

m.solve();

cout << a.value << endl;
cout << b.value << endl;
// ...
```

## Building UMO

UMO is written in C++ and uses CMake for the build. The Boost library and some optimization solvers are required to run the tests. You can build UMO with the following commands:

    mkdir build; cd build
    cmake ..
    make
    make test

For languages other than C/C++, you may install it via your package manager, or look at the build instructions in the corresponding api/ directory.


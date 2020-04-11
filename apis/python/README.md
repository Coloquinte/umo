
Umo is a modeling interface for mathematical programming solvers. 

## Using UMO

You can use UMO with the following solvers:
* CBC
* Minisat

## Python example

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


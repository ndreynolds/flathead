- Everything on the heap.
- Maybe take a similar approach as MRI does.

- A JLVALUE (i.e. RVALUE) that unionizes all of the data types

    JLVALUE {
        JLString
        JLInteger
        JLFloat
        JLArray
    }

- Need to map names (i.e. variables) to values. 
- Need to consider mutability of types.
- Store variable size types somewhere else and keep a pointer.
- Don't need to worry about GC just yet, as there is only one scope

AST-walker
Bytecode interpreter (Python does this and saves them as .pyc)
Machine code interpreter

This is just an AST-walker.


Evaluating an AST
-----------------

- Register variable declarations and assignments in the scope.
- Evaluate expressions by first reducing operands to literals and then 
  by applying ECMA5 specifications (e.g. adding strings and numbers).
- Test branch conditions (according to spec'd truth values) and then 
  branch accordingly.
- TODO: Find out how to handle loops.

Bootstrapping natives
---------------------

- Create a new object to serve as the global this.

- Assign various properties to it, e.g.:
    console  (obj)
    Math     (obj)
    Number   (func)
    String   (func)
    Object   (func)
    Boolean  (func)
    Function (func)
    NaN      (num)
    Infinity (num)

- Do do that, use:
    jl_assign_property(object, property_name, value)

- Functions can either be implemented natively, or in JavaScript.
    * The chosen implementation should be made visible by a flag. 
    * Some things need to be exposed natively (like printing to stdout), while other 
      things simply ought to be done natively for the performance benefits (e.g. sorting).
    * Native implementations are referenced by a function pointer.
    * JavaScript implementations are given as an AST node pointer.

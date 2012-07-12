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

# LSystem

A 3D [L-system](https://en.wikipedia.org/wiki/L-system) renderer.

![Fractal plant](../screenshots/fp_lsystem.png?raw=true)

#### Usage:
`./main <config file> <data file>`
#### Config format:
`<character> <instruction> ...`

Instruction can be one of the following:

* `move [nodraw] <distance>`

   Moves current position along current direction. If ``nodraw`` is not specified, a line is drawn connecting old and new positions.
* `rotx <angle>`

   `roty <angle>`
   
   `rotz <angle>`
   
   These instructions rotate local axes and direction around local X, Y and Z-axis, respectively.
* `push`

   Pushes current state onto a stack. State information includes position, direction and orientation (i.e. local axes).
* `pop`

   Pops state off the top of the stack and changes current state to it.
* `chill`

   Does nothing.

If more than one instruction is specified, instructions are executed sequentially.

#### Data format:
```
<axiom>
<number of iterations>
<rule1>
<rule2>
...
```

`axiom` is an arbitrary string that defines the initial state of the system.

Production rules `<ruleN>` have the following form:
`<char> <substitute string>`

Left side of a production rule must be a single char!

#### Controls:
`W`, `A`, `S`, `D` or `mouse` rotate camera around the origin.

`Q`, `E` move camera farther and closer to the origin.

Project Identity
================
###Names
- Leo Colobong
- Donny Reynolds

###Platform
`OS X` (Orchard Macs)

###Source Location
`./`

###Submitted by
cs61b-bw

###Compile Instruction
`make`

Demonstration
====================

###Core: Uniform Shading (Step size: 0.1)
Flat vs Smooth Shading `./renderBezier -f bezFiles/teapot.bez 0.1` + `press s` + `press w`
![teapot-u-flat](/examples/teapot_u_flat.png)
![teapot-u-smooth](/examples/teapot_u_smooth.png)
![teapot-u-wire](/examples/teapot_u_wire.png)

###Core: Adaptive Shading (Error tolerance: 0.01)
Flat vs Smooth Shading `./renderBezier -f bezFiles/teapot.bez 0.01 -a` + `press s` + `press w`
![teapot-a-flat](/examples/teapot_a_flat.png)
![teapot-a-smooth](/examples/teapot_a_smooth.png)
![teapot-a-wire](/examples/teapot_a_wire.png)

###Core: Transformation, Shading Modes and Fill Modes

###Extra Feature: Convert OBJ to BEZ Patches
`./renderBezier -f bezFiles/bunny.bez 0.1 -a`

Note: some errors come from our [obj2bez.py](obj/obj2bez.py) converter.
![bunny](/examples/bunny.png)

###
Directory Structure
=====================
- `build/`: temporary object files for linking (need to be created)
- `libraries/`: external libraries
- `source/`: source code
- `Makefile`
- `.gitignore`
- `BezierSurfaceRenderer.xcodeproj`: XCode project settings
- `bezFiles/`: default bez files for testing
- `examples/`: submitted examples
- `obj/`: obj2bez.py parser and other obj files

Command Line Usage
=====================

`./renderBezier [-f path/To/File.bez | -d path/to/dir/] stepSize|epsilon [-a] [--opengl3]`

##Parameters
`-f path/To/File.bez`: renders a single Bezier file (relative), the file must end in `.bez`

`-d path/to/dir/`: renders all the `.bez` files in the specified directory, the trailing `/` is required

`-a`: if `-a` is specified, adaptive tessellation will be used, otherwise uniform tessellation will be used

`--opengl3`: enables our incomplete extra credit modern OpenGL 3.2+ pipeline, no Phong shading is implemented, but all transformations work correctly

GUI Usage
=====================

##Drawing Control
`w`: press `w` to toggle between Wireframe and Filled drawing mode

`s`: press `s` to toggle between Smooth shading and Flat shading

`h`: press `h` to toggle between Hidden Line Removal mode and regular mode

`ESC`: press `ESC` to exit

##Transformations
`+/-`: press `+/-` to zoom in/out

`UP/DOWN/LEFT/RIGHT`: translate

`SHIFT + UP/DOWN/LEFT/RIGHT`: rotate

`Left Mouse Click`: open up a menu to choose which object to transform, selected object will be highlighted

Documentation
=====================

##External Libraries
- Eigen
- GLFW ([Doc](http://www.glfw.org/docs/latest/pages.html))
- GLEW ([Doc](http://glew.sourceforge.net/install.html))
- GLUT ([Doc](http://www.opengl.org/resources/libraries/glut/))
- OpenGL

##Main Classes
###BEZParser
Parses BEZ files containing patch data
###Tessellation
####UniformTessellation
####AdaptiveTessellation

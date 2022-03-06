# OpenGL Warmup Project
This project is a simple OpenGL 3D model renderer. It allows .obj and .mtl files to be opened and viewed. The core libraries used are GLEW, GLFW, and GLM.

## Compilation
The project uses CMake to build.

## Running
The program can be run by opening the visual studio project and launching the application in either Debug or Release mode.

## Operation
In `/config.txt`, choose which model to load and options on how to load it. The project comes with several 3D models, but new ones can be placed in the `/models/` folder and selected in `/config.txt` to be loaded.

### Options usage:
Which object should be loaded by the program. Should be a '.obj' file
`object = cube.obj`
Scale of the object in the program. Should be in range (0, 1]
`scale = 0.6`
Position of the object in the program. Marked as <x, y, z> coordinates
`position = 0.0 0.0 0.0`
Default color of meshes. Marked as <r, g, b> data
`dcolor = 0.8 0.8 0.8`
Vertex model. [0] separate, [1] indexed. Indexed data structure may cause issues with complicated models with non-triangular faces
`vertexmodel = 0`
Should model be drawn in wireframe? [0] No, [1] Yes
`wireframe = 0`
Should the model information be printed in the console? [0] No, [1] Yes
`print = 0`

### Program usage:
The program does not include user interaction after launching beyond closing the program. To change how the program runs, modify config.txt
When the program is open, press `Esc` to exit the program.

### Links to libraries:
http://glew.sourceforge.net/
https://www.glfw.org/
https://github.com/g-truc/glm
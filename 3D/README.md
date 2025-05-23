# Render a .obj model

<img align="right" src="./media/bunnyWireframe.png" alt="Stanford Bunny" width="400px"/>

> "Introducing the Stanford Bunny! Wait Stanford...how about a Husky?"

# Resources to help

Some additional resources to help you through this lab assignment

| SDL2 related links                                    | Description                       |
| --------------------------------------------------    | --------------------------------- |
| [SDL API Wiki](https://wiki.libsdl.org/APIByCategory) | Useful guide to all things SDL2   |
| [My SDL2 Youtube Playlist](https://www.youtube.com/playlist?list=PLvv0ScY6vfd-p1gSnbQhY7vMe2rng0IL0) | My Guide for using SDL2 in video form.   |
| [Lazy Foo](http://lazyfoo.net/tutorials/SDL/)         | Great page with written tutorials for learning SDL2. Helpful setup tutorials for each platform. |
| [Lazy Foo - Handling Key Presses](https://lazyfoo.net/tutorials/SDL/04_key_presses/index.php) | Useful tutorial for learning how to handle key presses | 

| OpenGL related links                                | Description                       |
| --------------------------------------------------  | --------------------------------- |
| [My OpenGL Youtube Series](https://www.youtube.com/playlist?list=PLvv0ScY6vfd9zlZkIIqGDeG5TUWswkMox) | My video series for learning OpenGL |
| [docs.gl](http://docs.gl)                           | Excellent documentation to search for OpenGL commands with examples and description of function parameters   |
| [learnopengl.com](https://learnopengl.com)          | OpenGL 3.3+ related tutorial and main free written resource for the course   |

| C++ related links                                   | Description                       |
| --------------------------------------------------  | --------------------------------- |
| [My C++ Youtube Series](https://www.youtube.com/playlist?list=PLvv0ScY6vfd8j-tlhYVPYgiIyXduu6m-L) | My video series playlist for learning C++ |
| [cppreference](https://en.cppreference.com/w/)      | Definitive, more encyclopedic guide to C++ (less beginner focused, but excellent technically) |
| [cplusplus.com](http://www.cplusplus.com)           | Nice website with examples and tutorials geared more for beginners, reference has lots of examples, and the tutorial page is a great starting point |
| [learncpp.com](https://www.learncpp.com/)           | Handy page for learning C++ in tutorial form   |


- [Lazy Foo keyboard input with SDL2 tutorial](http://lazyfoo.net/tutorials/SDL/04_key_presses/index.php)
    - Lazy Foo tutorial on keyboard input
- Help with OBJ Model Format
	- [Briefing on the .obj format](https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html) (I recommend reading this one for more information)
	- [Another nice writeup on .obj format](https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html)
	- [wiki page on wavfront .obj format](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
	- [Comparison of different .obj loaders](https://aras-p.info/blog/2022/05/14/comparing-obj-parse-libraries/)
- OpenGL Related
	- [Index Buffer Object](http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/]
		- For help with the Index buffer strategy

# Description

You have learned that we can draw polygons to the screen by plotting them out one triangle at a time both in a software render and in OpenGL. We can even use mathematical functions to draw nice curves and geometric patterns (e.g. the [sierpinski triangle](https://en.wikipedia.org/wiki/Sierpi%C5%84ski_triangle)) to draw more interesting graphical scenes.  Most often however complex shapes will either be modeled in 3D by artists [[Breaking into the Game Industry: 3D Character Art
](https://www.youtube.com/watch?v=SaPkbl5n7Pc)] [[Meet Anshelle – 3D Artist](https://www.youtube.com/watch?v=funtOiN3gKY)] or otherwise scanned from the real world by [lasers to capture the geometry](https://www.youtube.com/watch?v=1lDO1UevAJI) of an object. This data is then output in a 3D file format that can be read in by the CPU and then drawn to the screen using our GPUs.

One of the more famous 3D models is the [Stanford Bunny](https://en.wikipedia.org/wiki/Stanford_bunny). This bunny was a scan of a ceramic figurine (don't worry--no bunnies were harmed!) by a laser at Stanford University in 1994. Other common models that are used in computer graphics are the [Stanford Dragon](https://en.wikipedia.org/wiki/Stanford_dragon), the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot), or [Suzanne](https://en.wikipedia.org/wiki/Blender_(software)#Suzanne).

## Part 1 - Render a model

### Task 1 - Model loading

For this assignment you are going to render a 3D model in the [wavefront .obj format](https://en.wikipedia.org/wiki/Wavefront_.obj_file). Some sample models are provided in: [./../../common/objects/](./../../common/objects/)

The tasks for rendering in this assignment are the following:

1. Read in a .obj file from the command-line arguments.
	- [If you'd like, you can watch a video here on reading in command line arguments](https://youtu.be/C2Vhp-ozA0k)
2. Make sure you can parse the .obj file format (i.e. read in the vertices and faces). This is your geometry data.
	- I recommend you build a custom class (e.g. OBJ) for this task. 
3. Make sure you can store the geometry data.
4. Make sure you know how to work with vertex buffers and index buffers.
 	- You can use either glDrawArrays or an indexed-object strategy to be more efficient in loading the .obj file.
 	- Here is a resource motivating index buffers: [Index Buffer Object](http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/)

### OBJ Format

The models that you will be loading are in the [.obj format](https://en.wikipedia.org/wiki/Wavefront_.obj_file). This format stores the vertices, normals, and faces of an object (and potentially other information as well). The easiest way to become familiar with the OBJ file format is to investigate a 3D Cube model. Here is a cube exported in the .obj file format from the free tool [Blender3D](https://www.blender.org/).

![Alt text](./media/cube.png "Cube in Blender3D")

### Parsing the file

(For this assignment, ignore the .mtl materials files)
```
mtllib cube.mtl
o Cube1
#8 vertices, 12 faces
v -1.00000000 -1.00000000 -1.00000000
v -1.00000000 -1.00000000 1.00000000
v -1.00000000 1.00000000 -1.00000000
v -1.00000000 1.00000000 1.00000000
v 1.00000000 -1.00000000 -1.00000000
v 1.00000000 -1.00000000 1.00000000
v 1.00000000 1.00000000 -1.00000000
v 1.00000000 1.00000000 1.00000000
vn -0.57735027 -0.57735027 -0.57735027
vn -0.57735027 -0.57735027 0.57735027
vn -0.57735027 0.57735027 -0.57735027
vn -0.57735027 0.57735027 0.57735027
vn 0.57735027 -0.57735027 -0.57735027
vn 0.57735027 -0.57735027 0.57735027
vn 0.57735027 0.57735027 -0.57735027
vn 0.57735027 0.57735027 0.57735027
g Cube1_default
usemtl default
s 1
f 1//1 5//5 2//2
f 2//2 3//3 1//1
f 2//2 5//5 6//6
f 2//2 8//8 4//4
f 3//3 5//5 1//1
f 3//3 8//8 7//7
f 4//4 3//3 2//2
f 4//4 8//8 3//3
f 5//5 8//8 6//6
f 6//6 8//8 2//2
f 7//7 5//5 3//3
f 7//7 8//8 5//5
```

For this file, the encoding is the following:
* Lines that start with *v* stands for a 'vertex'.
	* A vertex has an x,y,z coordinate that follows.
* Lines that start with *vn* stands for a 'vertex normal'.
	* A vertex normal is a normalized directional vector(from 0,0,0 to its location)
* Lines that start with *f* stands for 'face'.
	* A face can consist of any number of vertices. 
	* The first number is the index of the vertex we are using. 
	* The second number after the double slashes (//) is the vertex normal index in the above example (Note that if we add in texture coordinates later, there maybe be a number between the slashes)
* The other fields (that start with o, s, or the .mtl file) can be ignored for this assignment.
* Note that there are no '0' values in the list. This means .obj is 1's based. In most languages we count from 0, so think about what this means when parsing!

### Task 2 - Interactive graphics

For task 2, we are going to utilize SDL to make our graphics application interactive. 

The tasks for interactivity in this assignment are the following:
- Pressing the <kbd>tab</kbd> key draws your object in wireframe mode (By default when you start the application it will show the model in filled).
- Pressing the <kbd>Esc</kbd> key exits the application.
- Pressing the <kbd>1</kbd> key renders a .obj model read in from the command-line (It is fine to have the object load at the start as well)
- (Optional) Pressing the <kbd>2</kbd> - <kbd>9</kbd> key renders any other objects that have been read in from the command-line.
	- e.g. (./prog "./../../common/objects/bunny_centered.obj" "./../../common/objects/capsule/capsule.obj")
		- Pressing 1 would render the bunny, and a 2 would render the capsule.

A resource for performing [keyboard input with SDL is provided here](http://lazyfoo.net/tutorials/SDL/04_key_presses/index.php)

### Assignment strategy

My suggested strategy for this project is:

* You can use any of the code from the previous assignments that you may find useful.
	* Generally speaking, I've found that folks who go back to the 'index-buffer' or 'first triangle' assignments are able to build better abstractions.
   		* Why? 1. Because you understand the process.
       		* Once you understand how to draw one triangle, rendering many is not much more difficult :)
* Have a C++ class (in a .hpp and .cpp file) for loading OBJ models--not doing so is bad style!
	* For example, `class OBJModel` would be an ideal name for handling the loading and rendering of an OBJ model.
	* Utilize within the class a STL data structure (e.g. std::vector, std::array, etc.)
  	* You may assume all faces are triangles (though if you download additional test .obj files from the web this may not be the case)
* Think about how you can load a line of text and then split it into individual tokens.
  	* A resource loading files (Filo I/O) in C++ can be found here: http://www.cplusplus.com/doc/tutorial/files/
  	* The reference on strings may be useful: http://www.cplusplus.com/reference/string/string/
    		* Some ideas on string splitting: http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
* You do not need to use any complex glsl fragment or vertex shaders. In fact, I recommend using the most basic ones for this assignment.
* I recommend using the [docs.gl](http://docs.gl/gl3/glPolygonMode) webpage for help with OpenGL. 
* Note, that depending on where your camera is, you may be inside the object--consider that when you are rendering (in the case of the cube for example, you will see an 'X' in your display). You can 'translate' the model if need be to adjust its position -- most of my provided .obj files are centered at the origin (e.g. bunny_centered.obj).

#### Debugging

If you don't see a 3D model appearing on your screen after your attempt, here are some tips:

1. Try rendering in wire frame mode first to see if anything shows up (A quick google of 'how do I render wireframe in OpenGL' will show you how to do this) 
2. Try loading a few different models (of different sizes) to see if your .obj loader works.
3. Ensure that you are loading your buffers with the correct number of vertices and indices (i.e. A common mistake I see is the improper use of 'sizeof' which returns the data type size, not the actual number of elements in a collection. See std::vector.size() for example)
4. Try to write a 'print()' function to display all the vertices and indices.
	- The vertices and indices printed out should match the data you are parsing.
6. If your bunny looks 'almost right', but a little bit scrambled -- Careful also with 'off by 1 errors'
	- Remember, the face list in the .obj uses a 1-based index, whereas we as C++ programmers are used to a 0-based index.
	- Thus, you have to 'subtract 1' from the index when parsing the .obj file and storing the data on your CPU.
7. Sometimes folks mix up GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER for where the vertices (the former), and indices (the latter GL_ELEMENT_ARRAY_BUFFER) should be used.
8. I often see folks do this in their code `float verts[some_vector.size()]` 
	- This is dangerous, as if some_vector.size() is a really big value, 'verts' which is a local variable (which is an array), and 'verts' is thus located on the stack which has a small amount of memory that may not be able to allocate for very large objects.

## How to compile and run your program

1. Your solution should compile using the [build.py](./build.py) file. 
	- That is, we will run your program by typing [python3 build.py](./build.py) and it should just work.
2. After compiling your code, we will run the executable with an argument to a .obj model.

# Submission/Deliverables

### Submission

- Commit all of your files to github, including any additional files you create.
- Do not commit any binary files unless told to do so.
- Do not commit any 'data' files generated when executing a binary.

### Deliverables

- You should be able to load and display a bunny_centered.obj and monkey_centered.obj at the least
	- The ./common/objects directory in your repository has some sample models.
	- You can substitute these models with other .obj's you find if you like.
- Your solution should be interactive, and utilize the keypresses as described in Task 2

* You need to commit your code to this repository.
* You need to use the build.py script provided. Anything else used is at your own risk--and you should provide complete documentation. If your program does not compile and run, you get a zero!


# F.A.Q. (Instructor Anticipated Questions)

* Q: Why the obj format?
  * A: It is a standard data format understood by most programs.
  * Here's a copy of the spec if you want to read more: http://www.martinreddy.net/gfx/3d/OBJ.spec
* Q: Can I load my own models that I have made to show off?
  * A: Sure -- just make sure they are added to the repository.
* Q: May I use the command line arguments to pass in a default model?
  * A: Sure -- please document at the top of your readme.md file so I am aware of this feature.
* Q: Will I lose points if I keep creating the same vertex buffer in a loop?
  * A: Yes, because you only need to allocate memory on the GPU once. Doing so many times is wasteful, and it will slow down your program by having to make the CPU and GPU communicate over and over again. Vertex Buffer Objects need only be recreated if the geometry changes.
* Q: Should I store the vertex normals, texture coordinates, and other information?
  * A: It's not necessary for this assignment, but we will later normal and texture coordinate information.
* Q: Are the vertex normals always the same as the vertex number?
  * A: Nope! And we'll want to make sure the store those vertex normals in a separate array. It just happens to be that for models like the cube, all of the normals would be unique, because the shared vertex normal is pointing in a different direction. 
 	 * Note: We could compute the normals by hand: https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
 	 * Note: We could compute the normals using a 'weight' based on the area of the triangle: https://www.cat.nyu.edu/~perlin/courses/spring2012/computing-normals.html 
* Q: I heard a cube with only 8 normals is weird?
	* A: Yeah they could be a weird case if you want to do a flat shaded rendering properly. The smooth shader version can look spherical depending on the lighting. You would instead have to have unique vertices at each of the edges and uniquely generate each of the normals to use depending on which direction the face is. This explains it somewhat: https://stackoverflow.com/questions/5046579/vertex-normals-for-a-cube
* Q: My program is segfaulting everytime I call an OpenGL function (e.g. glGenVertexArrays)
* A: You probably have not initialized OpenGL using glad and SDL2 to get a context. Calls to OpenGL functions will otherwise cause a 'bad excess error' or something equivalent and your program will terminate.
* Q: My bunny is almost right, but something looks off
  	* Here is an example (Using vertex normals as the r,g,b values in the shader):
  	* <img src="./media/depth.png" alt="Stanford Bunny" width="300px"/>
* A: Try playing with glEnable(GL_DEPTH_TEST) and glEnable(GL_CULL_FACE).

# Found a bug?

If you found a mistake (big or small, including spelling mistakes) in this lab, kindly send me an e-mail. It is not seen as nitpicky, but appreciated! (Or rather, future generations of students will appreciate it!)

- Fun fact: The famous computer scientist Donald Knuth would pay folks one $2.56 for errors in his published works. [[source](https://en.wikipedia.org/wiki/Knuth_reward_check)]
- Unfortunately, there is no monetary reward in this course :)

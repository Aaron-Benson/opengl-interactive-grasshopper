# OpenGL Interactive Grasshopper

![alt tag](https://raw.githubusercontent.com/Aaron-Benson/opengl-interactive-grasshopper/master/graphic.PNG)

This is a simple program using OpenGL to draw a scene of a grasshopper. You are able to control the movement and positioning of the grasshopper by passing input to the program. Transformations are done using a hierarchy through OpenGL's Vertex Buffer Objects (VBO).

Programmed in C++.

## Installation 

1. Clone the repository locally.
2. Link OpenGL, freeglut (v 3.0.0), glm (v 0.9.8.4), and glew (v 2.0.0) to the project.
3. Compile and run using Visual Studio or other IDE.

## Controls

f - Move the grasshopper forward

b - Move the grasshopper backward

l - Move the grasshopper to the left

r - Move the grasshopper to the right

t - Turn the grasshopper counter clockwise by 15 degrees

T - Turn the grasshopper clockwise by 15 degrees


7 - Rotate outer left top leg toward head

& - Rotate outer left top leg away from head

u - Rotate outer left bottom leg toward head

U - Rotate outer left bottom leg away from head

8 - Rotate inner left top leg toward head

\* - Rotate inner left top leg away from head

i - Rotate inner left bottom leg toward head

I - Rotate inner left bottom leg away from head

0 - Rotate outer right top leg toward head

) - Rotate outer right top leg away from head

p - Rotate outer right bottom leg toward head

P - Rotate outer right bottom leg away from head

9 - Rotate inner right top leg toward head

( - Rotate inner right top leg away from head

o - Rotate inner right bottom leg toward head

O - Rotate inner right bottom leg away from head

h - Scale head down

H - Scale head up

## Credits

Aaron Benson
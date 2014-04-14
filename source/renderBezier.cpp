//
//  renderBezier.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/3/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "renderBezier.h"
#include "UnitTest.cpp"
#include "BEZParser.h"
#include "Tessellation.h"
#include "UniformTessellation.h"
#include "AdaptiveTessellation.h"

void rasterizeMeshes(vector<Mesh>& meshes, vector<RasterMesh>& rasters);
void renderToOpenGL(vector<RasterMesh> meshes);
void renderToOldOpenGL(int argc, char *argv[]);

vector<RasterMesh> rasterMeshes;

int main(int argc, char *argv[]) {
    vector<CmdLineOptResult> *results;
    string basePath;
    string options = "--testOpenGL(0)"; //test OpenGL, GLEW, and GLFW
    options.append("--testBEZParser(0)"); //test BEZParser for files at "bezFiles/"
    options.append("--testUniform(0)"); //test UniformTessellation
    options.append("--testAdaptive(0)"); //test AdaptiveTessellation
    
    options.append("-f(2)"); //render file, param
    options.append("-d(2)"); //render directory of files, param
    options.append("-a(0)"); //use adaptive tessellation
    
    getCmdLineOptions(argc, argv, options, &results);
    
    float param = 0.0;
    string directoryName;
    string fileName;
    bool adaptive = false;
    
    for (auto & result : *results) {
        if (result.optName.compare("--testOpenGL")==0) {
            testOpenGL();
        }
        if (result.optName.compare("--testBEZParser")==0) {
            testBEZParser();
        }
        if (result.optName.compare("--testUniform")==0) {
            testUniform();
        }
        if (result.optName.compare("--testAdaptive")==0) {
            testAdaptive();
        }
        if (result.optName.compare("-f")==0) {
            fileName = result.args->at(0);
            param = floatFromString(result.args->at(1));
        }
        if (result.optName.compare("-d")==0) {
            directoryName = result.args->at(0);
            param = floatFromString(result.args->at(1));
        }
        if (result.optName.compare("-a")==0) {
            adaptive = true;
        }
    }
    
    //Render
    if ((!directoryName.empty() || !fileName.empty()) && param > 0.0) {
        unique_ptr<BEZParser> parser(new BEZParser());
        unique_ptr<Tessellation> tessellator;
        vector<BezierObject> objects;
        vector<Mesh> meshes;
        
        if (adaptive) {
            tessellator = unique_ptr<Tessellation>(new AdaptiveTessellation());
            tessellator->setErrorRate(param);
        } else {
            tessellator = unique_ptr<Tessellation>(new UniformTessellation());
            tessellator->setDivDepth((int)(1/param));
        }
        if (!directoryName.empty()) {
            parser->parseDirectory(directoryName, objects);
        }
        if (!fileName.empty()) {
            parser->parseFile(fileName, objects);
        }
        tessellator->tessellate(objects, meshes);
        //render mesh openGL
        rasterizeMeshes(meshes, rasterMeshes);
        renderToOldOpenGL(argc, argv);
    }
    return 0;
}


void rasterizeMeshes(vector<Mesh>& meshes, vector<RasterMesh>& rasters) {
    for (auto & mesh : meshes) {
        if (mesh.type == UniformMesh) {
            RasterMesh raster;
            float *newVertices = new float[mesh.numOfVertices*3];
            for (int vertex=0; vertex < mesh.numOfVertices; vertex++) {
                newVertices[vertex*3+0] = mesh.vertices[vertex](0);
                newVertices[vertex*3+1] = mesh.vertices[vertex](1);
                newVertices[vertex*3+2] = mesh.vertices[vertex](2);
            }
            delete mesh.vertices;
            raster.vertices = newVertices;
            raster.numOfVertices = mesh.numOfVertices*3;
            raster.indices = mesh.indices;
            raster.numOfIndices = mesh.numOfIndices;
            rasters.push_back(raster);
        }else if (mesh.type == AdaptiveMesh){
            RasterMesh raster;
            float *newVertices = new float[mesh.numOfVertices*3];
            int *newIndices = new int[mesh.numOfIndices];
            for (int vertex=0; vertex < mesh.numOfVertices; vertex++) {
                newVertices[vertex*3+0] = mesh.adaptiveVertices->at(vertex)(0);
                newVertices[vertex*3+1] = mesh.adaptiveVertices->at(vertex)(1);
                newVertices[vertex*3+2] = mesh.adaptiveVertices->at(vertex)(2);
                newIndices[vertex] = vertex;
            }
            delete mesh.adaptiveVertices;
            raster.vertices = newVertices;
            raster.indices = newIndices;
            raster.numOfIndices = mesh.numOfIndices;
            raster.numOfVertices = mesh.numOfVertices*3;
            rasters.push_back(raster);
        }
    }
}

/*
 Reference:
 typedef struct {
 size_t numOfVertices;
 size_t numOfIndices;
 Vector *vertices;
 int *indices;
 } Mesh;
 */




bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)
void keyOperations (void) {
    if (keyStates[GLUT_KEY_LEFT]) { // If the left arrow key has been pressed
        // Perform left arrow key operations
    }  
}
void keyPressed (unsigned char key, int x, int y) {
    keyStates[key] = true; // Set the state of the current key to pressed
}
void keyUp (unsigned char key, int x, int y) {
    keyStates[key] = false; // Set the state of the current key to not pressed
}

void reshape (int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
    gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

void renderMesh() {
    
    RasterMesh rasterMesh = rasterMeshes[0];
    for (int v = 0; v < rasterMesh.numOfVertices; v+=9) {
        glBegin(GL_TRIANGLES);
        glVertex3f(rasterMesh.vertices[v+0],
                   rasterMesh.vertices[v+1],
                   rasterMesh.vertices[v+2]);
        glVertex3f(rasterMesh.vertices[v+3],
                   rasterMesh.vertices[v+4],
                   rasterMesh.vertices[v+5]);
        glVertex3f(rasterMesh.vertices[v+6],
                   rasterMesh.vertices[v+7],
                   rasterMesh.vertices[v+8]);
//        glColor3f(1.0, 0.0, 0.0);
        glEnd();
    }
}

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0); //clear the screen to
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    GLfloat DiffuseLight[] = {1, 0, 0}; //set DiffuseLight
    GLfloat AmbientLight[] = {1, 0, 0}; //set AmbientLight
    GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0};
    
    glLightfv (GL_LIGHT0, GL_DIFFUSE, DiffuseLight); //change
    glLightfv (GL_LIGHT1, GL_AMBIENT, AmbientLight); //change
    
    GLfloat LightPosition[] = {0, 0, 1, 0}; //set the
    glLightfv (GL_LIGHT0, GL_POSITION, LightPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);

    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0f, 0.0f, -10.0f); // Push eveything 5 units back into the scene, otherwise we won't see the primitive
    glRotatef(90, 1.0, 0.0, 0.0); //rotate on the x axis
    renderMesh();
    glutSwapBuffers(); // Flush the OpenGL buffers to the window
}

void renderToOldOpenGL(int argc, char *argv[]) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (800, 600); // Set the width and height of the window
    glutInitWindowPosition (100, 100); // Set the position of the window
    glutCreateWindow ("BezierRenderer Leo C. & Donny R."); // Set the title for the window
    //for lighting
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHT1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering
    glutIdleFunc (display);
    glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping
    
    glutMainLoop(); // Enter GLUT's main loop
}

void renderToOpenGL(vector<RasterMesh> meshes) {
    // Shader sources
    const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec3 position;"
    "in vec3 color;"
    "out vec3 Color;"
    "uniform mat4 trans;"
    "void main() {"
    "   Color = color;"
    "   gl_Position = trans * vec4(position, 1.0);"
    "}";
    const GLchar* fragmentSource =
    "#version 150 core\n"
    "uniform vec3 Color;"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(Color, 1.0);"
    "}";
    
    if (!glfwInit()) {
        cerr << "GLFW cannot initialize" << endl;
        return;
    }
    
    //Only version 3.2 and up
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //No resize
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Important, otherwise "glfCreateWindow" will return NULL
    GLFWwindow* window = glfwCreateWindow(800, 600, "BezierShader", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    if (!window) {
        cerr << "GLFW cannot create window" << endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "GLEW cannot initialize" << endl;
        return;
    }
    
    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    // Uniform Transformation
    Transform3fAffine trans(Translation3f(0,1,-3.5));
    trans = trans*Transform3fAffine(AngleAxisf(0.25*M_PI, Vector3f::UnitX()));
    GLint uniColor = glGetUniformLocation(shaderProgram, "Color");
    glUniform3f(uniColor, 0.5f, 0.5f, 0.5f);
    GLint uniTrans = glGetUniformLocation(shaderProgram, "trans");
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
    
    RasterMesh mesh = meshes.at(0);
    /* Create 3D Buffer */
    
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.numOfVertices*sizeof(float), mesh.vertices, GL_STATIC_DRAW);
    
    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.numOfIndices*sizeof(int), mesh.indices, GL_STATIC_DRAW);
    
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    while(!glfwWindowShouldClose(window))
    {
        //poll and process events
        glfwPollEvents();
        
        GLenum drawingMode = GL_TRIANGLES;

        if (glfwGetKey(window, GLFW_KEY_W)) {
            drawingMode = GL_TRIANGLES ? GL_LINE_LOOP : GL_TRIANGLES;
        }
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawElements(drawingMode, mesh.numOfIndices, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        
        cout << glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) << endl;
        //hanldes ESC quit
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        //handles rotate and translate left
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
                trans = Transform3fAffine(Translation3f(-0.01,0,0))*trans;
            }else{
                trans = Transform3fAffine(AngleAxisf(-M_PI/128,Vector3f::UnitY()))*trans;
            }
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
        //handles rotate and translate right
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
                trans = Transform3fAffine(Translation3f(0.01,0,0))*trans;
            }else{
                trans = Transform3fAffine(AngleAxisf(M_PI/128,Vector3f::UnitY()))*trans;
            }
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
        //handles rotate and translate up
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
                trans = Transform3fAffine(Translation3f(0,0.01,0))*trans;
            }else{
                trans = Transform3fAffine(AngleAxisf(-M_PI/128,Vector3f::UnitX()))*trans;
            }
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
        //handles rotate and translate down
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
                trans = Transform3fAffine(Translation3f(0,-0.01,0))*trans;
            }else{
                trans = Transform3fAffine(AngleAxisf(M_PI/128,Vector3f::UnitX()))*trans;
            }
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
        //handles zoom in
        if (glfwGetKey(window, GLFW_KEY_EQUAL)) {
            trans = Transform3fAffine(Scaling3f(1.01,1.01,1.01))*trans;
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
        //handles zoom out
        if (glfwGetKey(window, GLFW_KEY_MINUS)) {
            trans = Transform3fAffine(Scaling3f(0.99,0.99,0.99))*trans;
            glUniformMatrix4fv(uniTrans, 1, GL_FALSE, trans.matrix().data());
        }
    }
    
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    
    glDeleteVertexArrays(1, &vao);
    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    
    glfwTerminate();
}
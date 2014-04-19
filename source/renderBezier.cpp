//
//  renderBezier.cpp
//  BezierSurfaceRenderer
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.


#include "renderBezier.h"
/*
 +––––––––––––––––––––––––––––––––––––––+
 |Main Functions                        |
 +––––––––––––––––––––––––––––––––––––––+
 Process command line options and kickstart tessellation and OpenGL
 */
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
    options.append("--opengl3(0)"); //use OpenGL 3.2 modern pipeline and shader

    getCmdLineOptions(argc, argv, options, &results);
    
    float param = 0.0;
    string directoryName;
    string fileName;
    
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
        if (result.optName.compare("--opengl3")==0) {
            opengl3 = true;
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
        
        //rasterize meshes
        rasterizeMeshes(meshes, rasterMeshes);
        //create transformation matrices
        for (int i=0;i<rasterMeshes.size();i++) {
            transformations.push_back(Matrix4f::Identity());
        }
        //render using OpenGL
        if (opengl3)
            renderToOpenGL3(rasterMeshes);
        else
            renderToOpenGL(argc, argv);
    }
    
    return 0;
}

void quit() {
    //cleanup
    for (auto& mesh : rasterMeshes) {
        delete[] mesh.vertices;
        delete[] mesh.indices;
    }
    for (auto& mesh : meshes) {
        //delete[] mesh.indices;
        delete[] mesh.vertices;
        delete[] mesh.adaptiveVertices;
    }
    for (auto& bezier : objects) {
        delete[] bezier.controlPoints;
    }
    exit(0);
}

// Convert mesh Vector arrays to x-y-z arrays
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
            for (int vertex=0; vertex < mesh.numOfVertices; vertex++) {
                newVertices[vertex*3+0] = mesh.adaptiveVertices->at(vertex)(0);
                newVertices[vertex*3+1] = mesh.adaptiveVertices->at(vertex)(1);
                newVertices[vertex*3+2] = mesh.adaptiveVertices->at(vertex)(2);
            }
            delete mesh.adaptiveVertices;
            raster.vertices = newVertices;
            raster.indices = mesh.indices;
            raster.numOfIndices = mesh.numOfIndices;
            raster.numOfVertices = mesh.numOfVertices*3;
            rasters.push_back(raster);
        }
    }
}

/*
 +––––––––––––––––––––––––––––––––––––––+
 |Menu Event Handling                   |
 +––––––––––––––––––––––––––––––––––––––+
 */
// Function to assign selected object to current obj subject to transformation
void menuItemPressed(int value) {
    currentObj = value;
}
//Create menu to allow selective transformation
void createMenu() {
    menuHandle = glutCreateMenu(menuItemPressed);
    glutSetMenu(menuHandle);
    for (int i=0; i<rasterMeshes.size(); i++) {
        string entry = string("Object ") + to_string(i+1);
        glutAddMenuEntry(entry.c_str(), i);
    }
}


/*
+––––––––––––––––––––––––––––––––––––––+
|Keyboard Event Handling               |
+––––––––––––––––––––––––––––––––––––––+
*/
// Keyboard function to handle some special keys for transformations
void specialKeyPressed(int key, int x, int y) {
    int mod;
    switch(key) {
        case GLUT_KEY_UP:
            mod = glutGetModifiers();
            if (mod == GLUT_ACTIVE_SHIFT) {
                gTrans = Transform3fAffine(Translation3f(0,0.09,0))*gTrans;
            } else {
                gTrans = gTrans*Transform3fAffine(AngleAxisf(-M_PI/128,Vector3f::UnitX())).matrix();
            }
            break;
        case GLUT_KEY_DOWN:
            mod = glutGetModifiers();
            if (mod == GLUT_ACTIVE_SHIFT) {
                gTrans = Transform3fAffine(Translation3f(0,-0.09,0))*gTrans;
            } else {
                gTrans = gTrans*Transform3fAffine(AngleAxisf(M_PI/128,Vector3f::UnitX())).matrix();
            }
            break;
        case GLUT_KEY_LEFT:
            mod = glutGetModifiers();
            if (mod == GLUT_ACTIVE_SHIFT) {
                gTrans = Transform3fAffine(Translation3f(-0.09,0,0))*gTrans;
            } else {
                gTrans = gTrans*Transform3fAffine(AngleAxisf(-M_PI/128,Vector3f::UnitY())).matrix();
            }
            break;
        case GLUT_KEY_RIGHT:
            mod = glutGetModifiers();
            if (mod == GLUT_ACTIVE_SHIFT) {
                gTrans = Transform3fAffine(Translation3f(0.09,0,0))*gTrans;
            } else {
                gTrans = gTrans*Transform3fAffine(AngleAxisf(M_PI/128,Vector3f::UnitY())).matrix();
            }
            break;
    }
}
// Keyboard function to handle regular keys used for transformations and other commands
void keyPressed (unsigned char key, int x, int y) {
    keyStates[key] = true; // Set the state of the current key to pressed
    //for toggling between different modes
    switch (key) {
        case 's': //toggle between FLAT and SMOOTH shading
            shadeToggle = true;
            break;
        case 'w': //toggle between FILLED and WIREFRAME drawing
            fillToggle = true;
            break;
        case 27: // Escape key to exit
            quit();
            break;
        case '-': //zoom out
            gTrans = Transform3fAffine(Translation3f(0,0,-0.09))*gTrans;
            break;
        case '=': //zoom in
        case '+': //zoom in
            gTrans = Transform3fAffine(Translation3f(0,0,0.09))*gTrans;
            break;
        default:
            break;
    }
}
// Handles when keyboard keys are released
void keyUp (unsigned char key, int x, int y) {
    keyStates[key] = false; // Set the state of the current key to not pressed
}


/*
 +––––––––––––––––––––––––––––––––––––––+
 |Resize Handling                       |
 +––––––––––––––––––––––––––––––––––––––+
 */
void reshape (int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
    gluPerspective(60, //Field of view angle (in degrees)
                   (GLfloat)width / (GLfloat)height, //aspect ratio of our window
                   1.0, 100.0); // new and far planes
    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

/*
 +––––––––––––––––––––––––––––––––––––––+
 |Drawing Module                        |
 +––––––––––––––––––––––––––––––––––––––+
 */
void renderMesh(RasterMesh& rasterMesh) {
    GLenum drawingMode = adaptive ? GL_TRIANGLES : GL_QUADS;
    int increment = adaptive ? 3 : 6;
    
    for (int v = 0; v < rasterMesh.numOfIndices; v += increment) {
        glBegin(drawingMode);
        glNormal3f(rasterMesh.vertices[6*rasterMesh.indices[v]+3],
                   rasterMesh.vertices[6*rasterMesh.indices[v]+4],
                   rasterMesh.vertices[6*rasterMesh.indices[v]+5]);
        glVertex3f(rasterMesh.vertices[6*rasterMesh.indices[v]+0],
                   rasterMesh.vertices[6*rasterMesh.indices[v]+1],
                   rasterMesh.vertices[6*rasterMesh.indices[v]+2]);
        glNormal3f(rasterMesh.vertices[6*rasterMesh.indices[v+1]+3],
                   rasterMesh.vertices[6*rasterMesh.indices[v+1]+4],
                   rasterMesh.vertices[6*rasterMesh.indices[v+1]+5]);
        glVertex3f(rasterMesh.vertices[6*rasterMesh.indices[v+1]+0],
                   rasterMesh.vertices[6*rasterMesh.indices[v+1]+1],
                   rasterMesh.vertices[6*rasterMesh.indices[v+1]+2]);
        glNormal3f(rasterMesh.vertices[6*rasterMesh.indices[v+2]+3],
                   rasterMesh.vertices[6*rasterMesh.indices[v+2]+4],
                   rasterMesh.vertices[6*rasterMesh.indices[v+2]+5]);
        glVertex3f(rasterMesh.vertices[6*rasterMesh.indices[v+2]+0],
                   rasterMesh.vertices[6*rasterMesh.indices[v+2]+1],
                   rasterMesh.vertices[6*rasterMesh.indices[v+2]+2]);
        if (!adaptive) {
            glNormal3f(rasterMesh.vertices[6*rasterMesh.indices[v+5]+3],
                       rasterMesh.vertices[6*rasterMesh.indices[v+5]+4],
                       rasterMesh.vertices[6*rasterMesh.indices[v+5]+5]);
            glVertex3f(rasterMesh.vertices[6*rasterMesh.indices[v+5]+0],
                       rasterMesh.vertices[6*rasterMesh.indices[v+5]+1],
                       rasterMesh.vertices[6*rasterMesh.indices[v+5]+2]);
        }
//        chrono::milliseconds timespan(1); // or whatever
//        this_thread::sleep_for(timespan);
        glEnd();
    }
}

/*
 +––––––––––––––––––––––––––––––––––––––+
 |Display Control                       |
 +––––––––––––––––––––––––––––––––––––––+
 */
void display (void) {
    glClearColor (0.0,0.0,0.0,1.0); //clear the screen to
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    GLfloat DiffuseLight[] = {1, 1, 1}; //set DiffuseLight
    GLfloat AmbientLight[] = {0.5, 0.5, 0.5}; //set AmbientLight
    GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0};
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight); //change
    glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight); //change
    
    GLfloat LightPosition[] = {0, 0, 1, 0}; //set the
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);

    gluLookAt (0.0, 0.0, 5.0, //eyeX, eyeY, eyeZ
               0.0, 0.0, 0.0, //centerX, centerY, centerZ
               0.0, 1.0, 0.0); //upX, upY, upZ
    
    glTranslatef(0.0f, 0.0f, -5.0f); // Push eveything 5 units back into the scene, otherwise we won't see the primitive
    
    //loop through all meshes and graph with a different transformation
    for (int meshIdx=0; meshIdx<rasterMeshes.size(); meshIdx++) {
        glPushMatrix();
        
        GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
        GLfloat shininess[] = {80};
        
        if (meshIdx == currentObj) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
        }else {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
        }
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
        
        glMultMatrixf(transformations.at(meshIdx).data());
        renderMesh(rasterMeshes.at(meshIdx));
        
        glPopMatrix();
    }

    glutSwapBuffers(); // Flush the OpenGL buffers to the window

    if (shadeToggle) {
        if (shadeMode == SMOOTH) {
            shadeMode = FLAT;
            glShadeModel(GL_FLAT);
        } else {
            shadeMode = SMOOTH;
            glShadeModel(GL_SMOOTH);
        }
        shadeToggle = false;
    }
    if (fillToggle) {
        if (fillMode == FILLED) {
            fillMode = WIREFRAME;
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        } else {
            fillMode = FILLED;
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        fillToggle = false;
    }
}

/*
 +––––––––––––––––––––––––––––––––––––––+
 |Main OpenGL                           |
 +––––––––––––––––––––––––––––––––––––––+
 */
void renderToOpenGL(int argc, char *argv[]) {
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
    
    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events
    glutSpecialFunc(specialKeyPressed);
    
    createMenu();
    glutAttachMenu(GLUT_LEFT_BUTTON);
    
    glutMainLoop(); // Enter GLUT's main loop
}

/*
 +––––––––––––––––––––––––––––––––––––––+
 |New OpenGL                            |
 +––––––––––––––––––––––––––––––––––––––+
 Uses modern OpenGL 3.2+ pipleline and custom shading language to shade the teapot.
 Only supports a blob of color, no Phong shading or light supported.
  */
void renderToOpenGL3(vector<RasterMesh>& meshes) {
    // Shader sources
    const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec3 position;"
    "in vec3 normal;"
    "uniform mat4 trans;"
    "void main() {"
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
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    
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
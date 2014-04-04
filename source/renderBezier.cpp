//
//  renderBezier.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/3/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "renderBezier.h"

int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        return -1;
    }
    
    //Only version 3.2 and up
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //No resize
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Important, otherwise "glfCreateWindow" will return NULL
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    
    while(!glfwWindowShouldClose(window))
    {
        //render here
        
        glfwSwapBuffers(window);
        
        //poll and process events
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    glfwTerminate();
    return 0;
}
//
//  renderBezier.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/3/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "renderBezier.h"

#include <GLFW/glfw3.h>
#include <thread>

int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
//    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    
    glfwMakeContextCurrent(window);
    
    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    glfwTerminate();
    return 0;
}
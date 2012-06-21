﻿//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include <openme.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>


int main(void)
{
    int i;
    double t;
    omeBuffer *buffer;
    omeCamera *camera;
    omeVector pos = {200.f, 200.f, 200.f};
    omeVector target = {0.f, 0.f, 0.f};
    omeVector vec = {0, 0, 1};
    omeMesh *mesh;
    omeMesh *objMesh;
    omeMatrix matrix;
    float angle = 0;
    float vertices[][18] = {
        {-1, -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1}, 
        {-1, -1, 0, 1, -1, 0, 1, 1, 0, 1, 1, 0, -1, 1, 0, -1, -1, 0},
        {-1, -1, 1, 1, -1, 1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, -1, 1}};
    float vertices2[] = {-1, -1, 2, 1, -1, 2, -1, 1, 2, 1, 1, 2};
    unsigned char indices[] = {0, 1, 2, 1, 2, 3};
    omePolygonType polygonTypes[] = {OME_TRIANGLES, OME_LINE_LOOP, OME_POINTS};
    int width = 640;//1280;
    int height = 480;//800;

    // get OpenGL context
    if(!glfwInit() || !glfwOpenWindow(width, height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        return EXIT_FAILURE;

    omeEngineStart();

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 70.f, width / (float)height, 0.01f, 1000.f);

    // mesh test
    mesh = omeMeshCreate(4);

    // 3 squares, with different colors and polygon types
    for(i = 0; i < 3; i++)
    {
        buffer = omeMeshAddBuffer(mesh, 6, 3, polygonTypes[i]);
        omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, &vertices[i]);
        omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_COLOR, &vertices[i]);
        omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_NORMAL, &vertices[i]);
    }

    // 1 last square,  using indices
    buffer = omeMeshAddBuffer(mesh, 6, 2, OME_TRIANGLES);
    omeBufferUseIndices(buffer);
    omeBufferAddIndices(buffer, OME_UBYTE, 0, indices);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, vertices2);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_COLOR, vertices2);

    omeSaveOmeMeshToFile("data/plans.omeMesh", mesh);
    mesh = omeLoadOmeMeshFromFile("data/plans.omeMesh");

    // obj loading test    
    t = glfwGetTime();
    //objMesh = omeLoadOBJFromFile("data/bunny69k.obj", OME_TRUE);
    printf("obj loading time: %.6fs\n", glfwGetTime() - t);

    t = glfwGetTime();
    //omeSaveOmeMeshToFile("data/mesh1.omeMesh", objMesh);
    printf("omeMesh saving time: %.6fs\n", glfwGetTime() - t);

    t = glfwGetTime();
    objMesh = omeLoadOmeMeshFromFile("data/mesh1.omeMesh");
    printf("omeMesh loading time: %.6fs\n", glfwGetTime() - t);

    // deprecated stuff to test normals before using shaders
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos.tab);

    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // move camera
        omeCameraSetPosition(camera, &pos);
        omeCameraSetTarget(camera, &target);
        omeCameraUpdate(camera);

        // ugly rotation :D
        // TODO: use angles and sin, cos..
        angle += omeEngineGetFrameDuration() * 50;
        target.x += glfwGetKey(GLFW_KEY_LEFT) * omeEngineGetFrameDuration() * 50;
        target.x -= glfwGetKey(GLFW_KEY_RIGHT) * omeEngineGetFrameDuration() * 50;
        target.y += glfwGetKey(GLFW_KEY_UP) * omeEngineGetFrameDuration() * 50;
        target.y -= glfwGetKey(GLFW_KEY_DOWN) * omeEngineGetFrameDuration() * 50;
        target.z += glfwGetMouseWheel() * 10;
        glfwSetMouseWheel(0);

        // render
        omeMeshRender(mesh);
        omeMeshRender(objMesh);

        // TODO: limit fps here?
        omeEngineUpdate();
        glfwSleep(0.002);
        glfwSwapBuffers();
    }

    // free all the memory!!!
    omeMeshDestroy(&mesh);
    omeMeshDestroy(&objMesh);
    omeCameraDestroy(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}

//  ome: Open Minimalistic Engine
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
    omeBuffer *buffer;
    omeCamera *camera;
    omeVector pos = {10.f, 10.f, 10.f};
    omeVector vec = {0, 0, 1};
    omeMesh *mesh;
    omeMesh *objMesh;
    omeMatrix matrix;
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
    omeCameraSetPosition(camera, &pos);
    omeCameraUpdate(camera);

    // mesh test
    mesh = omeMeshCreate(4);

    // 3 squares, with different colors and polygon types
    for(i = 0; i < 3; i++)
    {
        buffer = omeMeshAddBuffer(mesh, 6, 2, polygonTypes[i]);
        omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, &vertices[i]);
        omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_COLOR, &vertices[i]);
    }

    // 1 last square,  using indices
    buffer = omeMeshAddBuffer(mesh, 6, 2, OME_TRIANGLES);
    omeBufferUseIndices(buffer);
    omeBufferAddIndices(buffer, OME_UBYTE, 0, indices);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, vertices2);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_COLOR, vertices2);

    //omeSaveOmeMeshToFile("data/plans.omeMesh", mesh);
    //mesh = omeLoadOmeMeshFromFile("data/plans.omeMesh");

    // obj loading test
    {
        double t;
        
        t = glfwGetTime();
        objMesh = omeLoadOBJFromFile("data/bunny69k.obj");
        printf("obj loading time: %.3fs\n", glfwGetTime() - t);

        t = glfwGetTime();
        omeSaveOmeMeshToFile("data/bunny69k.omeMesh", objMesh);
        printf("omeMesh saving time: %.3fs\n", glfwGetTime() - t);

        t = glfwGetTime();
        objMesh = omeLoadOmeMeshFromFile("data/bunny69k.omeMesh");
        printf("omeMesh loading time: %.3fs\n", glfwGetTime() - t);
    }

    // ugly rotation :D
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix.tab);
    omeMatrixTranspose(&matrix);
    omeMatrixRotateAxis(&matrix, &vec, 1);
    //omeMatrixLoad(&matrix, OME_TRUE);

    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ugly rotation :D
        glGetFloatv(GL_MODELVIEW_MATRIX, matrix.tab);
        omeMatrixTranspose(&matrix);
        omeMatrixRotateAxis(&matrix, &vec, 1);
        omeMatrixLoad(&matrix, OME_TRUE);

        omeMeshRender(mesh);
        glColor3f(1, 1, 1);
        omeMeshRender(objMesh);

        // TODO: limit fps here?
        omeEngineUpdate();
        glfwSleep(0.001);
        glfwSwapBuffers();
    }

    omeMeshDestroy(&mesh);
    omeMeshDestroy(&objMesh);
    omeCameraDestroy(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}

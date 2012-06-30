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
#include <IL/il.h>
#include <math.h>


// quick and dirty test of texture coordinates
void init_texture(wchar_t *fileName)
{
    GLuint id;
    ILuint il_id;
    int w, h;
    GLenum format;

    // init
    ilInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);

    // create image
    ilGenImages(1, &il_id);
    ilBindImage(il_id);

    if(ilLoadImage(fileName) == IL_FALSE)
    {
        ilDeleteImage(il_id);
        //printf("unable to load: %s\n", fileName);
        return;
    }

    if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_FALSE)
    {
        ilDeleteImage(il_id);
        //printf("Unable to convert: %s\n", fileName);
        return;
    }

    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    //format = ilGetInteger(IL_IMAGE_FORMAT);
    format = GL_RGBA;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // send pixels to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, GL_UNSIGNED_BYTE, ilGetData());

    // set minifying and magnifying filters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ilDeleteImage(il_id);
}


int main(void)
{
    omeCamera *camera;
    omeVector pos = 	{{2.f, 2.f, 2.f}};
    omeVector target = 	{{0.f, 0.f, 0.f}};
    omeMesh *mesh;
    int width = 640;//1280;
    int height = 480;//800;
    float theta = OME_PIF / 4;
    float phi = OME_PIF / 4;
    float angleStep;
    float zoom = 5;
    omeBool drag = OME_FALSE;
    omeProgram *shaderProgram;
    omeMaterial *mat;

    // get OpenGL context
    if(!glfwInit() || !glfwOpenWindow(width, height, 8, 8, 8, 8, 32, 0, GLFW_WINDOW))
        return EXIT_FAILURE;

    glfwSetMouseWheel(0);
    omeEngineStart();

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 70.f, width / (float)height, 0.01f, 1000.f);
    omeCameraSetTarget(camera, &target);
    omeEngineSetActiveCamera(camera);

    // obj loading test
    //mesh = omeLoadOBJFromFile("data/chamfer.obj", OME_TRUE);
    //omeMeshSave("data/mesh1.omeMesh", mesh);
    mesh = omeMeshLoad("data/mesh1.omeMesh");

    // deprecated stuff to test normals before using shaders
    //init_texture(L"data/test.png");

    // shader test
    shaderProgram = omeProgramCreate();
    omeProgramAddShader(shaderProgram, omeShaderCreate("data/basic.vs"));
    omeProgramAddShader(shaderProgram, omeShaderCreate("data/basic.ps"));
    omeProgramLink(shaderProgram);
    omeProgramUse(shaderProgram);
    mesh->program = shaderProgram;

    // material test
    mat = omeMaterialCreate();
    mesh->material = mat;

    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // move camera
        angleStep = 3 * omeEngineGetFrameDuration();

        // angles update
        if(glfwGetKey(GLFW_KEY_UP))
            phi += angleStep;
        else if(glfwGetKey(GLFW_KEY_DOWN))
            phi -= angleStep;
        if(glfwGetKey(GLFW_KEY_LEFT))
            theta -= angleStep;
        else if(glfwGetKey(GLFW_KEY_RIGHT))
            theta += angleStep;

        // angle update using mouse
        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
        {
            int x, y;
            static int lastX, lastY;

            glfwGetMousePos(&x, &y);

            if(!drag)
                drag = OME_TRUE;
            else
            {
                theta += (lastX - x) * 0.01f;
                phi += (y - lastY) * 0.01f;
            }

            lastX = x;
            lastY = y;
        }
        else
            drag = OME_FALSE;

        // avoid camera being upside down
        if(phi > (OME_PIF / 2.f - 0.01f))
            phi = OME_PIF / 2.f - 0.01f;

        if(phi < (-OME_PIF / 2.f + 0.01f))
            phi = -OME_PIF / 2.f + 0.01f;

        // zoom update
        zoom -= glfwGetMouseWheel();
        glfwSetMouseWheel(0);

        // avoid negative zoom
        if(zoom < 0.1f)
            zoom = 0.1f;

        pos.x = cos(phi) * cos(theta);
        pos.y = cos(phi) * sin(theta);
        pos.z = sin(phi);
        omeVectorMultScal(&pos, zoom, &pos);
        omeCameraSetPosition(camera, &pos);
        omeCameraUpdate(camera);

        // render
        omeEngineRender();

        // TODO: limit fps here?
        omeEngineUpdate();
        glfwSleep(0.002);
        glfwSwapBuffers();
    }

    // free all the memory!!!
    omeMeshDestroy(&mesh);
    omeCameraDestroy(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}

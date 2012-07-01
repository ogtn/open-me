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
omeTexture *loadTexture(const char *fileName)
{
    ILuint il_id;
    int w, h;
    omeTexture *t;

    // init
    ilInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);

    // create image
    ilGenImages(1, &il_id);
    ilBindImage(il_id);

    if(ilLoadImage((const ILstring)fileName) == IL_FALSE)
    {
        ilDeleteImage(il_id);
        //printf("unable to load: %s\n", fileName);
        return NULL;
    }

    if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_FALSE)
    {
        ilDeleteImage(il_id);
        //printf("Unable to convert: %s\n", fileName);
        return NULL;
    }

    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);

    t = omeTextureCreate2D(w, h, ilGetData());

    ilDeleteImage(il_id);

    return t;
}


int main(void)
{
    omeCamera *camera;
    omeVector pos;
    omeVector target = 	{{0.f, 0.f, 0.f}};
    omeMesh *mesh, *mesh2;
    int width = 640;
    int height = 480;
    float theta = OME_PIF / 4;
    float phi = OME_PIF / 4;
    float angleStep;
    float zoom = 15;
    omeBool drag = OME_FALSE;
    omeProgram *shaderProgram;
    omeMaterial *mat;
    omeRenderTarget *renderTarget;

    // get OpenGL context
    if(!glfwInit() || !glfwOpenWindow(width, height, 8, 8, 8, 8, 32, 0, GLFW_WINDOW))
        return EXIT_FAILURE;

    glfwSetMouseWheel(0);
    glfwSetWindowSizeCallback(omeEngineResize);
    omeEngineStart(width, height);

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 70.f, width / (float)height, 0.1f, 1000.f);
    omeCameraSetTarget(camera, &target);
    omeEngineSetActiveCamera(camera);

    // obj loading test
    //mesh = omeLoadOBJFromFile("data/chamfer.obj", OME_TRUE);
    //omeMeshSave("data/mesh1.omeMesh", mesh);
    //mesh = omeMeshLoad("data/mesh1.omeMesh");
    //mesh2 = omeMeshLoad("data/mesh1.omeMesh");
    
    // primitives test
    mesh = omePrimitiveCube(10, 1);
    mesh->entity.position.z = -4;
    mesh2 = omePrimitiveSphere(8, 16);

    // shader test
    shaderProgram = omeProgramCreate();
    omeProgramAddShader(shaderProgram, omeShaderCreate("data/basic.vs"));
    omeProgramAddShader(shaderProgram, omeShaderCreate("data/basic.ps"));
    omeProgramLink(shaderProgram);
    omeProgramUse(shaderProgram);
    mesh->program = shaderProgram;
    mesh2->program = shaderProgram;

    // material test
    mat = omeMaterialCreate();
    mat->ambiantColor.r = 0.f;
    mesh->material = mat;
    mat = omeMaterialCreate();
    mat->ambiantColor.b = 1.f;
    mesh2->material = mat;

    // render target test
    renderTarget = omeRenderTargetCreate(256, 256);

    // texture test
    mesh->material->diffuseTexture = renderTarget->colorBuffer;
    mesh2->material->diffuseTexture = loadTexture("data/lena.jpg");

    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        // move camera
        angleStep = 3 * omeEngineGetFrameDuration();

        // angles update
        if(glfwGetKey(GLFW_KEY_UP))
            mesh->entity.position.z += omeEngineGetFrameDuration() * 20;
        else if(glfwGetKey(GLFW_KEY_DOWN))
            mesh->entity.position.z -= omeEngineGetFrameDuration() * 20;
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

        // render
        omeEngineUpdate(); // TODO: limit fps here?
        omeEngineRender(renderTarget);
        omeEngineRender(NULL);

        glfwSleep(0.002);
        glfwSwapBuffers();
    }

    // free all the memory!!!
    omeMeshDestroy(&mesh);
    omeCameraDestroy(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}

//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include <openme.h>
#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <unistd.h>

static void testString(omeMesh *m)
{
    int i, j;
    omeGeometry *g;
    omeString *str = omeStringCreateFromStr("#version 120\n\n");

    omeStringAppendStr(str, "// input\n");

    // attributes
    for(i = 0; i < m->nbBuffers; i++)
    {
        g = m->geometries[i];

        for(j = 0; j < OME_ATTRIB_MAX; j++)
            if(g->attributes[j].valid)
                omeStringAppendStr(str, "attribute %s %s;\n", g->attributes[j].glslType, g->attributes[j].name);
    }

    omeStringAppendStr(str, "void main(void)\n{\n");
    omeStringAppendStr(str, "}\n");
    
    puts(omeStringGetStr(str));

    omeStringDestroy(&str);
}

void scrollCallback(GLFWwindow *window, double x, double y)
{
	float *zoom = glfwGetWindowUserPointer(window);

    *zoom += x;
}

void resizeCallback(GLFWwindow *window, int x, int y)
{
   omeEngineResize(x, y);
}

void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error occured : %s\n", description);
}

GLFWwindow *createContext(int width, int height)
{
	GLFWwindow *window;
    const GLFWvidmode *videoMode;

    if(!glfwInit())
    {
        fprintf(stderr, "Holy fuckin' shit, glfwInit() failed\n");
        return NULL;
    }

    glfwSetErrorCallback(errorCallback);

    // configure context
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    //glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);

    window = glfwCreateWindow(width, height, "Hello world", NULL, NULL);

    // create window and OpenGL context
    if(!window)
    {
        fprintf(stderr, "Holy fuckin' shit, glfwCreateWindow() failed\n");
        return NULL;
    }

    // center the window
    videoMode = glfwGetVideoMode(NULL);
    glfwSetWindowPos(window, (videoMode->width - width) / 2, (videoMode->height - height) / 2);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetScrollCallback(window, scrollCallback);

    return window;
}


int main(void)
{
	GLFWwindow *window;
    omeCamera *camera;
    omeVector pos;
    omeVector target = 	{{0.f, 0.f, 0.f}};
    omeMesh *mesh = NULL;
    omeMesh *mesh2 = NULL;
    omeMesh *pickedMesh = NULL;
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
    omeBool picked = OME_FALSE;
    int maxFrameCpt = 0;


    window = createContext(width, height);

    if(window == NULL)
        return EXIT_FAILURE;

    glfwSetWindowUserPointer(window, &zoom);
    omeEngineStart(width, height);

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 70.f, width / (float)height, 0.1f, 1000.f);
    omeCameraSetTarget(camera, &target);
    omeEngineSetActiveCamera(camera);

    /*
    // obj loading test
    mesh = omeLoadOBJFromFile("data/bunny69k.obj", OME_TRUE);
    omeMeshSave("data/mesh1.omeMesh", mesh);
    omeMeshDestroy(&mesh);

    mesh = omeMeshLoad("data/mesh1.omeMesh");
    mesh2 = omeMeshLoad("data/mesh1.omeMesh");

    omeMeshDestroy(&mesh);
    omeMeshDestroy(&mesh2);
    */
    
    // primitives test
    omePrimitiveGrid(16, 4);

    /*
    mesh = omePrimitiveSphere(10, 4);
    mesh2 = omePrimitiveSphere(8, 16);

    // shader test
    shaderProgram = omeProgramCreate();
    omeProgramAddShader(shaderProgram, omeShaderLoadFromFile("data/basic.vs"));
    omeProgramAddShader(shaderProgram, omeShaderLoadFromFile("data/basic.ps"));
    omeProgramLink(shaderProgram);
    mesh->program = shaderProgram;

    shaderProgram = omeProgramCreate();
    omeProgramAddShader(shaderProgram, omeShaderLoadFromFile("data/cubemap.vs"));
    omeProgramAddShader(shaderProgram, omeShaderLoadFromFile("data/cubemap.ps"));
    omeProgramLink(shaderProgram);
    mesh2->program = shaderProgram;

    // material test
    mat = omeMaterialCreate();
    mat->ambiantColor.r = 0.f;
    mesh->material = mat;
    mat = omeMaterialCreate();
    mat->ambiantColor.b = 1.f;
    mesh2->material = mat;

    // render target test
    renderTarget = omeRenderTargetCreate(128, 128);

    // texture test
    mesh->material->diffuseTexture = omeTextureLoadFromFile("data/lena.jpg");//renderTarget->colorBuffer;
    mesh2->material->diffuseTexture = omeTextureCubeMapLoadFromFile("data/cloudy.omeCubeMap");
    mesh->entity.position.z += 10;*/
    // testString(mesh);

    while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        // move camera
        angleStep = 3 * (float)omeEngineGetFrameDuration();

        // moving selected mesh
        if(pickedMesh)
        {
            float step = (float)omeEngineGetFrameDuration() * 10;
            omeVector *v = &pickedMesh->entity.position;

            if(glfwGetKey(window, GLFW_KEY_UP))
                v->y += step;
            else if(glfwGetKey(window, GLFW_KEY_DOWN))
                v->y -= step;
            if(glfwGetKey(window, GLFW_KEY_LEFT))
                v->x -= step;
            else if(glfwGetKey(window, GLFW_KEY_RIGHT))
                v->x += step;

            v = &pickedMesh->entity.rotation;

            if(glfwGetKey(window, 'A'))
                v->x += step * 5;
            if(glfwGetKey(window, 'Z'))
                v->y += step * 5;
            if(glfwGetKey(window, 'E'))
                v->z += step * 5;

            v = &pickedMesh->entity.scaling;

            if(glfwGetKey(window, 'Q'))
                v->x += step * 0.1F;
            if(glfwGetKey(window, 'S'))
                v->y += step * 0.1F;
            if(glfwGetKey(window, 'D'))
                v->z += step * 0.1F;

            // reset mesh
            if(glfwGetKey(window, GLFW_KEY_SPACE))
            {
                v = &pickedMesh->entity.position;
                v->x = v->y = v->z = 0;

                v = &pickedMesh->entity.rotation;
                v->x = v->y = v->z = 0;

                v = &pickedMesh->entity.scaling;
                v->x = v->y = v->z = 1;
            }
        }

        // angle update using mouse
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        {
            double x, y;
            static double lastX, lastY;

            glfwGetCursorPos(window, &x, &y);

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

        // avoid negative zoom
        if(zoom < 0.1f)
            zoom = 0.1f;

        pos.x = cosf(phi) * cosf(theta);
        pos.y = cosf(phi) * sinf(theta);
        pos.z = sinf(phi);
        omeVectorMultScal(&pos, zoom, &pos);
        omeCameraSetPosition(camera, &pos);

        // picking test
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
        {
            if(!picked)
            {
            	double xd, yd;
                int x, y;

                picked = OME_TRUE;
                glfwGetCursorPos(window, &xd, &yd);
                x = floor(xd);
                y = floor(yd);
                glfwGetWindowSize(window, &width, &height);

                pickedMesh = omeEnginePick(x, height - y);
                
                if(pickedMesh)
                    printf("%s picked!\n", pickedMesh->entity.name);
                else
                    printf("nothing picked!\n");
            }
        }
        else
            picked = OME_FALSE;

        // render
        omeEngineUpdate(); // TODO: limit fps here?
        //omeEngineRender(renderTarget);
        omeEngineRender(NULL);

        usleep(2000);
        glfwSwapBuffers(window);
    }

    // free all the memory!!!
    omeMeshDestroy(&mesh);
    omeMeshDestroy(&mesh2);
    omeCameraDestroy(&camera);
    omeProgramDestroy(&shaderProgram);
    omeEngineStop();

    return EXIT_SUCCESS;
}


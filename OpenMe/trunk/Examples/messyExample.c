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

#ifdef _MSC_VER
#include <windows.h>
void usleep(__int64 usec);
#else
#include <unistd.h>
#endif


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

    *zoom -= y;
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
	GLFWmonitor *monitor;

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
	monitor = glfwGetPrimaryMonitor();
    videoMode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (videoMode->width - width) / 2, (videoMode->height - height) / 2);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
	glfwMakeContextCurrent(window);

    return window;
}


int main(void)
{
	GLFWwindow *window;
    omeCamera *camera = NULL;
    omeVector pos;
    omeVector target = 	{{0.f, 0.f, 0.f}};
	omeMesh *grid = NULL, *sphere = NULL, *cube = NULL, *bunny = NULL;
    omeMesh *pickedMesh = NULL;
    int width = 640;
    int height = 480;
    float theta = OME_PIF / 4;
    float phi = OME_PIF / 4;
    float angleStep;
    float zoom = 15;
    omeBool drag = OME_FALSE;
	omeProgram *gridProgram = NULL, *cubeMapProgram = NULL, *textureProgram = NULL;
    omeMaterial *textureMat = NULL, *cubeMat = NULL;
    omeRenderTarget *renderTarget = NULL;
    omeBool picked = OME_FALSE;
    int maxFrameCpt = 0;


    window = createContext(width, height);

    if(window == NULL)
        return EXIT_FAILURE;

    glfwSetWindowUserPointer(window, &zoom);
    
	if (omeEngineStart(width, height) != OME_SUCCESS)
		return EXIT_FAILURE;

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 70.f, width / (float)height, 0.1f, 1000.f);
    omeCameraSetTarget(camera, &target);
    omeEngineSetActiveCamera(camera);

	// shader test
	textureProgram = omeProgramCreate();
	omeProgramAddShader(textureProgram, omeShaderLoadFromFile("data/basic.vs"));
	omeProgramAddShader(textureProgram, omeShaderLoadFromFile("data/basic.ps"));
	omeProgramLink(textureProgram);
	
	cubeMapProgram = omeProgramCreate();
	omeProgramAddShader(cubeMapProgram, omeShaderLoadFromFile("data/cubemap.vs"));
	omeProgramAddShader(cubeMapProgram, omeShaderLoadFromFile("data/cubemap.ps"));
	omeProgramLink(cubeMapProgram);
	
	// material tests
	textureMat = omeMaterialCreate();
	textureMat->ambiantColor.r = 0.f;
	textureMat->diffuseTexture = omeTextureLoadFromFile("data/testing.png");

	cubeMat = omeMaterialCreate();
	cubeMat->ambiantColor.r = 0.f;
	cubeMat->diffuseTexture = omeTextureCubeMapLoadFromFile("data/cloudy.omeCubeMap");

	// primitives tests
	grid = omePrimitiveGrid(16, 4);

	//sphere = omePrimitiveSphere(10, 4);
	//sphere->program = cubeMapProgram;
	//sphere->material = cubeMat;

	//cube = omePrimitiveCube(5, 4);
	//cube->program = cubeMapProgram;
	//cube->material = cubeMat;
    
    // obj loading test
    bunny = omeLoadOBJFromFile("data/bunny69k.obj", OME_TRUE);
    //omeMeshSave("data/mesh1.omeMesh", bunny);
    //omeMeshDestroy(&bunny);
    //bunny = omeMeshLoad("data/mesh1.omeMesh");
	bunny->program = cubeMapProgram;
	bunny->material = cubeMat;

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
		glfwWaitEvents();
    }

    // free all the memory!!!
    omeEngineStop();

    return EXIT_SUCCESS;
}

#ifdef _MSC_VER
// https://www.c-plusplus.net/forum/topic/109539/usleep-unter-windows
void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
#endif

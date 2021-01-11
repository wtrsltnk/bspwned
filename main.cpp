/* 
 * File:   main.cpp
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:29 PM
 */

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include "camera.h"
#include "worldloader.h"
#include "math3d.h"
#include "resources.h"

using namespace std;

Camera* pCam = NULL;
void* odeSphere = NULL;

void keyPressed(int key, int action)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
    }
}

int main(int argc, char** argv)
{
    Camera cam;
    WorldRenderer renderer;
    #ifdef WIN32
    ResourceManager resources("D:\\Games\\Counter-Strike1.1");
    #else
    ResourceManager resources("/media/data/Games/Half-Life");
    #endif
    WorldLoader::Config config;
    config.resourceManager = &resources;
    WorldLoader loader(config);
    int w = 1024, h = 768;
    float plane[] = { 0, 1, 0, 0 };

    pCam = &cam;

    try
    {
        if (!loader.loadBSP(argv[1], renderer))
        {
            cout << loader.getLastError() << endl;
            return 1;
        }
    }
    catch (const char* err)
    {
        cout << err << endl;
    }
    
    int    ok;             // Flag telling if the window was opened
    int    running;        // Flag telling if the program is running

    float rot[3] = { 0 };
    
    glfwInit();
    ok = glfwOpenWindow(
          w, h,              // Width and height of window
          8, 8, 8,           // Number of red, green, and blue bits for color buffer
          8,                 // Number of bits for alpha buffer
          24,                // Number of bits for depth buffer (Z-buffer)
          0,                 // Number of bits for stencil buffer
          GLFW_WINDOW        // We want a desktop window (could be GLFW_FULLSCREEN)
    );

    // If we could not open a window, exit now
    if( !ok )
    {
      glfwTerminate();
      return 0;
    }
    
    // Set window title
    glfwSetWindowTitle( "Bspwned" );
    glfwSetKeyCallback(keyPressed);

    // Enable sticky keys
    glfwEnable( GLFW_STICKY_KEYS );
    glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0f, 1, 0.1f, 10000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    loader.setupWorld(renderer);

    double time = glfwGetTime();
    double prevTime = time;
    int fps = 0;
    float speed = 300.0f;
    
    GLUquadricObj* glSphere = gluNewQuadric();
    float p[3] = { 0, 100, -50 };
    float f[3] = { 0 };
    float a[3] = { 0 };
    
    // Main rendering loop
    do
    {
        fps++;
        double newTime = glfwGetTime();
        double timeDiff = newTime - prevTime;
        prevTime = newTime;
        if ((newTime - time) > 1)
        {
            double realFps = double(fps) / (newTime - time);
            char str[32];
            sprintf(str, "FPS: %f", realFps);
            glfwSetWindowTitle(str);
            fps = 0;
            time = newTime;
        }

        if (glfwGetKey('W'))
            cam.MoveLocal(speed * timeDiff, 0, 0);
        if (glfwGetKey('S'))
            cam.MoveLocal(-speed * timeDiff, 0, 0);

        if (glfwGetKey('A'))
            cam.MoveLocal(0, speed * timeDiff, 0);
        if (glfwGetKey('D'))
            cam.MoveLocal(0, -speed * timeDiff, 0);

        int mx = 0, my = h / 2;
        glfwGetMousePos(&mx, &my);

        // Now the rotatins do not depent on the speed of rendering
        rot[2] = ((float(mx) / w) * M_PI - M_PI_2) * 4.0f;
        rot[0] = -90.0f + (float(my) / h) * M_PI - M_PI_2;

        cam.SetRotation(rot);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.Update();

        Vector3 cameraPosition(cam.getPosition());
        cameraPosition *= -1;
        cameraPosition.copyTo(renderer.mConfig.mViewPoint);
        renderer.render(cameraPosition);

        // Swap front and back buffers (we use a double buffered display)
        glfwSwapBuffers();

        // Check if the escape key was pressed, or if the window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    }
    while( running );

    glfwTerminate();

    return (EXIT_SUCCESS);
}

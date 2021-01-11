/* 
 * File:   main.cpp
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:29 PM
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <string.h>

#include "opengl.h"

#include "camera.h"
#include "math3d.h"
#include "resources.h"
#include "worldloader.h"

using namespace std;

Camera *pCam = NULL;
void *odeSphere = NULL;

void glhFrustumf2(
    float *matrix,
    float left,
    float right,
    float bottom,
    float top,
    float znear,
    float zfar)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0] = temp / temp2;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    matrix[4] = 0.0f;
    matrix[5] = temp / temp3;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    matrix[8] = (right + left) / temp2;
    matrix[9] = (top + bottom) / temp3;
    matrix[10] = (-zfar - znear) / temp4;
    matrix[11] = -1.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = (-temp * zfar) / temp4;
    matrix[15] = 0.0f;
}

// Matrix will receive the calculated perspective matrix.
// You would have to upload to your shader
// or use glLoadMatrixf if you aren't using shaders.
void glhPerspectivef2(
    float *matrix,
    float fovyInDegrees,
    float aspectRatio,
    float znear,
    float zfar)
{
    float ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);

    float xmax = ymax * aspectRatio;

    glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

std::string FindRootFromFilePath(
    const std::string &filePath /*,
    std::string &mod*/
)
{
    auto path = std::filesystem::path(filePath);

    if (!path.has_parent_path())
    {
        spdlog::error("given path ({}) has no parent path", filePath);

        return "";
    }

    path = path.parent_path();

    auto fn = path.filename();
    if (path.has_parent_path() && (fn == "maps" || fn == "models" || fn == "sprites" || fn == "sound" || fn == "gfx" || fn == "env"))
    {
        path = path.parent_path();
    }

    auto lastDirectory = path.filename().generic_string();

    do
    {
        for (auto &p : std::filesystem::directory_iterator(path))
        {
            if (p.is_directory())
            {
                continue;
            }

            if (p.path().filename() == "hl.exe" && p.path().has_parent_path())
            {
                /*mod = lastDirectory;*/
                return p.path().parent_path().generic_string();
            }
        }

        lastDirectory = path.filename().generic_string();
        path = path.parent_path();

    } while (path.has_parent_path() && path.parent_path() != path);

    return "";
}

int main(
    int argc,
    char **argv)
{
    if (argc < 2)
    {
        spdlog::error("usage incorrect, pass the path to a bsp file plz.");

        return 0;
    }

    Camera cam;
    WorldRenderer renderer;
    ResourceManager resources(FindRootFromFilePath(argv[0]));
    WorldLoader::Config config;
    config.resourceManager = &resources;
    WorldLoader loader(config);
    int w = 1024, h = 768;

    pCam = &cam;

    try
    {
        if (!loader.loadBSP(argv[1], renderer))
        {
            spdlog::error(loader.getLastError());

            return 1;
        }

        spdlog::info("{} loaded", argv[1]);
    }
    catch (const char *err)
    {
        spdlog::error("{}", err);

        return 0;
    }

    int ok;             // Flag telling if the window was opened
    int running = true; // Flag telling if the program is running

    float rot[3] = {0};

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    auto window = glfwCreateWindow(
        w, h,
        "Bspwned",
        nullptr,
        nullptr);

    // If we could not open a window, exit now
    if (!window)
    {
        glfwTerminate();

        return 0;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    // Enable sticky keys
    glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float matrix[16];
    glhPerspectivef2(matrix, 90.0f, 1, 0.1f, 10000.0f);
    glLoadMatrixf(matrix);

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

    // Main rendering loop
    while (running)
    {
        glfwPollEvents();

        fps++;
        double newTime = glfwGetTime();
        double timeDiff = newTime - prevTime;
        prevTime = newTime;
        if ((newTime - time) > 1)
        {
            double realFps = double(fps) / (newTime - time);

            fps = 0;
            time = newTime;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            cam.MoveLocal(float(speed * timeDiff), 0, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            cam.MoveLocal(float(-speed * timeDiff), 0, 0);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            cam.MoveLocal(0, float(speed * timeDiff), 0);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            cam.MoveLocal(0, float(-speed * timeDiff), 0);
        }

        double mx = 0, my = h / 2;
        glfwGetCursorPos(window, &mx, &my);

        // Now the rotatins do not depent on the speed of rendering
        rot[2] = float(((mx / w) * M_PI - M_PI_2) * 4.0);
        rot[0] = float(-90.0 + (my / h) * M_PI - M_PI_2);

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
        glfwSwapBuffers(window);

        // Check if the escape key was pressed, or if the window was closed
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE) && glfwGetWindowAttrib(window, GLFW_VISIBLE);
    }

    glfwTerminate();

    return (EXIT_SUCCESS);
}

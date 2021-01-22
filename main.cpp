/* 
 * File:   main.cpp
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:29 PM
 */

#include "opengl.h"

#include "camera.h"
#include "resources.h"
#include "stb_truetype.h"
#include "worldloader.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <glm/glm.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <string.h>

int running = true; // Flag telling if the program is running

Camera *pCam = nullptr;

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

static float fontSize = 16.0f;
unsigned char ttf_buffer[1 << 20];
unsigned char temp_bitmap[512 * 512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

void my_stbtt_initfont()
{
    FILE *f = nullptr;

    errno_t err = fopen_s(&f, "c:/windows/fonts/SourceCodePro-Light.ttf", "rb");
    if (err)
    {
        err = fopen_s(&f, "c:/windows/fonts/consola.ttf", "rb");
    }

    if (err)
    {
        spdlog::error("loading font failed");

        return;
    }

    fread(ttf_buffer, 1, 1 << 20, f);
    stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
    // can free ttf_buffer at this point
    glActiveTextureARB(GL_TEXTURE0);
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    fclose(f);
}

float my_stbtt_print_width(
    const std::string &text)
{
    const char *txt = text.c_str();

    float x = 0;
    float y = 0;

    while (*txt)
    {
        if (*txt < 32)
        {
            break;
        }

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, 512, 512, *txt - 32, &x, &y, &q, 1);

        ++txt;
    }

    return x;
}

void my_stbtt_print(
    float x,
    float y,
    const std::string &text)
{
    const char *txt = text.c_str();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);

    glActiveTextureARB(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ftex);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    while (*txt)
    {
        if (*txt < 32)
        {
            break;
        }

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, 512, 512, *txt - 32, &x, &y, &q, 1);
        glTexCoord2f(q.s0, q.t0);
        glVertex2f(q.x0, q.y0);

        glTexCoord2f(q.s1, q.t0);
        glVertex2f(q.x1, q.y0);

        glTexCoord2f(q.s1, q.t1);
        glVertex2f(q.x1, q.y1);

        glTexCoord2f(q.s0, q.t1);
        glVertex2f(q.x0, q.y1);

        ++txt;
    }
    glEnd();
}

class Console
{
public:
    Console();

    void Render(
        float widt);

    void AddChar(
        unsigned int c);

    void Backspace();

    void Enter();

    void ToggleOpen();

    bool IsOpen() const { return _isOpen; }

private:
    std::vector<std::string> _lines;
    std::string _input;
    bool _isOpen = true;
    float _currentPosition = 0;
    const float _openPosition = 0;
    const float _closePosition = -400;
};

Console::Console()
{
    _lines.push_back("hit ` to toggle this console");
}

void Console::Render(
    float width)
{
    if (_isOpen)
    {
        if (_currentPosition < _openPosition)
        {
            _currentPosition += 20.0f;
        }
    }
    else
    {
        if (_currentPosition > _closePosition)
        {
            _currentPosition -= 20.0f;
        }
    }

    glTranslatef(0, _currentPosition, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);

    glActiveTextureARB(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glColor4f(0.3f, 0.3f, 0.3f, 0.7f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(width, 0.0f);
    glVertex2f(width, 400.0f);
    glVertex2f(0.0f, 400.0f);
    glEnd();

    float y = 390;

    my_stbtt_print(10, y, fmt::format("] {}_", _input));
    y -= fontSize * 1.2f;

    for (auto line = _lines.rbegin(); line != _lines.rend(); line++)
    {
        my_stbtt_print(10, y, *line);
        y -= fontSize * 1.2f;
    }
}

void Console::AddChar(
    unsigned int c)
{
    if (c == '`')
    {
        return;
    }

    if (!_isOpen)
    {
        return;
    }

    _input += c;
}

void Console::Backspace()
{
    if (_input.empty())
    {
        return;
    }

    _input = _input.substr(0, _input.size() - 1);
}

void Console::Enter()
{
    if (_input.empty())
    {
        return;
    }

    if (_input == "exit" || _input == "quit")
    {
        _lines.push_back("bye!");
        running = false;
        _input = "";
        return;
    }

    _lines.push_back(_input);
    _input = "";
}

void Console::ToggleOpen()
{
    _isOpen = !_isOpen;
}

static Console console;

void CharCallback(
    GLFWwindow *window,
    unsigned int codepoint)
{
    (void)window;

    console.AddChar(codepoint);
}

void KeyCallback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mods)
{
    (void)window;
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_GRAVE_ACCENT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        console.ToggleOpen();
    }
    else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        console.Backspace();
    }
    else if (key == GLFW_KEY_ENTER && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        console.Enter();
    }
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
        running = console.IsOpen();
        console.ToggleOpen();
    }
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
    ShaderManager shaderManager;
    WorldRenderer renderer(shaderManager);
    std::string mod;
    ResourceManager resources(ResourceManager::FindRootFromFilePath(argv[1], mod), mod);
    WorldLoader::Config config;
    config.resourceManager = &resources;
    WorldLoader loader(config);
    int w = 1024, h = 768;

    pCam = &cam;

    try
    {
        if (!loader.loadBSP(argv[1], renderer))
        {
            return 1;
        }

        spdlog::info("{} loaded", argv[1]);
    }
    catch (const char *err)
    {
        spdlog::error("{}", err);

        return 0;
    }

    glm::vec3 start = renderer.getPlayerStart();
    start *= -1;
    cam.SetPosition(start);

    glm::vec3 rot = glm::vec3(0.0f);

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCharCallback(window, CharCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    my_stbtt_initfont();

    glClearColor(1.0f, 0.1f, 0.3f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float matrix[16];
    glhPerspectivef2(matrix, 90.0f, 1, 0.1f, 10000.0f);
    glLoadMatrixf(matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    loader.setupWorld(renderer);

    double time = glfwGetTime();
    double prevTime = time;
    int fps = 0;
    float speed = 300.0f;
    double realFps = 0;

    bool closeConsoleFirstTime = true;

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
            if (closeConsoleFirstTime)
            {
                console.ToggleOpen();
                closeConsoleFirstTime = false;
            }
            realFps = double(fps) / (newTime - time);

            fps = 0;
            time = newTime;
        }

        if (!console.IsOpen())
        {
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
        }

        double mx = 0, my = h / 2;
        glfwGetCursorPos(window, &mx, &my);

        // Now the rotatins do not depent on the speed of rendering
        rot[2] = float(((mx / w) * M_PI - M_PI_2) * 4.0);
        rot[0] = float(-90.0 + (my / h) * M_PI - M_PI_2);

        cam.SetRotation(rot);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.Update();

        glm::vec3 cameraPosition = cam.getPosition();
        cameraPosition *= -1;
        renderer._config._viewPoint = cameraPosition;
        renderer.render(cameraPosition);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glOrtho(0, w, h, 0, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        console.Render(w);

        glLoadIdentity();
        auto fpsstr = fmt::format("fps: {:.2f}", realFps);

        my_stbtt_print(w - my_stbtt_print_width(fpsstr) - (fontSize * 0.4f), fontSize * 1.2f, fpsstr);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // Swap front and back buffers (we use a double buffered display)
        glfwSwapBuffers(window);

        // Check if the escape key was pressed, or if the window was closed
        if (!glfwGetWindowAttrib(window, GLFW_VISIBLE))
        {
            running = false;
        }
    }

    glfwTerminate();

    return (EXIT_SUCCESS);
}

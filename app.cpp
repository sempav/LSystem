#include "app.h"

#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <glm/glm.hpp>

const int TREES_CNT = 5;

// these are defined in main.cpp
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

App::App() : running(false), window(NULL), width(0), height(0), ratio(0.0), 
             camera(), painter(nullptr), sc(nullptr), trees()
{
}

App::~App()
{
    OnCleanup();
}

bool App::OnInit(int argc, char *argv[])
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return false;
    }

    window = glfwCreateWindow(640, 480, "Complicated example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    ratio = width / static_cast<float>(height);

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file> <str_file>" << std::endl;
        return -1;
    }

    try {
        std::unique_ptr<StringControl> sc(new StringControl(argv[2]));
        painter = std::make_unique<TreePainter>(argv[1]);
        trees.push_back(std::make_pair(sc->GetResult(), glm::vec3(0.0f)));
        for (int i = TREES_CNT - 1; i >= 0; --i) {
            trees.push_back(std::make_pair(sc->GetResult(i),
                            glm::vec3(0.0f, 0.0f, TREES_CNT - i + 0.0f)));
        }
    } catch(const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "TreeBuilder initialization failed\n";
        return -1;
    }

    running = true;
    return true;
}

void App::OnEvent()
{
}

void App::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
    ratio = width / static_cast<float>(height);
}

void App::OnKey(int key, int scancode, int action, int mode)
{
    static const float ROTATION_ANGLE = 2.0f;
    static const float TRANSLATION_DIST = 0.05f;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_W && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.RotateX(ROTATION_ANGLE);
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.RotateX(-ROTATION_ANGLE);
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.RotateY(ROTATION_ANGLE);
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.RotateY(-ROTATION_ANGLE);
    }
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.Translate(TRANSLATION_DIST);
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS ||
                              action == GLFW_REPEAT)) {
        camera.Translate(-TRANSLATION_DIST);
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && (action == GLFW_PRESS ||
                                                   action == GLFW_REPEAT)) {
        if (key - GLFW_KEY_0 < static_cast<int>(trees.size())) {
            camera.SetPos(trees[key - GLFW_KEY_0].second);
        }
        //camera.SetPos(glm::vec3(0.0f, 0.0f, 0.0f - key + GLFW_KEY_0));
    }
}

void App::OnCursorPos(double xpos, double ypos)
{
    static const float ROT_COEFF = 0.1f;
    static double old_xpos = 0.5;
    static double old_ypos = 0.5;
    camera.RotateX(ROT_COEFF * (old_ypos - ypos));
    camera.RotateY(ROT_COEFF * (old_xpos - xpos));
    old_xpos = xpos;
    old_ypos = ypos;
}

void App::OnLoop()
{
}

void App::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, ratio, 0.01f, 1000.0f);
    camera.LookFrom();
    //glTranslatef(0.0f, 0.0f, -1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_LINES);
      glColor3f(1.f, 0.f, 0.f);
      glVertex3f(0.f, 0.f, 0.f);
      glVertex3f(1.f, 0.f, 0.f);
      glColor3f(0.f, 1.f, 0.f);
      glVertex3f(0.f, 0.f, 0.f);
      glVertex3f(0.f, 1.f, 0.f);
      glColor3f(0.f, 0.f, 1.f);
      glVertex3f(0.f, 0.f, 0.f);
      glVertex3f(0.f, 0.f, 1.f);
    glEnd();

    glm::vec3 pos(0.0f);
    glm::vec3 size;
    static bool first_time = true;
    for (auto& p: trees) {
        p.second = pos;
        painter->Paint(p.first, size, pos);
        pos.z += size.z * 0.5 + 0.1f;
        if (first_time) {
            camera.Translate(glm::max(size.x, glm::max(size.y, size.z)) - 1.0f);
            first_time = false;
        }
    }
    glfwSwapBuffers(window);

    static float t = 0;
    float elapsed_time = glfwGetTime();
    float last_render_time = elapsed_time - t;
    t = elapsed_time;
    char s[50];
    sprintf(s, "%.2f fps (%.4f sec/frame)",
                           1.0 / last_render_time,
                           last_render_time);
    glfwSetWindowTitle(window, s);
}

void App::OnCleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
    running = false;
}

int App::OnExecute(int argc, char *argv[])
{
	if (!OnInit(argc, argv)) {
        return -1;
    }

	while (running) {
        glfwPollEvents(); // do callbacks execute in this thread?
		OnLoop();
		OnRender();
        usleep(5);
        if (glfwWindowShouldClose(window)) {
            running = false;
        }
	}

	return 0;
}


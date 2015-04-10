#ifndef __APP_H__
#define __APP_H__

#include "camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "string_control.h"
#include "tree_painter.h"

class App {
	private:
		bool running;

        GLFWwindow *window;
        int width, height;
        float ratio;

        Camera camera;
        std::unique_ptr<TreePainter> painter;
        std::unique_ptr<StringControl> sc;
        std::vector<std::pair<std::string, glm::vec3>> trees;

		bool OnInit(int argc, char *argv[]);
		void OnEvent();
        void OnResize(int width, int height);
        void OnKey(int key, int scancode, int action, int mode);
        void OnCursorPos(double xpos, double ypos);
		void OnLoop();
		void OnRender();
		void OnCleanup();

        App(const App&);
        App &operator= (const App&);

	public:
		App();
        ~App();
		int OnExecute(int argc, char *argv[]);


    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif //__APP_H__

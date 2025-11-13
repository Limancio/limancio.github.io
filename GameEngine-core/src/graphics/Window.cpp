#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "..\..\src\vendor\imgui\imgui.h"
#include "..\..\src\vendor\imgui\imgui_impl_glfw.h"
#include "..\..\src\vendor\imgui\imgui_impl_opengl3.h"

float Window::m_ElapsedTime = 0.0f;

void window_resize(GLFWwindow *window, int width, int height);

	Window::Window(const char * title, int width, int height)
	{
		m_Title = title;
		m_Width = width;
		m_Height = height;
		if (!init()) 
			glfwTerminate();

		for (int i = 0; i < MAX_KEYS; i++) {
			m_Keys[i] = false;
		}

		for (int i = 0; i < MAX_BUTTONS; i++) {
			m_MouseButtons[i] = false;
		}
	}

	Window::~Window() {
		glfwTerminate();
	}

	bool Window::isKeyPressed(unsigned int keyCode) const
	{
		//TODO: Log Error
		if (keyCode >= MAX_KEYS)
			return false;
		return m_Keys[keyCode];

	}

	bool Window::isMouseButtonPressed(unsigned int button) const
	{
		//TODO: Log Error
		if (button >= MAX_BUTTONS)
			return false;
		return m_MouseButtons[button];
	}

	glm::vec2 Window::getMousePosition() const
	{
		return glm::vec2(mx, my);
	}

	bool Window::init() {

		if (!glfwInit())
		{
			std::cout << "Failed to initialize GLFW. " << std::endl;
			return false;
		}

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
		if (!m_Window) {
			std::cout << "Failed to create GLFW window. " << std::endl;
			return false;
		}
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, this);

		//INITIALIZATION CALLBACKS
		glfwSetWindowSizeCallback(m_Window, window_resize);
		glfwSetKeyCallback(m_Window, key_callback);
		glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
		glfwSetCursorPosCallback(m_Window, cursor_position_callback);
//		glfwSetScrollCallback(m_Window, glfw_onMouseScroll);
		//

		// Hides and grabs cursor, unlimited movement
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(m_Window, m_Width / 2.0, m_Height / 2.0);

		if (glewInit() != GLEW_OK) {

			std::cout << "Failed to create GLEW. " << std::endl;
			return false;
		}

		//Inic IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
		while (GLenum error = glGetError()) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}
		//TODO: fix error.
		
		std::cout << glGetString(GL_VERSION) << std::endl;
		glEnable(GL_DEPTH_TEST);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		return true;
	}

	void Window::clear() const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/*void Window::update(double elapsedTime) const {	
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cout << "OpenGL Error: " << error << std::endl;
		// Camera orientation
		double mouseX, mouseY;

		// Get the current mouse cursor position delta
		glfwGetCursorPos(m_Window, &mouseX, &mouseY);
		// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
		if (!m_DebugMouse) {
			glfwSetCursorPos(m_Window, getWidth() / 2.0, getHeight() / 2.0);
			m_FpsCamera->rotate((float)(getWidth() / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(getHeight() / 2.0 - mouseY) * MOUSE_SENSITIVITY);
		}


		// Camera FPS movement

		// Forward/backward
		if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * m_FpsCamera->getLook(), false);
		else if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * -m_FpsCamera->getLook(), false);

		// Strafe left/right
		if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * -m_FpsCamera->getRight(), false);
		else if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * m_FpsCamera->getRight(), false);

		// Up/down
		if (glfwGetKey(m_Window, GLFW_KEY_Z) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * m_FpsCamera->getUp(), false);
		else if (glfwGetKey(m_Window, GLFW_KEY_X) == GLFW_PRESS)
			m_FpsCamera->move(MOVE_SPEED * (float)elapsedTime * -m_FpsCamera->getUp(), false);
				
		if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			MOVE_SPEED = 50.0f;
		}
		else {
			MOVE_SPEED = 5.0f;
		}


		if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS && m_KeyUp == true) {
			m_DebugMouse = !m_DebugMouse;
			m_KeyUp = false;
			if (m_DebugMouse) {
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			} else {
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		} else if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_RELEASE) {

			m_KeyUp = true;
		}
		//m_MousePicker->update();
		//std::cout << "Vector3f [" << m_MousePicker->getCurrentTerrainPoint().x << ", " << m_MousePicker->getCurrentTerrainPoint().y << ", " << m_MousePicker->getCurrentTerrainPoint().z << "]" << std::endl;
		//glfwSwapBuffers(m_Window);
	}*/

	bool Window::closed() const {
		return glfwWindowShouldClose(m_Window);
	}

	void window_resize(GLFWwindow *window, int width, int height) {
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->setWidth(width);
		win->setHeight(height);
		glViewport(0, 0, width, height);
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* win = (Window*) glfwGetWindowUserPointer(window);
		win->m_Keys[key] = (action != GLFW_RELEASE);
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		{
			win->m_Wireframe = !win->m_Wireframe;
			if (win->m_Wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
		{
			win->m_MouseArrow = !win->m_MouseArrow;
			if (win->m_MouseArrow)
				glfwSetInputMode(win->m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else
				glfwSetInputMode(win->m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_MouseButtons[button] = (action != GLFW_RELEASE);
	}
	void cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->mx = xpos;
		win->my = ypos;
	}
	/*void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		double fov = win->getCamera()->getFOV() + deltaY * win->ZOOM_SENSITIVITY;

		fov = glm::clamp(fov, 1.0, 120.0);

		win->getCamera()->setFOV((float)fov);
	}*/
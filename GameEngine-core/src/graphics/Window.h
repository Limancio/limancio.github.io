#pragma once

#include "Camera.h";
#include "glm/glm.hpp"

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

class MousePicker;

#define MAX_KEYS	1024
#define MAX_BUTTONS 32

class Window {
public:
	static float m_ElapsedTime;

private:
	friend struct GLFWwindow;
	const char *m_Title;
	int m_Width, m_Height;
	GLFWwindow *m_Window;


	bool m_Wireframe;
	bool m_MouseArrow;
	bool m_Keys[MAX_KEYS];
	bool m_MouseButtons[MAX_BUTTONS];
	double mx, my;

public:
	Window() = default;
	Window(const char *title, int width, int height);
	~Window();
	void clear() const;
	bool closed() const;

	inline bool isMouseLocked() const { return m_MouseArrow; }

	inline static void setElapsedTime(float elapsedTime) { m_ElapsedTime = elapsedTime; }
	inline static float getElapsedTime() { return m_ElapsedTime; }

	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }

	inline void setWidth(int value) { m_Width = value; }
	inline void setHeight(int value) { m_Height = value; }

	inline bool getWireFrame() const { return m_Wireframe; }
	inline GLFWwindow* getWindow() const { return m_Window; }
		
	bool isKeyPressed(unsigned int keyCode) const;
	bool isMouseButtonPressed(unsigned int button) const;
	glm::vec2 getMousePosition() const;
private:
	bool init();
	friend static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	friend static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	//friend static void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
};

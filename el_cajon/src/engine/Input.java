package engine;

import maths.lvec2;

import static org.lwjgl.glfw.GLFW.*;

public class Input {

	public lvec2 mouse_pos;
	public boolean left_button_pressed;
	public boolean right_button_pressed;
	
	public void init(Window window) {
		mouse_pos = new lvec2(0, 0);

        glfwSetMouseButtonCallback(window.window_handle, (windowHandle, button, action, mode) -> {
        	left_button_pressed  = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
        	right_button_pressed = button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS;
        });
        
        glfwSetCursorPosCallback(window.window_handle, (windowHandle, xpos, ypos) -> {
        	// @NOTE(Liman1): In this case we make (0,0) the center of the window.
        	
        	mouse_pos.x = xpos - (window.width  * 0.5f);
        	mouse_pos.y = (window.height * 0.5f) - ypos;
        });
	}
}

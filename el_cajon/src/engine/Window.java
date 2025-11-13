package engine;

import static org.lwjgl.glfw.GLFW.*;
import org.lwjgl.glfw.GLFWErrorCallback;
import org.lwjgl.glfw.GLFWVidMode;
import org.lwjgl.opengl.GL;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.system.MemoryUtil.NULL;

public class Window {
	public String title;
	public int width;
	public int height;
	
	public long window_handle;
	public boolean resized;
	public boolean vsync;

	public boolean typed_keys[];
	public boolean press_keys[];
	public boolean up_keys   [];
	public boolean last_keys [];
	public final int MAX_KEYS = 1024;
    
	public Window(String title, int width, int height, boolean vsync) {
		typed_keys = new boolean[MAX_KEYS];
		press_keys = new boolean[MAX_KEYS];
		up_keys	   = new boolean[MAX_KEYS];
		last_keys  = new boolean[MAX_KEYS];
		
        this.title = title;
        this.width = width;
        this.height = height;
        this.vsync = vsync;
        this.resized = false;
    }
	
	public void init() {
        // Setup an error callback. The default implementation
        // will print the error message in System.err.
        GLFWErrorCallback.createPrint(System.err).set();

        // Initialize GLFW. Most GLFW functions will not work before doing this.
        if (!glfwInit()) {
            throw new IllegalStateException("Unable to initialize GLFW");
        }

        glfwDefaultWindowHints(); 				 // optional, the current window hints are already the default
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);  // the window will stay hidden after creation
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // the window will be resizable
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create the window
        window_handle = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window_handle == NULL) {
            throw new RuntimeException("Failed to create the GLFW window");
        }

        // Setup resize callback
        glfwSetFramebufferSizeCallback(window_handle, (window, width, height) -> {
            this.width   = width;
            this.height  = height;
            this.resized = true;
            glViewport(0, 0, width, height);
        });

        // Setup a key callback. It will be called every time a key is pressed, repeated or released.
        glfwSetKeyCallback(window_handle, (window, key, scancode, action, mods) -> {
        	press_keys[key] = glfwGetKey(window_handle, key) == GLFW_PRESS;
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(window, true); // We will detect this in the rendering loop
            }
        });

        // Get the resolution of the primary monitor
        GLFWVidMode vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(
    		window_handle,
            (vidmode.width() - width)   / 2,
            (vidmode.height() - height) / 2
        );

        // Make the OpenGL context current
        glfwMakeContextCurrent(window_handle);

        if(vsync) {
            glfwSwapInterval(1);
        }
        
        glfwShowWindow(window_handle);
        GL.createCapabilities();
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);  
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    }

	public void set_clear_color(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
	
    public void update() {
        glfwSwapBuffers(window_handle);
        glfwPollEvents();
        
        for(int key = 0; key < MAX_KEYS; key++) {
            if(!last_keys[key] && press_keys[key]) {
                typed_keys[key] = true;
            } else {
                typed_keys[key] = false;
            }
            if(last_keys[key] && !press_keys[key]) {
                up_keys[key] = true;
            } else {
                up_keys[key] = false;
            }
        }

        for(int key = 0; key < MAX_KEYS; key++) {
        	last_keys[key] = press_keys[key];
        }
    }
    
    public boolean is_key_press(int key_code) {
        return(press_keys[key_code]);
    }

    public boolean is_key_typed(int key_code) {
        return(typed_keys[key_code]);
    }

    public boolean window_should_close() {
        return glfwWindowShouldClose(window_handle);
    }

	public void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}

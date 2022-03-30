#include <glad/glad.h>
#include "render_window.h"

namespace ifcre {
    // --------------------- event processing ------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        that->recreateFramebuffer(width, height);
        glViewport(0, 0, width, height);
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        
        that->zoom_parameter -= (float)(0.1 * yoffset);
        if (that->zoom_parameter < 0.1) {
            that->zoom_parameter = 0.1;
        }
        std::cout << "that->zoom_parameter: " << that->zoom_parameter << std::endl;
    }
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    // --------------------- construction ----------------------
	RenderWindow::RenderWindow(const char* title, int w, int h, bool vsync)
	{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        m_window = glfwCreateWindow(w, h, title, NULL, NULL);
        if (m_window == NULL)
        {

            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        glfwSetWindowUserPointer(m_window, this);

        // load gl functions by glad
		static bool load_gl = false;
		if (!load_gl && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			load_gl = true;
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

        if (vsync) {
            glfwSwapInterval(1);
        }

        createFramebuffer(w, h);
        zoom_parameter = 3.0;
	}
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    RenderWindow::~RenderWindow()
    {
        glDeleteTextures(1, &m_framebuffer.colortex_id);
        glDeleteRenderbuffers(1, &m_framebuffer.rbo_id);
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void RenderWindow::processInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_window, true);
        }
    }

    bool RenderWindow::isClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    void RenderWindow::swapBuffer()
    {
        glfwSwapBuffers(m_window);
    }

    void RenderWindow::pollEvents()
    {
        glfwPollEvents();
    }

    void RenderWindow::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
    }

    void RenderWindow::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderWindow::recreateFramebuffer(int w, int h)
    {
        glDeleteTextures(1, &m_framebuffer.colortex_id);
        glDeleteRenderbuffers(1, &m_framebuffer.rbo_id);
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        createFramebuffer(w, h);
    }

    uint32_t RenderWindow::getColorTexId()
    {
        return m_framebuffer.colortex_id;
    }

// private:
    void RenderWindow::createFramebuffer(int w, int h)
    {
        auto& mfb = m_framebuffer;

        // create a color attachment texture
        glCreateTextures(GL_TEXTURE_2D, 1, &mfb.colortex_id);
        //glBindTexture(GL_TEXTURE_2D, mfb.colortex_id);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTextureStorage2D(mfb.colortex_id, 1, GL_RGBA8, w, h);
        glTextureParameteri(mfb.colortex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(mfb.colortex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glCreateFramebuffers(1, &mfb.fbo_id);
        glNamedFramebufferTexture(mfb.fbo_id, GL_COLOR_ATTACHMENT0, mfb.colortex_id, 0);

        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        //unsigned int rbo;
        //glGenRenderbuffers(1, &rbo);
        //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        glCreateRenderbuffers(1, &mfb.rbo_id);
        glNamedRenderbufferStorage(mfb.rbo_id, GL_DEPTH24_STENCIL8, w, h);
        glNamedFramebufferRenderbuffer(mfb.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mfb.rbo_id);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
    }

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}
#include <glad/glad.h>
#include "render_window.h"

namespace ifcre {
    // --------------------- event processing ------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
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

        auto& mfb = m_framebuffer;

        // create a color attachment texture
        glCreateTextures(GL_TEXTURE_2D, 1, &mfb.colortex_id);

        //glBindTexture(GL_TEXTURE_2D, m_framebuffer.color_texid);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTextureStorage2D(mfb.colortex_id, 0, GL_RGBA, w, h);
        glTextureSubImage2D(mfb.colortex_id, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(mfb.colortex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(mfb.colortex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mfb.colortex_id, 0);
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

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
}
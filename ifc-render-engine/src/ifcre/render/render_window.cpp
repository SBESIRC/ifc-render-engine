#include <glad/glad.h>
#include "render_window.h"

namespace ifcre {
    // --------------------- event processing ------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        that->recreateFramebuffer(width, height);
        glViewport(0, 0, width, height);
    }

    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {

    }

    void RenderWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        camera.translateByScreenOp(0, 0, yoffset);
    }
    
    static void mouse_button_button_callback(GLFWwindow* window, int button, int action, int mods) {

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
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

        // mouse callback
        glfwSetCursorPosCallback(m_window, cursor_pos_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_button_callback);
        

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
        
	}
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    RenderWindow::~RenderWindow()
    {
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

    void RenderWindow::startRenderToWindow()
    {
        m_cur_fbo = m_framebuffer.fbo_id;
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
    }

    void RenderWindow::endRenderToWindow()
    {
        m_cur_fbo = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderWindow::switchRenderDepthNormal()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_depth_normal_rt->getTexId(), 0);
        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
    }

    void RenderWindow::switchRenderColor()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_default_rt->getTexId(), 0);
        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
    }

    void RenderWindow::recreateFramebuffer(int w, int h)
    {
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        createFramebuffer(w, h);
    }

    uint32_t RenderWindow::getColorTexId()
    {
        //return m_framebuffer.colortex_id;
        return m_framebuffer.m_default_rt->getTexId();
    }

    void RenderWindow::setCamera(SharedPtr<GLCamera> camera)
    {
        m_camera = camera;
    }

// private:
    void RenderWindow::createFramebuffer(int w, int h)
    {
        auto& mfb = m_framebuffer;
        mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);
        glCreateFramebuffers(1, &mfb.fbo_id);
        glNamedFramebufferTexture(mfb.fbo_id, GL_COLOR_ATTACHMENT0, mfb.m_default_rt->getTexId(), 0);
        //glNamedFramebufferTexture(mfb.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, mfb.m_default_rt->getDepthId(), 0);
        glNamedFramebufferRenderbuffer(mfb.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mfb.m_default_rt->getDepthId());

        mfb.m_depth_normal_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
    }

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}
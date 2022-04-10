#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render_window.h"

namespace ifcre {
    bool m_lbutton_down, m_rbutton_down;
    double lastX, lastY, curX, curY;
    double rlastX, rlastY, rcurX, rcurY;

    // --------------------- event helper ----------------------
    
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    
    // --------------------- event processing ------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        that->recreateFramebuffer(width, height);
        glViewport(0, 0, width, height);
    }

    void RenderWindow::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& cur_rt = *(that->m_cur_rt);
        auto& status = that->m_status;
        if (status.lbtn_down) {

            if (status.last_mouse_x != xpos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                that->horizontalRot = status.last_mouse_x - xpos > 0 ? 1 : -1;
            }

            if (status.last_mouse_y != ypos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                that->verticalRot = status.last_mouse_y - ypos > 0 ? 1 : -1;
            }
            //if (status.last_mouse_y != ypos) {
            //    camera.rotateByScreenY(status.click_world_center, glm::radians((status.last_mouse_y - ypos) > 0 ? 5.0f : -5.0f));
            //}
        }


        if (status.rbtn_down) {
            //glfwGetCursorPos(window, &curX, &curY);
            camera.translate(status.last_mouse_x - xpos, ypos - status.last_mouse_y);
            //camera.translateByScreenOp(status.last_mouse_x - xpos, ypos - status.last_mouse_y, 0);
        }

        status.last_mouse_x = xpos;
        status.last_mouse_y = ypos;
    }

    void RenderWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        camera.translateByScreenOp(0, 0, yoffset);
    }
    
    void RenderWindow::mouse_button_button_callback(GLFWwindow* window, int button, int action, int mods) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& status = that->m_status;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switch (action) {
            case GLFW_PRESS: {
                // OpenGL Screen space:
                //  ^y
                //   | 
                //   ¨N¡ª>x
                Real z;
                Real w = that->m_width, h = that->m_height;
                double click_x, click_y;
                glfwGetCursorPos(window, &click_x, &click_y);
                glBindFramebuffer(GL_FRAMEBUFFER, that->m_framebuffer.fbo_id);
                glReadPixels(click_x, h - click_y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                status.lbtn_down = true;
                if (z == 1.0) {
                    break;
                }
                // from [0, 1] to [-1, 1]
                Real y = (h - click_y - 1) / h * 2 - 1;
                Real x = click_x / w * 2 - 1;
                z = z * 2 - 1;
                glm::vec4 ndc(x, y, z, 1.0f);
                glm::mat4 vp_inv = glm::inverse(that->m_projection * camera.getViewMatrix());
                glm::vec4 t = vp_inv * ndc;
                t = t / t.w;
                status.click_world_center = t;

                that->clickWorldCenter = t;
                break;
            }
            case GLFW_RELEASE:
                status.lbtn_down = false;
                break;
            }
        }
        
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (GLFW_PRESS == action) {/*
                m_lbutton_down = true;
                glfwGetCursorPos(window, &lastX, &lastY);*/
                status.rbtn_down = true;
            }
            else if(GLFW_RELEASE == action) {/*
                m_lbutton_down = false;*/
                status.rbtn_down = false;
            }
        }
    }
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    // --------------------- construction ----------------------
	RenderWindow::RenderWindow(const char* title, int32_t w, int32_t h, bool vsync)
	{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // glfw window creation
        m_window = glfwCreateWindow(w, h, title, NULL, NULL);
        m_lbutton_down = m_rbutton_down = false;
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
        horizontalRot = 0;
        verticalRot = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderWindow::switchRenderDepthNormal()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_depth_normal_rt->getTexId(), 0);
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, m_framebuffer.m_depth_normal_rt->getDepthId(), 0);

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
        m_cur_rt = m_framebuffer.m_depth_normal_rt.get();
    }

    void RenderWindow::switchRenderColor()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_default_rt->getTexId(), 0);
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
        m_cur_rt = m_framebuffer.m_default_rt.get();
    }

    void RenderWindow::recreateFramebuffer(int w, int h)
    {
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        createFramebuffer(w, h);
    }
    
    uint32_t RenderWindow::getColorTexId()
    {
        return m_framebuffer.m_default_rt->getTexId();
    }

    uint32_t RenderWindow::getDepthNormalTexId() 
    {
        return m_framebuffer.m_depth_normal_rt->getTexId();
    }

    glm::vec2 RenderWindow::getWindowSize() 
    {
        return glm::vec2(m_width, m_height);
    }

    glm::mat4 RenderWindow::getProjMatrix() {
        return m_projection;
    }

    void RenderWindow::setCamera(SharedPtr<GLCamera> camera)
    {
        m_camera = camera;
    }

// private:
    void RenderWindow::createFramebuffer(int w, int h)
    {
        m_width = w;
        m_height = h;
        m_projection = glm::perspective(glm::radians(fov), (Real)w / h, m_znear, m_zfar);
        auto& mfb = m_framebuffer;
        mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH32);
        glCreateFramebuffers(1, &mfb.fbo_id);
        glNamedFramebufferTexture(mfb.fbo_id, GL_COLOR_ATTACHMENT0, mfb.m_default_rt->getTexId(), 0);
        //glNamedFramebufferTexture(mfb.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, mfb.m_default_rt->getDepthId(), 0);
        glNamedFramebufferTexture(mfb.fbo_id, GL_DEPTH_ATTACHMENT, mfb.m_default_rt->getDepthId(), 0);
        //glNamedFramebufferRenderbuffer(mfb.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mfb.m_default_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(mfb.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mfb.m_default_rt->getDepthId());
        m_cur_rt = mfb.m_default_rt.get();

        mfb.m_depth_normal_rt = make_shared<GLRenderTexture>(w, h, DEPTH32);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
    }

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}
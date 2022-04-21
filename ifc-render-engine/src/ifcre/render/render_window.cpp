#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render_window.h"
#define TEST_COMP_ID

namespace ifcre {
    bool m_lbutton_down, m_rbutton_down;
    double lastX, lastY, curX, curY;
    double rlastX, rlastY, rcurX, rcurY;

    // --------------------- event helper ----------------------
    //float get_depth_value(GLRTDepthFormatEnum depth_enum) {
    //    
    //}

    void RenderWindow::_setClickedWorldCoords(double click_x, double click_y) {
        // OpenGL Screen space:
        //  ^y
        //   | 
        //   �N��>x
        Real z;
        Real w = m_width, h = m_height;
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        glReadPixels(click_x, h - click_y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (z == 1.0) {
            return;
        }
        z = z * 2 - 1;
        m_mouse_status.click_z = z;
        // from [0, 1] to [-1, 1]
        Real y = (h - click_y - 1) / h * 2 - 1;
        Real x = click_x / w * 2 - 1;
        glm::vec4 ndc(x, y, z, 1.0f);
        glm::mat4 vp_inv = glm::inverse(m_projection * m_camera->getViewMatrix());
        glm::vec4 t = vp_inv * ndc;
        t = t / t.w;
        m_mouse_status.click_world_center = t;
    }

    void RenderWindow::_setClickedWorldColors(double click_x, double click_y) {

        glm::vec3 getColor;
        Real w = m_width, h = m_height;
        m_cur_rt = m_framebuffer.m_comp_id_rt.get();
        m_cur_rt->attach(m_framebuffer.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        glReadPixels(click_x, h - click_y - 1, 1, 1, GL_RGB, GL_FLOAT, &getColor);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (glm::distance(getColor,glm::vec3(1.f, 1.f, 1.f))<0.001) {
            m_mouse_status.click_comp_id = -1;
            std::cout << "NO!" << std::endl;
            return;
        }
        if (glm::distance(getColor, glm::vec3(0.f, 0.f, 0.f)) < 0.001) {
            m_mouse_status.click_comp_id = -1;
            std::cout << "NO2!" << std::endl;
            return;
        }
        int clicked_comp_id = 0;
        clicked_comp_id += (int)round(getColor.x * 128) << 16;
        clicked_comp_id += (int)round(getColor.y * 128) << 8;
        clicked_comp_id += (int)round(getColor.z * 128);
        m_mouse_status.click_comp_id = clicked_comp_id;
        std::cout << clicked_comp_id << std::endl;
    }

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
        auto& status = that->m_mouse_status;
        if (status.lbtn_down) {
            if (status.last_mouse_x != xpos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                status.horizontal_move = status.last_mouse_x - xpos < 0 ? 1 : -1;

            }
            if (status.last_mouse_y != ypos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                status.vertical_move = status.last_mouse_y - ypos < 0 ? 1 : -1;
            }
            //if (status.last_mouse_y != ypos) {
            //    camera.rotateByScreenY(status.click_world_center, glm::radians((status.last_mouse_y - ypos) > 0 ? 5.0f : -5.0f));
            //}
        }


        if (status.rbtn_down) {
            //glfwGetCursorPos(window, &curX, &curY);
            //camera.translate(lastX - curX, curY - lastY);
            //lastX = curX, lastY = curY;

            if (status.last_mouse_x != xpos) {
                status.horizontal_move = status.last_mouse_x - xpos < 0 ? 1 : -1;

            }
            if (status.last_mouse_y != ypos) {
                status.vertical_move = status.last_mouse_y - ypos < 0 ? 1 : -1;
            }
            //camera.translateByScreenOp(status.last_mouse_x - xpos, ypos - status.last_mouse_y, 0);
            Real w = that->m_width, h = that->m_height;
            Real y = (h - ypos - 1) / h * 2 - 1;
            Real x = xpos / w * 2 - 1;

            glm::vec4 ndc(x, y, status.click_z, 1.0f);
            glm::mat4 vp_inv = glm::inverse(that->m_projection * camera.getViewMatrix());
            glm::vec4 t = vp_inv * ndc;
            t = t / t.w;
            status.hover_world_coord = t;
            status.click_world_center.x = t.x;
            status.click_world_center.y = t.y;
        }

        status.last_mouse_x = xpos;
        status.last_mouse_y = ypos;
    }

    void RenderWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        double click_x, click_y;
        glfwGetCursorPos(window, &click_x, &click_y);
        that->_setClickedWorldCoords(click_x, click_y);
#ifdef TEST_COMP_ID
        that->_setClickedWorldColors(click_x, click_y);
#endif // TEST_COMP_ID

        //camera.translateByScreenOp(0, 0, yoffset);
        camera.zoom(that->m_mouse_status.click_world_center, yoffset > 0 ? 1.0f : -1.0f);
        
    }
    
    void RenderWindow::mouse_button_button_callback(GLFWwindow* window, int button, int action, int mods) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& status = that->m_mouse_status;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switch (action) {
            case GLFW_PRESS: {
                double click_x, click_y;
                glfwGetCursorPos(window, &click_x, &click_y);
                that->_setClickedWorldCoords(click_x, click_y);
#ifdef TEST_COMP_ID
                that->_setClickedWorldColors(click_x, click_y);
#endif // TEST_COMP_ID
                status.lbtn_down = true;
                break;
            }
            case GLFW_RELEASE:
                status.lbtn_down = false;
                break;
            }
        }
        
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {

            switch (action) {
            case GLFW_PRESS: {
                double click_x, click_y;
                glfwGetCursorPos(window, &click_x, &click_y);
                that->_setClickedWorldCoords(click_x, click_y);
#ifdef TEST_COMP_ID
                that->_setClickedWorldColors(click_x, click_y);
#endif // TEST_COMP_ID
                status.rbtn_down = true;
                break;
            }
            case GLFW_RELEASE: {

                status.rbtn_down = false;
                break;
            }
            }

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
	RenderWindow::RenderWindow(const char* title, int32_t w, int32_t h, bool aa, bool vsync)
	{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_option.anti_aliasing = aa;
        if (aa) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }

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
        else {
            glfwSwapInterval(0);
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
        double now_time = glfwGetTime();
        m_delta_time = now_time - m_last_time;
        m_last_time = now_time;

        m_mouse_status.horizontal_move = 0;
        m_mouse_status.vertical_move = 0;
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
        if (m_option.anti_aliasing) {
            m_framebuffer.m_default_rt->attach(m_framebuffer.fbo_id);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa_fb.fbo_id);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer.fbo_id);
            glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }
        m_cur_fbo = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderWindow::switchRenderCompId() {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.\n", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_comp_id_rt->getTexId(), 0);
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, m_framebuffer.m_comp_id_rt->getDepthId(), 0);
        m_cur_rt = m_framebuffer.m_comp_id_rt.get();
        m_cur_rt->attach(m_framebuffer.fbo_id);
    }

    void RenderWindow::switchRenderDepthNormal()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.\n", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        m_cur_rt = m_framebuffer.m_depth_normal_rt.get();
        m_cur_rt->attach(m_framebuffer.fbo_id);

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
    }

    void RenderWindow::switchRenderColor()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.\n", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        
        //bool open_aa = m_cur_rt->isOpenAA();
        //glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_framebuffer.m_default_rt->getTexId(), 0);
        ////glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_cur_rt->getDepthId(), 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_framebuffer.m_default_rt->getDepthId(), 0);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (m_option.anti_aliasing) {
            m_cur_rt = m_msaa_fb.m_msaa_rt.get();
            m_cur_rt->attach(m_msaa_fb.fbo_id);
            glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_fb.fbo_id);
        }
        else {
            m_cur_rt = m_framebuffer.m_default_rt.get();
            m_cur_rt->attach(m_framebuffer.fbo_id);
        }

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
    }

    void RenderWindow::recreateFramebuffer(int w, int h)
    {
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        createFramebuffer(w, h);
    }

    void RenderWindow::readPixels() {
        Vector<float> data(m_width * m_height * 3);
        glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_FLOAT, data.data());
        printf("");
    }
    
    uint32_t RenderWindow::getFBOId()
    {
        return m_framebuffer.fbo_id;
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

// --------------------- mouse status ---------------------
    glm::vec3 RenderWindow::getClickedWorldCoord()
    {
        return m_mouse_status.click_world_center;
    }
    glm::vec3 RenderWindow::getHoverWorldCoord()
    {
        return m_mouse_status.hover_world_coord;
    }
    float RenderWindow::getMouseHorizontalVel()
    {
        return m_option.mouse_hori_vel * m_mouse_status.horizontal_move;
    }
    float RenderWindow::getMouseVerticalVel()
    {
        return m_option.mouse_vert_vel * m_mouse_status.vertical_move;
    }
    bool RenderWindow::isMouseHorizontalRot()
    {
        return m_mouse_status.lbtn_down 
            && !m_mouse_status.rbtn_down 
            && m_mouse_status.horizontal_move != 0;
    }
    bool RenderWindow::isMouseVerticalRot()
    {
        return m_mouse_status.lbtn_down 
            && !m_mouse_status.rbtn_down 
            && m_mouse_status.vertical_move != 0;
    }
    bool RenderWindow::isMouseMove()
    {
        return m_mouse_status.rbtn_down 
            && !m_mouse_status.lbtn_down
            && (m_mouse_status.horizontal_move != 0 
                || m_mouse_status.vertical_move != 0);
    }
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

// private:
    void RenderWindow::createFramebuffer(int w, int h)
    {
        if (w == 0 || h == 0) {
            return;
        }
        m_width = w;
        m_height = h;
        m_projection = glm::perspective(glm::radians(fov), (Real)w / h, m_znear, m_zfar);
        auto& mfb = m_framebuffer;
        //mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH32);
        mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);
        glCreateFramebuffers(1, &mfb.fbo_id);
        //glBindFramebuffer(GL_FRAMEBUFFER, mfb.fbo_id);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mfb.m_default_rt->getTexId(), 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mfb.m_default_rt->getDepthId(), 0);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mfb.m_default_rt->attach(m_framebuffer.fbo_id);

        if (m_option.anti_aliasing) {
            glCreateFramebuffers(1, &m_msaa_fb.fbo_id);
            m_msaa_fb.m_msaa_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY, true);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
        }

        m_cur_rt = mfb.m_default_rt.get();

        mfb.m_depth_normal_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);
        mfb.m_comp_id_rt = make_shared<GLRenderTexture>(w, h, DEPTH32);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
    }



// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}
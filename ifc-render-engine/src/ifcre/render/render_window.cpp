#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render_window.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "glfw3native.h"
#define TEST_COMP_ID

namespace ifcre {
    bool m_lbutton_down, m_rbutton_down;
    double lastX, lastY, curX, curY;
    double rlastX, rlastY, rcurX, rcurY;

    // --------------------- event helper ----------------------

    void RenderWindow::_setClickedWorldCoords(double clicked_x, double clicked_y, double clicked_z) {
        // OpenGL Screen space:
        //  ^y
        //   | 
        //   �N��>x
        Real w = m_width, h = m_height;
        // from [0, 1] to [-1, 1]
        Real y = (h - clicked_y - 1) / h * 2 - 1;
        Real x = clicked_x / w * 2 - 1;
        Real z = clicked_z * 2 - 1;
        m_mouse_status.click_z = z;
        glm::vec4 ndc(x, y, z, 1.0f);
        glm::mat4 vp_inv = glm::inverse(m_projection * m_camera->getViewMatrix());
        glm::vec4 t = vp_inv * ndc;
        t = t / t.w;
        m_mouse_status.click_world_center = t;
        m_mouse_status.click_x = clicked_x;
        m_mouse_status.click_y = clicked_y;
    }

    float RenderWindow::_getClickedDepthValue(double clicked_x, double clicked_y)
    {
        Real z;
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        glReadPixels(clicked_x, m_height - clicked_y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return z;
    }

    void RenderWindow::_setClickedWorldColors(double click_x, double click_y,bool hover_mode) {

        //glm::vec3 getColor;
        //Real w = m_width, h = m_height;
        ////m_cur_rt = m_framebuffer.m_comp_id_rt.get();
        //m_cur_rt = m_comp_fb.m_comp_id_rt.get();
        ////glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        //glBindFramebuffer(GL_FRAMEBUFFER, m_comp_fb.fbo_id);
        //glReadPixels(click_x, h - click_y - 1, 1, 1, GL_RGB, GL_FLOAT, &getColor);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //if (glm::distance(getColor, glm::vec3(1.f, 1.f, 1.f)) < 0.001) {
        //    m_mouse_status.click_comp_id = -1;
        //    std::cout << "NO!" << std::endl;
        //    return;
        //}
        //int clicked_comp_id = 0;
        //clicked_comp_id += (int)round(getColor.x * 256) << 16;
        //clicked_comp_id += (int)round(getColor.y * 256) << 8;
        //clicked_comp_id += (int)round(getColor.z * 256);
        //if (hover_mode)
        //    m_mouse_status.hover_comp_id = clicked_comp_id;
        //else
        //    m_mouse_status.click_comp_id = clicked_comp_id;


        uint32_t clicked_comp_id;
        Real w = m_width, h = m_height;
        //m_cur_rt = m_framebuffer.m_comp_id_rt.get();
        m_cur_rt = m_comp_fb.m_comp_id_rt.get();
        //glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_comp_fb.fbo_id);
        glReadPixels(click_x, h - click_y - 1, 1, 1, GL_RED_INTEGER, GL_INT, &clicked_comp_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //if (comp_id == -1) {
        //    m_mouse_status.click_comp_id = -1;
        //    //std::cout << "NO!" << std::endl;
        //    return;
        //}

        //std::cout << comp_id.x << "\n";
        
        if (hover_mode)
            m_mouse_status.hover_comp_id = clicked_comp_id;
        else
            m_mouse_status.click_comp_id = clicked_comp_id;
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    
    // --------------------- event processing ------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        that->recreateFramebuffer(width, height);
    }

    void RenderWindow::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& cur_rt = *(that->m_cur_rt);
        auto& status = that->m_mouse_status;
        that->_setClickedWorldColors(xpos, ypos, true);
        if (status.lbtn_down || status.rbtn_down) {
            if (status.last_mouse_x != xpos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                status.horizontal_move = xpos - status.last_mouse_x;

            }
            if (status.last_mouse_y != ypos) {
                //camera.rotateByScreenX(status.click_world_center, glm::radians((status.last_mouse_x - xpos) > 0 ? 2.0f : -2.0f));
                status.vertical_move = ypos - status.last_mouse_y;
            }
            //if (status.last_mouse_y != ypos) {
            //    camera.rotateByScreenY(status.click_world_center, glm::radians((status.last_mouse_y - ypos) > 0 ? 5.0f : -5.0f));
            //}
        }


        if (status.rbtn_down) {
            //glfwGetCursorPos(window, &curX, &curY);
            //camera.translate(lastX - curX, curY - lastY);
            //lastX = curX, lastY = curY;
            //camera.translateByScreenOp(status.last_mouse_x - xpos, ypos - status.last_mouse_y, 0);
            float click_z = that->_getClickedDepthValue(xpos, ypos);

            Real w = that->m_width, h = that->m_height;
            if (status.click_z == 1.0) {
                glm::vec4 pos = glm::vec4(status.click_world_center, 1.0);
                pos = that->m_projection * camera.getViewMatrix() * pos;
                pos /= pos.w;
                status.click_z = pos.z;
                // from [-1, 1] to [0, width], [0, height]
                status.click_x = (pos.x + 1) * 0.5 * w;
                status.click_y = (-pos.y + 1) * 0.5 * h;
            }
            Real x, y;
            if (click_z != 1.0 
                && (ypos >= 0 && ypos < h && xpos >= 0 && xpos < w)
                && status.click_init_mask == 1) {
                y = (h - ypos - 1) / h * 2 - 1;
                x = xpos / w * 2 - 1;
            }
            else {
                y = (h - (status.click_y + (ypos - status.last_mouse_y)) - 1) / h * 2 - 1;
                x = (status.click_x + (xpos - status.last_mouse_x)) / w * 2 - 1;
            }

			glm::vec4 ndc(x, y, status.click_z, 1.0f);
			glm::mat4 vp_inv = glm::inverse(that->m_projection * camera.getViewMatrix());
			glm::vec4 t = vp_inv * ndc;
			t = t / t.w;
			status.hover_world_center = t;
			//status.click_world_center.x = t.x;
			//status.click_world_center.y = t.y;
            status.click_world_center = t;
            status.click_y = status.click_y + (ypos - status.last_mouse_y);
            status.click_x = status.click_x + (xpos - status.last_mouse_x);
        }
        status.last_mouse_x = xpos;
        status.last_mouse_y = ypos;
    }

    void RenderWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& status = that->m_mouse_status;
        double click_x, click_y;
        glfwGetCursorPos(window, &click_x, &click_y);
        float click_z = that->_getClickedDepthValue(click_x, click_y);
        if (click_z != 1.0) {
            that->_setClickedWorldCoords(click_x, click_y, click_z);
            //that->_setClickedWorldColors(click_x, click_y, false);
        }
        camera.zoom(that->m_mouse_status.click_world_center, yoffset > 0 ? 1.0f : -1.0f);

        Real w = that->m_width, h = that->m_height;
        glm::vec4 pos = glm::vec4(status.click_world_center, 1.0);
        pos = that->m_projection * camera.getViewMatrix() * pos;
        pos /= pos.w;
        status.click_z = pos.z;
        // from [-1, 1] to [0, width], [0, height]
        status.click_x = (pos.x + 1) * 0.5 * w;
        status.click_y = (-pos.y + 1) * 0.5 * h;
        //camera.translateByScreenOp(0, 0, yoffset);
        
    }
    
    void RenderWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        RenderWindow* that = (RenderWindow*)glfwGetWindowUserPointer(window);
        auto& camera = *(that->m_camera);
        auto& status = that->m_mouse_status;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switch (action) {
            case GLFW_PRESS: {
                double click_x, click_y;
                glfwGetCursorPos(window, &click_x, &click_y);
                float click_z = that->_getClickedDepthValue(click_x, click_y);
                if (click_z != 1.0) {
                    that->_setClickedWorldCoords(click_x, click_y, click_z);
                    that->_setClickedWorldColors(click_x, click_y, false);
                }
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
                float click_z = that->_getClickedDepthValue(click_x, click_y);
                if (click_z != 1.0) {
                    that->_setClickedWorldCoords(click_x, click_y, click_z);
#ifdef TEST_COMP_ID
                    //that->_setClickedWorldColors(click_x, click_y, false);
#endif // TEST_COMP_ID
                    that->m_mouse_status.click_init_mask = 1;
                }
                else {
                    that->m_mouse_status.click_init_mask = -1;
                }
                status.rbtn_down = true;
                break;
            }
            case GLFW_RELEASE: {
                status.rbtn_down = false;
                break;
            }
            }
        }
    }
    void errorCallback(int error, const char* description) 
    {
        
    }
    bool RenderWindow::Init(const char* title, int32_t w, int32_t h, bool aa, bool vsync, GLFWwindow* wndPtr)
    {
        glfwSetErrorCallback(errorCallback);

        if (NULL == wndPtr)
        {
            //create new window
            glfwInit();
            m_option.anti_aliasing = aa;
            m_lbutton_down = m_rbutton_down = false;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            if (aa) {
                glfwWindowHint(GLFW_SAMPLES, 4);
            }
            // glfw window creation
            m_window = glfwCreateWindow(w, h, title, NULL, NULL);
            if (m_window == NULL)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }

            glfwMakeContextCurrent(m_window);
        }
        else
        {
            //use existing
            m_window = (GLFWwindow*)wndPtr;
        }
        //glfwMakeContextCurrent(m_window);
        m_window = glfwGetCurrentContext();
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

        // mouse callback
        glfwSetCursorPosCallback(m_window, cursor_pos_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);

        // load gl functions by glad
        static bool load_gl = false;
        int rst = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!load_gl && !rst)
        {
            load_gl = true;
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        if (vsync) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }

        createFramebuffer(w, h);

        return true;
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    // --------------------- construction ----------------------
    RenderWindow::RenderWindow(const char* title, int32_t w, int32_t h, bool aa, bool vsync, GLFWwindow* wndPtr)
    {
        Init(title, w, h, aa, vsync, wndPtr);
	}
    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    RenderWindow::~RenderWindow()
    {
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        auto handle = glfwGetWin32Window(m_window);
        if (IsWindow(handle))
            glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void RenderWindow::processInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_window, true);
        }
        if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
            hidden = true;
        }
        if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) {
            hidden = false;
        }
        if (!hidden) {
            if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
                use_clip_plane.base_pos += use_clip_plane.moveSpeed * use_clip_plane.normal;
            }
            if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
                    use_clip_plane.base_pos -= use_clip_plane.moveSpeed * use_clip_plane.normal;
            }
            if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    use_clip_plane.rotateByFront(use_clip_plane.rotateSpeed);
            }
            if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    use_clip_plane.rotateByFront(-use_clip_plane.rotateSpeed);
            }
            if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
                    use_clip_plane.rotateByRight(use_clip_plane.rotateSpeed);
            }
            if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    use_clip_plane.rotateByRight(-use_clip_plane.rotateSpeed);
            }
        }
    }

    bool RenderWindow::isClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    bool RenderWindow::swapBuffer()
    {
        auto handle = glfwGetWin32Window(m_window);
        if (!IsWindow(handle))
            return false;

        glfwSwapBuffers(m_window);
        double now_time = glfwGetTime();
        m_delta_time = now_time - m_last_time;
        m_last_time = now_time;

        m_mouse_status.horizontal_move = 0;
        m_mouse_status.vertical_move = 0;

        return true;
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


            // TODO delete
            //m_framebuffer.m_comp_id_rt->attach(m_framebuffer.fbo_id);
            //glBindFramebuffer(GL_READ_BUFFER, m_comp_fb.fbo_id);
            //glBindFramebuffer(GL_DRAW_BUFFER, m_framebuffer.fbo_id);
            //glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        m_cur_fbo = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderWindow::switchRenderCompId() {
        /*if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.\n", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_COLOR_ATTACHMENT0, m_framebuffer.m_comp_id_rt->getTexId(), 0);
        glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, m_framebuffer.m_comp_id_rt->getDepthId(), 0);
        m_cur_rt = m_framebuffer.m_comp_id_rt.get();
        m_cur_rt->attach(m_framebuffer.fbo_id);
        */
        /*
           m_cur_fbo = m_comp_fb.fbo_id;
           glBindFramebuffer(GL_FRAMEBUFFER, m_comp_fb.fbo_id);
           glNamedFramebufferTexture(m_comp_fb.fbo_id, GL_COLOR_ATTACHMENT0, m_comp_fb.m_comp_id_rt->getTexId(), 0);
           glNamedFramebufferTexture(m_comp_fb.fbo_id, GL_DEPTH_ATTACHMENT, m_comp_fb.m_comp_id_rt->getDepthId(), 0);
           glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

        m_cur_rt = m_comp_fb.m_comp_id_rt.get();
        m_cur_rt->attach(m_comp_fb.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_comp_fb.fbo_id);
    }

    void RenderWindow::switchRenderDepthNormal()
    {
        if (m_cur_fbo != m_framebuffer.fbo_id) {
            printf("Current FBO ID is %d, not %d.\n", m_cur_fbo, m_framebuffer.fbo_id);
            return;
        }
        m_cur_rt = m_framebuffer.m_depth_normal_rt.get();
        m_cur_rt->attach(m_framebuffer.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_depth_normal_rt->getDepthId());
    }

    void RenderWindow::switchRenderBack() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.fbo_id);
        }

        //glNamedFramebufferTexture(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_framebuffer.m_default_rt->getDepthId(), 0);
        
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
        //glNamedFramebufferRenderbuffer(m_framebuffer.fbo_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.m_default_rt->getDepthId());
    }

    void RenderWindow::recreateFramebuffer(int w, int h)
    {
        glDeleteFramebuffers(1, &m_framebuffer.fbo_id);
        while (w == 0 || h == 0) {
            glfwGetFramebufferSize(m_window, &w, &h);
            glfwWaitEvents();
        }
        createFramebuffer(w, h);
        glViewport(0, 0, w, h);
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

    int RenderWindow::getClickCompId()
    {
        return m_mouse_status.click_comp_id;
    }

    int RenderWindow::getHoverCompId()
    {
        return m_mouse_status.hover_comp_id;
    }

    glm::vec2 RenderWindow::getWindowSize()
    {
        return glm::vec2(m_width, m_height);
    }

    glm::mat4 RenderWindow::getProjMatrix() {
        return m_projection;
    }

    ClipPlane RenderWindow::getClippingPlane() {
        if (hidden)
            return hidden_clip_plane;
        return use_clip_plane;
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
    glm::vec3 RenderWindow::getVirtualHoverWorldCoord()
    {
        return m_mouse_status.hover_world_center;
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
        return (m_mouse_status.horizontal_move != 0
            || m_mouse_status.vertical_move != 0);
    }
    bool RenderWindow::isRightMouseClicked()
    {
        return m_mouse_status.rbtn_down;
    }

    void RenderWindow::setDefaultStatus() {
        m_mouse_status.click_comp_id = -1;
        m_mouse_status.hover_comp_id = -1;
        hidden = true;
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
        mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH32);
        mfb.m_default_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);
        glCreateFramebuffers(1, &mfb.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, mfb.fbo_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mfb.m_default_rt->getTexId(), 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mfb.m_default_rt->getDepthId(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mfb.m_default_rt->attach(m_framebuffer.fbo_id);

        if (m_option.anti_aliasing) {
            glCreateFramebuffers(1, &m_msaa_fb.fbo_id);
            m_msaa_fb.m_msaa_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY, true);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
        }
        glCreateFramebuffers(1, &m_comp_fb.fbo_id);
        m_comp_fb.m_comp_id_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY, false, COLOR_R32I);

        m_cur_rt = mfb.m_default_rt.get();

        mfb.m_depth_normal_rt = make_shared<GLRenderTexture>(w, h, DEPTH_WRITE_ONLY);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
    }

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}
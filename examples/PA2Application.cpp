#define GLM_FORCE_RADIANS
#include "PA2Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils.hpp"

PA2Application::PA2Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight, "Application for PA2"), m_vao(2), m_program("shaders/simple3d.v.glsl", "shaders/simple3d.f.glsl"), m_currentTime(0)
{
  makeA3dCube();
}

void PA2Application::makeA3dCube()
{
  std::vector<glm::vec3> positions = {{-0.5, -0.5,  0.5}, {0.5, -0.5,  0.5}, {0.5, 0.5,  0.5}, {-0.5, 0.5,  0.5},
                                      {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}};
  std::vector<glm::vec3> colors = {{1, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 1, 0},
                                   {1, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 1, 0}}; // a single yellow color
  std::vector<uint> ibo = {0, 1, 2,     0, 3, 2,   // Front
                           4, 5, 6,     4, 7, 6,   // Back
                           0, 4, 7,     0, 3, 7,   // Left
                           1, 2, 5,     2, 6, 5,   // Right
                           3, 4, 6,     3, 7, 6,   // Top
                           0, 1, 5,     0, 4, 5};  // Bottom
  m_vao.setVBO(0, positions);
  m_vao.setVBO(1, colors);
  m_vao.setIBO(ibo);
}

void PA2Application::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program.bind();

  glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), this->m_currentTime, glm::vec3(1.0f, 1.0f, 0.0f));
  
  glm::mat4 m1 = glm::translate(rotate, glm::vec3(-0.5f, -0.5f, -0.5f));
  glm::mat4 m2 = glm::translate(rotate, glm::vec3(0.5f,   0.5f,  0.5f));

  glm::mat4 v = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
  
  glm::mat4 p = glm::perspective(
    glm::radians(90.0f),
    16.0f / 3.0f,
    0.1f,
    100.0f
  );

  this->m_program.setUniform("mvp", p*v*m1);
  m_vao.draw(); // rendering first instance
  this->m_program.setUniform("mvp", p*v*m2);
  m_vao.draw(); // rendering second instance
  m_program.unbind();
}

void PA2Application::update()
{
  m_currentTime = glfwGetTime();
  m_program.bind();
  m_program.setUniform("time", m_currentTime);
  m_program.unbind();
}

void PA2Application::resize(GLFWwindow *, int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA2Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA2Application::resize);
}

void PA2Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 2";
  synopsis = "pa2";
  description = "  An application on GLSL uniform variables and true 3D meshes.\n";
}

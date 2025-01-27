#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "PA3Application.hpp"
#include "utils.hpp"

PA3Application::PA3Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight, "Application for PA3"), m_program(new Program("shaders/simple3d.v.glsl", "shaders/simple3d.f.glsl")), m_view(1), m_currentTime(0), m_deltaTime(0),
      m_renderMode(GL_TRIANGLES)
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  resize(window, windowWidth, windowHeight);
  initGLState();
  makeASphere(40, 40);
  makeATorus(40, 40, 0.25);
  makeAShell(40, 40);
}

std::shared_ptr<VAO> PA3Application::makeParamSurf(DiscreteLinRange rgPhi, DiscreteLinRange rgTheta, const std::function<glm::vec3(float, float)> & posFunc, bool isCyclicInPhi, bool isCyclicInTheta)
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;
  std::vector<uint> ibo;
  
  for (uint i=0; i < rgPhi.nbVals; i++) {
    for (uint j=0; j < rgTheta.nbVals; j++) {
      float phi   = rgPhi.value(i);
      float theta = rgTheta.value(j);

      positions.push_back(posFunc(phi, theta));
      colors.push_back({0, 100, 0});

      uint ni;
      uint nj;

      if (i <= rgPhi.nbVals-2) {
        ni = (i+1);
      } else if (isCyclicInPhi) {
        ni = 0;
      } else {
        continue;
      }

      if (j <= rgTheta.nbVals-2) {
        nj = (j+1);
      } else if (isCyclicInTheta) {
        nj = 0;
      } else {
        continue;
      }

      if ((i <= rgPhi.nbVals-2) && (j <= rgTheta.nbVals-2)) {
        ibo.push_back( i*rgTheta.nbVals + j);   // i   ; j
        ibo.push_back(ni*rgTheta.nbVals + j);   // i+1 ; j
        ibo.push_back( i*rgTheta.nbVals + nj);  // i   ; j+1

        ibo.push_back( i*rgTheta.nbVals + nj);  // i   ; j+1
        ibo.push_back(ni*rgTheta.nbVals +  j);  // i+1 ; j
        ibo.push_back(ni*rgTheta.nbVals + nj);  // i+1 ; j+1
      }
    }
  }

  std::shared_ptr<VAO> vao(new VAO(2));
  vao->setVBO(0, positions);
  vao->setVBO(1, colors);
  vao->setIBO(ibo);
  return vao;
}

void PA3Application::makeASphere(unsigned int nbPhi, unsigned int nbTheta)
{
  auto posFunc = [](float phi, float theta) { return glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), 1 - cos(theta)); };

  const float pi = glm::pi<float>();
  std::shared_ptr<VAO> vao = makeParamSurf(DiscreteLinRange(nbPhi, 0, 2 * pi), DiscreteLinRange(nbTheta, 0, pi), posFunc, true, false);
  glm::mat4 mw(1);
  mw = glm::scale(mw, {0.2, 0.2, 0.2});
  m_vaos.push_back(RenderObject::createInstance(m_program, vao, mw));
}

void PA3Application::makeATorus(unsigned int nbPhi, unsigned int nbTheta, float smallRadius)
{
  auto posFunc = [&smallRadius](float phi, float theta) { return glm::vec3((1+smallRadius*cos(theta))*cos(phi), (1+smallRadius*cos(theta))*sin(phi), smallRadius*sin(theta)); };
  const float pi = glm::pi<float>();
  
  std::shared_ptr<VAO> vao = makeParamSurf(DiscreteLinRange(nbPhi, 0, 2 * pi), DiscreteLinRange(nbTheta, 0, 2 * pi), posFunc, true, true);

  glm::mat4 mw(1);
  mw = glm::translate(mw, {0.5, 0, 0});
  mw = glm::rotate(mw, 3 * glm::pi<float>() / 4, {1, 0, 1});
  mw = glm::scale(mw, {0.2, 0.2, 0.2});
  m_vaos.push_back(RenderObject::createInstance(m_program, vao, mw));
}

void PA3Application::makeAShell(unsigned int nbPhi, unsigned int nbTheta)
{
  auto posFunc = [&](float theta, float phi) { return glm::vec3(-phi * sin(theta) * sin(theta) * sin(phi), phi * sin(theta) * sin(theta) * cos(phi), phi * sin(theta) * cos(theta)); };

  const float pi = glm::pi<float>();
  std::shared_ptr<VAO> vao = makeParamSurf(DiscreteLinRange(nbPhi, 0, pi), DiscreteLinRange(nbTheta, -pi / 4, 5 * pi / 2), posFunc, false, false);
  glm::mat4 mw(1);
  mw = glm::translate(mw, {-0.5, 0, 0});
  mw = glm::rotate(mw, 3 * glm::pi<float>() / 4, {1, 0, 1});
  mw = glm::scale(mw, {0.05, 0.05, 0.05});
  m_vaos.push_back(RenderObject::createInstance(m_program, vao, mw));
}

void PA3Application::initGLState() const
{
  glClearColor(1, 1, 1, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void PA3Application::renderFrame()
{
  glClear(GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto & vao : m_vaos) {
    vao->draw(m_renderMode);
  }
}

void PA3Application::update()
{
  float prevTime = m_currentTime;
  m_currentTime = glfwGetTime();
  m_deltaTime = m_currentTime - prevTime;
  m_program->bind();
  m_program->setUniform("time", m_currentTime);
  m_program->setUniform("V", m_view);
  m_program->setUniform("P", m_proj);
  m_program->unbind();
  continuousKey();
}

void PA3Application::rotateView(const glm::vec3 & axis, float angleFactor)
{
  const glm::mat4 eye(1);
  const float pi = glm::pi<float>();
  float angle = m_deltaTime * pi;
  m_view = glm::rotate(eye, angleFactor * angle, axis) * m_view;
}

void PA3Application::continuousKey()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glm::mat4 eye(1);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    glm::vec3 right{1, 0, 0};
    rotateView(right, 1);
  } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    glm::vec3 right{1, 0, 0};
    rotateView(right, -1);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    glm::vec3 up{0, 1, 0};
    rotateView(up, 1);
  } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    glm::vec3 up{0, 1, 0};
    rotateView(up, -1);
  }
}

void PA3Application::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  PA3Application & app = *static_cast<PA3Application *>(glfwGetWindowUserPointer(window));
  float aspect = framebufferHeight / float(framebufferWidth);
  if (aspect > 1) {
    app.m_proj = glm::ortho(-1.f, 1.f, -aspect, aspect);
  } else {
    app.m_proj = glm::ortho(-1 / aspect, 1 / aspect, -1.f, 1.f);
  }
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA3Application::keyCallback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*mods*/)
{
  PA3Application & app = *static_cast<PA3Application *>(glfwGetWindowUserPointer(window));
  switch (key) {
  case 'L':
    if (action == GLFW_PRESS) {
      app.m_renderMode = (app.m_renderMode == GL_LINES) ? GL_TRIANGLES : GL_LINES;
    }
    break;
  case 'R':
    app.m_view = glm::mat4(1);
    break;
  }
}

void PA3Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA3Application::resize);
  glfwSetKeyCallback(window, PA3Application::keyCallback);
}

void PA3Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 3";
  synopsis = "pa3";
  description = "  An application for rendering parametric surfaces (sphere, torus, ...) and handling occlusion.\n"
                "  The following key bindings are available to interact with thi application:\n"
                "     L                toggle between solid and wireframe rendering\n"
                "     <up> / <down>    rotate the view around the horizontal axis\n"
                "     <left> / <right> rotate the view around the vertical axis\n"
                "     R                reset the view\n";
}

PA3Application::RenderObject::RenderObject(const std::shared_ptr<Program> & prog, const std::shared_ptr<VAO> & vao, const glm::mat4 & modelWorld) : m_prog(prog), m_vao(vao), m_mw(modelWorld) {}

std::shared_ptr<PA3Application::RenderObject> PA3Application::RenderObject::createInstance(const std::shared_ptr<Program> & prog, const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView)
{
  return std::shared_ptr<RenderObject>(new RenderObject(prog, vao, modelView));
}

void PA3Application::RenderObject::draw(GLenum mode) const
{
  if (m_vao and m_prog) {
    m_prog->bind();
    updateProgram();
    m_vao->draw(mode);
    m_prog->unbind();
  }
}

void PA3Application::RenderObject::updateProgram() const
{
  m_prog->setUniform("M", m_mw);
}

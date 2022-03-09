#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "glApi.hpp"
#include "utils.hpp"

Buffer::Buffer(GLenum target) : m_location(0), m_target(target), m_attributeSize(0)
{
  glGenBuffers(1, &this->m_location);
}

Buffer::~Buffer()
{
  glDeleteBuffers(1, &this->m_location);
}

void Buffer::bind() const
{
  glBindBuffer(this->m_target, this->m_location);
}

void Buffer::unbind() const
{
  glBindBuffer(this->m_target, 0);
}

template <> void Buffer::setData(const std::vector<char> & values)
{
  AttributeProperties<char> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(char)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<unsigned char> & values)
{
  AttributeProperties<unsigned char> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(unsigned char)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<short> & values)
{
  AttributeProperties<short> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(short)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<unsigned short> & values)
{
  AttributeProperties<unsigned short> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(unsigned short)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<int> & values)
{
  AttributeProperties<int> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(int)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<unsigned int> & values)
{
  AttributeProperties<unsigned int> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(unsigned int)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<float> & values)
{
  AttributeProperties<float> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(float)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<double> & values)
{
  AttributeProperties<double> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(double)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<glm::vec2> & values)
{
  AttributeProperties<glm::vec2> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(glm::vec2)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<glm::vec3> & values)
{
  AttributeProperties<glm::vec3> properties;
  
  this->bind();
  glBufferData(this->m_target, sizeof(glm::vec3)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

template <> void Buffer::setData(const std::vector<glm::vec4> & values)
{
  AttributeProperties<glm::vec4> properties;

  this->bind();
  glBufferData(this->m_target, sizeof(glm::vec4)*values.size(), &values[0], GL_STATIC_DRAW);
  this->unbind();

  this->m_attributeCount = values.size();
  this->m_attributeSize  = properties.components;
  this->m_attributeType  = properties.typeEnum;
}

uint Buffer::attributeCount() const
{
  return m_attributeCount;
}

GLenum Buffer::attributeType() const
{
  return m_attributeType;
}

uint Buffer::attributeSize() const
{
  return m_attributeSize;
}

VAO::VAO(uint nbVBO) : m_location(0), m_vbos(nbVBO), m_ibo(GL_ELEMENT_ARRAY_BUFFER)
{
  for (auto & vbo : m_vbos) {
    vbo = std::shared_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER));
  }
  assert(nbVBO <= 16); // You may want to replace 16 by the real hardware limitation
  glGenVertexArrays(1, &this->m_location);
}

VAO::~VAO()
{
  glDeleteVertexArrays(1, &this->m_location);
}

void VAO::bind() const
{
  glBindVertexArray(this->m_location);
}

void VAO::unbind() const
{
  glBindVertexArray(0);
}

void VAO::encapsulateVBO(unsigned int attributeIndex) const
{
  std::shared_ptr<Buffer> vbo = this->m_vbos[attributeIndex];

  this->bind();
  vbo->bind();
  
  glEnableVertexAttribArray(attributeIndex);
  glVertexAttribPointer(attributeIndex, vbo->attributeSize(), vbo->attributeType(), GL_FALSE, 0, nullptr);

  /*
   * glVertexArrayAttribFormat(this->m_location, attributeIndex,
   *                        vbo->attributeCount(), vbo->attributeType(),
   *                         GL_FALSE, vbo->attributeSize());
   */

  vbo->unbind();
  this->unbind();
}

std::shared_ptr<VAO> VAO::makeSlaveVAO() const
{
  unsigned int nbVBO = m_vbos.size();
  std::shared_ptr<VAO> slave(new VAO(nbVBO));
  slave->m_vbos = m_vbos;
  slave->bind();
  for (unsigned int attributeIndex = 0; attributeIndex < nbVBO; attributeIndex++) {
    slave->encapsulateVBO(attributeIndex);
  }
  slave->unbind();
  return slave;
}

void VAO::draw(GLenum mode) const
{
  this->bind();
  glDrawElements(mode, this->m_ibo.attributeCount(), this->m_ibo.attributeType(), nullptr);
  this->unbind();
}

Shader::Shader(GLenum type, const std::string & filename) : m_location(0)
{
  this->m_location = glCreateShader(type);
  std::string shaderContent = fileContent(filename);
  const char *cShader = shaderContent.c_str();

  glShaderSource(this->m_location, 1, &cShader, NULL);
  glCompileShader(this->m_location);

  GLint isCompiled;
  glGetShaderiv(this->m_location, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(this->m_location, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(this->m_location, maxLength, &maxLength, &errorLog[0]);
    
    std::cerr << "ERROR WHEN COMPILING SHADER " << filename << " : " << std::endl;

    for (GLchar c : errorLog) {
      std::cerr << c;
    }

    std::cerr << std::endl;

    exit(EXIT_FAILURE);
  }
}

Shader::~Shader()
{
  glDeleteShader(this->m_location);
}

uint Shader::location() const
{
  return m_location;
}

Program::Program(const std::string & vname, const std::string & fname) : m_location(0), m_vshader(GL_VERTEX_SHADER, vname), m_fshader(GL_FRAGMENT_SHADER, fname)
{
  this->m_location = glCreateProgram();

  glAttachShader(this->m_location, this->m_vshader.location());
  glAttachShader(this->m_location, this->m_fshader.location());

  glLinkProgram(this->m_location);

  glDetachShader(this->m_location, this->m_vshader.location());
  glDetachShader(this->m_location, this->m_fshader.location());
}

Program::~Program()
{
  glDeleteProgram(this->m_location);
}

void Program::bind() const
{
  glUseProgram(this->m_location);
}

void Program::unbind() const
{
  glUseProgram(0);
}

bool Program::getUniformLocation(const std::string & name, int & location) const
{
  location = glGetUniformLocation(this->m_location, name.c_str());
  return location != -1;
}

template <> void Program::uniformDispatcher(int location, const int & val)
{
  glUniform1i(location, val);
}

template <> void Program::uniformDispatcher(int location, const uint & val)
{
  glUniform1ui(location, val);
}

template <> void Program::uniformDispatcher(int location, const float & val)
{
  glUniform1f(location, val);
}

template <> void Program::uniformDispatcher(int location, const glm::vec2 & val)
{
  glUniform2fv(location, 1, glm::value_ptr(val));
}

template <> void Program::uniformDispatcher(int location, const glm::vec3 & val)
{
  glUniform3fv(location, 1, glm::value_ptr(val));
}

template <> void Program::uniformDispatcher(int location, const glm::vec4 & val)
{
  glUniform4fv(location, 1, glm::value_ptr(val));
}

template <> void Program::uniformDispatcher(int location, const glm::mat2 & val)
{
  glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

template <> void Program::uniformDispatcher(int location, const glm::mat3 & val)
{
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

template <> void Program::uniformDispatcher(int location, const glm::mat4 & val)
{
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

bool Program::bound() const
{
  int currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  return m_location == (GLuint)currentProgram;
}

Texture::Texture(GLenum target) : m_location(0), m_target(target)
{
  glGenTextures(1, &this->m_location);
}

Texture::~Texture()
{
  glDeleteTextures(1, &this->m_location);
}

void Texture::bind() const
{
  glBindTexture(this->m_target, this->m_location);
}

void Texture::unbind() const
{
  glBindTexture(this->m_target, 0);
}

template <> void Texture::setData<GLubyte>(const Image<GLubyte> & image, bool mipmaps) const
{
  int format;

  switch (image.channels)
  {
    case 1:
      format = GL_R;
      break;

    case 2:
      format = GL_RG;
      break;

    case 3:
      format = GL_RGB;
      break;
    
    case 4:
    default:
      format = GL_RGBA;
      break;
  }

  this->bind();
  switch (this->m_target) {
    case GL_TEXTURE_1D:
      glTexImage1D(this->m_target, 0, format, image.width, 0, format, GL_UNSIGNED_INT, image.data);
      break;
      
    case GL_TEXTURE_2D:
      glTexImage2D(this->m_target, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
      break;

    case GL_TEXTURE_3D:
    default:
      glTexImage3D(this->m_target, 0, format, image.width, image.height, image.depth, 0, format, GL_UNSIGNED_BYTE, image.data);
      break;
  }

  if (mipmaps) {
    glGenerateMipmap(this->m_target);
    glGenerateTextureMipmap(this->m_location);
  }

  this->unbind();
}

Sampler::Sampler(int texUnit) : m_location(0), m_texUnit(texUnit)
{
  glGenSamplers(1, &this->m_location);
}

Sampler::~Sampler()
{
  glDeleteSamplers(1, &this->m_location);
}

void Sampler::bind() const
{
  glBindSampler(this->m_texUnit, this->m_location);
}

void Sampler::unbind() const
{
  glBindSampler(this->m_texUnit, 0);
}

void Sampler::attachToProgram(const Program & prog, const std::string & samplerName, BindOption bindOption) const
{
  if (bindOption == BindOption::BindUnbind) {
    prog.bind();
  }
  
  prog.setUniform(samplerName, this->m_texUnit);

  if (bindOption == BindOption::BindUnbind) {
    prog.unbind();
  }
}

void Sampler::attachTexture(const Texture & texture) const
{
  glActiveTexture(GL_TEXTURE0+this->m_texUnit);
  texture.bind();
  glActiveTexture(GL_TEXTURE0);
}

template <> void Sampler::setParameter<int>(GLenum paramName, const int & value) const
{
  glSamplerParameteri(this->m_location, paramName, value);
}

template <> void Sampler::setParameter<float>(GLenum paramName, const float & value) const
{
  glSamplerParameterf(this->m_location, paramName, value);
}

void Sampler::enableAnisotropicFiltering() const
{
  this->setParameter(GL_TEXTURE_MAX_ANISOTROPY, 2.0f);
}

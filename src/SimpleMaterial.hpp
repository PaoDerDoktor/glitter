#ifndef __SIMPLE_MATERIAL_H__
#define __SIMPLE_MATERIAL_H__

#include <glm/glm.hpp>
#include <string>

struct SimpleMaterial {
  std::string name;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  std::string diffuseTexName;
  std::string normalTexName;
};
#endif // !defined(__SIMPLE_MATERIAL_H__)

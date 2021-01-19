#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Object
{
public:
	glm::vec3 position;
	float rotation;
	glm::vec3 scale;

	Object()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = 0;
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

};

#endif // !OBJECT_H

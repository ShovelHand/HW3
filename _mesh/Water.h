#pragma once
#include "icg_common.h"

class Water
{
public:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo_vpointWater; ///< memory buffer

	GLuint _tex_water; ///< Texture ID

	std::vector<vec3> vertices;
	std::vector<vec3> triangle_vec; //defines the order in which vertices are used in the triangle strips

	void MakeVertices(int width, int height)
	{

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				vertices.push_back(vec3(float(i), 0.0, float(j)));
		}

		//triangle strip
		for (int j = 0; j < (height - 1); ++j)
		{
			//sqaures inside a triangle strip
			for (int i = 0; i < (width - 1); ++i)
			{
				int topleft = j * width + i;
				int topright = topleft + 1;
				int bottomleft = (j + 1) * width + i;
				int bottomright = bottomleft + 1;
				//the upper left triangle
				triangle_vec.push_back(vertices[topleft]);
				triangle_vec.push_back(vertices[topright]);
				triangle_vec.push_back(vertices[bottomleft]);
				//the lower right triangle
				triangle_vec.push_back(vertices[topright]);
				triangle_vec.push_back(vertices[bottomright]);
				triangle_vec.push_back(vertices[bottomleft]);
			}
		}
	}

	GLuint getProgramID(){
		return _pid;
	}

	void init(int width, int height)
	{
		//compile shaders
		_pid = opengp::load_shaders("C:/icg/lecture_viewing3d/_mesh/Water_vshader.glsl",
			"C:/icg/lecture_viewing3d/_mesh/Water_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		///--- Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		MakeVertices(width, height);

		///--- Buffer
		glGenBuffers(1, &_vbo_vpointWater);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpointWater);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*triangle_vec.size(), &triangle_vec[0], GL_STATIC_DRAW);

		///--- Attribute
		GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
		glEnableVertexAttribArray(vpoint_id);
		glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

		///--- Load texture water
		glGenTextures(1, &_tex_water);
		glBindTexture(GL_TEXTURE_2D, _tex_water);
		glfwLoadTexture2D("_mesh/water.tga", 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glUniform1i(glGetUniformLocation(_pid, "tex_water"), 5 /*GL_TEXTURE5*/);

		///--- to avoid the current object being polluted
		glBindVertexArray(0);
		glUseProgram(0);

	}

	void draw()
	{
		glUseProgram(_pid);
		glBindVertexArray(_vao);
		check_error_gl();

		glBindTexture(GL_TEXTURE_2D, _tex_water);
		glActiveTexture(GL_TEXTURE5);

		glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime()); //will be used for water ripples.
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		glEnable(GL_BLEND);
		glDrawArrays(GL_TRIANGLES, 0, triangle_vec.size());  //uncomment to see the mesh drawn as triangle strips
		glDisable(GL_BLEND);
		GLint vpoint_id = glGetAttribLocation(_pid, "_vbo_vpointWater");
		if (vpoint_id >= 0)
			glDisableVertexAttribArray(vpoint_id);

		glUseProgram(0);
		glBindVertexArray(0);
	}

};
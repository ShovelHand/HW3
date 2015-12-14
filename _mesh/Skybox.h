
#pragma once
#include "icg_common.h"

class Skybox{

private:
	GLuint _vaoSky; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID
	GLuint _vbo_vpoint; ///< memory buffer
	GLuint _vbo_vtexcoord; ///< memory buffer
	GLuint _tex; ///< Texture ID
	GLuint vpoint_id;
public:
	typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
	//make cube around scene such that it encapsulates the terrain mesh
	void init(int width, int depth)
	{
		///--- Compile the shaders
		_pid = opengp::load_shaders("C:/icg/lecture_viewing3d/_mesh/sky_vshader.glsl",
			"C:/icg/lecture_viewing3d/_mesh/sky_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		///--- Vertex one vertex Array
		glGenVertexArrays(1, &_vaoSky);
		glBindVertexArray(_vaoSky);

		///--- Vertex coordinates
		{
			float vpoint[] = {
				-600.0f, 600.0f, -600.0f,
				-600.0f, -600.0f, -600.0f,
				600.0f, -600.0f, -600.0f,
				600.0f, -600.0f, -600.0f,
				600.0f, 600.0f, -600.0f,
				-600.0f, 600.0f, -600.0f,

				-600.0f, -600.0f, 600.0f,
				-600.0f, -600.0f, -600.0f,
				-600.0f, 600.0f, -600.0f,
				-600.0f, 600.0f, -600.0f,
				-600.0f, 600.0f, 600.0f,
				-600.0f, -600.0f, 600.0f,

				600.0f, -600.0f, -600.0f,
				600.0f, -600.0f, 600.0f,
				600.0f, 600.0f, 600.0f,
				600.0f, 600.0f, 600.0f,
				600.0f, 600.0f, -600.0f,
				600.0f, -600.0f, -600.0f,

				-600.0f, -600.0f, 600.0f,
				-600.0f, 600.0f, 600.0f,
				600.0f, 600.0f, 600.0f,
				600.0f, 600.0f, 600.0f,
				600.0f, -600.0f, 600.0f,
				-600.0f, -600.0f, 600.0f,

				-600.0f, 600.0f, -600.0f,
				600.0f, 600.0f, -600.0f,
				600.0f, 600.0f, 600.0f,
				600.0f, 600.0f, 600.0f,
				-600.0f, 600.0f, 600.0f,
				-600.0f, 600.0f, -600.0f,

				-600.0f, -600.0f, -600.0f,
				-600.0f, -600.0f, 600.0f,
				600.0f, -600.0f, -600.0f,
				600.0f, -600.0f, -600.0f,
				-600.0f, -600.0f, 600.0f,
				600.0f, -600.0f, 600.0f
			};
			///--- Buffer
			glGenBuffers(1, &_vbo_vpoint);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpoint);
			glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &vpoint, GL_STATIC_DRAW);

			///--- Attribute
			vpoint_id = glGetAttribLocation(_pid, "vpointSky");
			glEnableVertexAttribArray(vpoint_id);
			//	glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpoint);
			//	glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		}

		///--- Texture coordinates
		{
			const GLfloat vtexcoord[] = {
				// front
				-0.5, 0.5, 1.0,
				0.5, 0.5, 1.0,
				0.5, -0.5, 1.0,
				-0.5, -0.5, 1.0,
				// top
				-0.5, 1.0, -1.0,
				0.0, 1.0, -1.0,
				0.0, 1.0, 1.0,
				-0.5, 1.0, 1.0,
				// back
				0.5, -0.5, -1.0,
				1.0, -0.5, -1.0,
				1.0, 0.5, -1.0,
				0.5, 0.5, -1.0,
				// bottom
				-0.5, 0.5, 1.0,
				0.0, 0.5, 1.0,
				0.0, 0.5, -1.0,
				-0.5, -1.0, -1.0,
				// left
				-1.0, 0.5, -1.0,
				-0.5, 0.5, 1.0,
				-0.5, -0.5, 1.0,
				-1.0, -1.0, -1.0,
				// right
				0.0, 0.5, 1.0,
				0.5, 0.5, -1.0,
				0.5, -0.5, -1.0,
				0.0, -0.5, 1.0, };

			///--- Buffer
			glGenBuffers(1, &_vbo_vtexcoord);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_vtexcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

			///--- Attribute
			GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoordSky");
			glEnableVertexAttribArray(vtexcoord_id);
			glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}
		
		///--- Load texture
		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
		glfwLoadTexture2D("_mesh/skybox.tga", 0);
		GLvoid* data = &_tex;

		for (GLuint i = 0; i < 6; i++)
		{
			
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glUniform1i(glGetUniformLocation(_pid, "tex_sky"), 0 /*GL_TEXTURE0*/);


		///--- to avoid the current object being polluted
		glBindVertexArray(0);
		glUseProgram(0);
		check_error_gl();
	}

	void draw()
	{
		//		glDepthMask(GL_FALSE);
		glUseProgram(_pid);
		glBindVertexArray(_vaoSky);


		///--- Bind textures
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
		glActiveTexture(GL_TEXTURE0);

		//	glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime()); //will be used for water ripples.


		glDrawArrays(GL_TRIANGLES, 0, 36);
		//	glDepthMask(GL_TRUE);
		GLint vpoint_id = glGetAttribLocation(_pid, "_vbo_vpoint");
		if (vpoint_id >= 0)
			glDisableVertexAttribArray(vpoint_id);
		glDisableVertexAttribArray(vpoint_id);

		glUseProgram(0);
		glBindVertexArray(0);
	}


	GLuint getProgramID()
	{
		return _pid;
	}

};
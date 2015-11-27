
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
				-10.0f, 10.0f, -10.0f,
				-10.0f, -10.0f, -10.0f,
				10.0f, -10.0f, -10.0f,
				10.0f, -10.0f, -10.0f,
				10.0f, 10.0f, -10.0f,
				-10.0f, 10.0f, -10.0f,

				-10.0f, -10.0f, 10.0f,
				-10.0f, -10.0f, -10.0f,
				-10.0f, 10.0f, -10.0f,
				-10.0f, 10.0f, -10.0f,
				-10.0f, 10.0f, 10.0f,
				-10.0f, -10.0f, 10.0f,

				10.0f, -10.0f, -10.0f,
				10.0f, -10.0f, 10.0f,
				10.0f, 10.0f, 10.0f,
				10.0f, 10.0f, 10.0f,
				10.0f, 10.0f, -10.0f,
				10.0f, -10.0f, -10.0f,

				-10.0f, -10.0f, 10.0f,
				-10.0f, 10.0f, 10.0f,
				10.0f, 10.0f, 10.0f,
				10.0f, 10.0f, 10.0f,
				10.0f, -10.0f, 10.0f,
				-10.0f, -10.0f, 10.0f,

				-10.0f, 10.0f, -10.0f,
				10.0f, 10.0f, -10.0f,
				10.0f, 10.0f, 10.0f,
				10.0f, 10.0f, 10.0f,
				-10.0f, 10.0f, 10.0f,
				-10.0f, 10.0f, -10.0f,

				-10.0f, -10.0f, -10.0f,
				-10.0f, -10.0f, 10.0f,
				10.0f, -10.0f, -10.0f,
				10.0f, -10.0f, -10.0f,
				-10.0f, -10.0f, 10.0f,
				10.0f, -10.0f, 10.0f
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
			const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f,
				/*V2*/ 1.0f, 0.0f,
				/*V3*/ 0.0f, 1.0f,
				/*V4*/ 1.0f, 1.0f };

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
		glBindTexture(GL_TEXTURE_2D, _tex);
		glfwLoadTexture2D("skybox.tga", 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glUniform1i(glGetUniformLocation(_pid, "skyTex"), 0 /*GL_TEXTURE0*/);
	

		///--- to avoid the current object being polluted
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void draw()
	{
		glDepthMask(GL_FALSE);
		glUseProgram(_pid);
		glBindVertexArray(_vaoSky);
		check_error_gl();


		///--- Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
		
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
	//	glDepthMask(GL_TRUE);
		
		glDisableVertexAttribArray(vpoint_id);
		glUseProgram(0);
		glBindVertexArray(0);
	}


	GLuint getProgramID()
	{
		return _pid;
	}

};
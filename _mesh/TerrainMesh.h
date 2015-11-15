#pragma once
#include "icg_common.h"

#include "OpenGLImage/EigenVisualizer.h"
using namespace EigenVisualizer;

class TerrainMesh{
private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo_vpointTerrain; ///< memory buffer
	GLuint _vbo_vtexcoord; ///< memory buffer
	GLuint _tex_day; ///< Texture ID
	GLuint _tex_night; ///< Texture ID

	opengp::Surface_mesh mesh;
	GLuint _vnormal;   ///< memory buffer
	
	std::vector<vec3> vertices;
	std::vector<vec3> triangle_vec; //defines the order in which vertices are used in the triangle strips

	/*Perlin noise helper functions*/
float mix(float x, float y, float alpha)
{
	return y * alpha + x * (1.0f - alpha);
}

float f(float t)
{
	float t_3 = t * t * t;
	return 6 * t * t * t_3 - 15 * t * t_3 + 10 * t_3;
}

//return a random floating point number between [0, 1]
float rand0_1()
{
	return ((float)std::rand()) / ((float)RAND_MAX);
}
void buildMeshVertices(int width, int height)
{
	RGBImage base(width, height);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			base(i, j) = vec3(i / float(width), j / float(height), 0);
		}
	}

	//set vertex heights using Perlin noise
	int period = 4;
	float frequency = 1.0f / period;
	std::srand(1);
	vec3 randGradientVec;

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			//we use vec3 instead of vec2 but set the last component to zero
			randGradientVec.x() = cos(2 * M_PI * rand0_1());
			randGradientVec.y() = sin(2 * M_PI * rand0_1());
			randGradientVec.z() = 0;
			base(i, j) = randGradientVec;
		}
	//set terrain mesh vertices
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int left = (i / period) * period;
			int right = (left + period) % width;
			float dx = (i - left) * frequency;

			int top = (j / period) * period;
			int bottom = (top + period) % height;
			float dy = (j - top) * frequency;

			vec2 a(dx, -dy);
			vec2 b(dx - 1, -dy);
			vec2 c(dx - 1, 1 - dy);
			vec2 d(dx, 1 - dy);


			vec3 topleft = base(left, top);
			float s = topleft(0) * a(0) + topleft(1) * a(1);
			vec3 topright = base(right, top);
			float t = topright(0) * b(0) + topright(1) * b(1);
			vec3 bottomleft = base(left, bottom);
			float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
			vec3 bottomright = base(right, bottom);
			float v = bottomright(0) * c(0) + bottomright(1) * c(1);

			float fx = f(dx);
			float fy = f(dy);

			float st = mix(s, t, fx);
			float uv = mix(u, v, fx);
			float noise = mix(st, uv, fy);

			base(i, j) = vec3(noise, noise, noise);

			vertices.push_back(vec3(float(i), noise, float(j))); //once working in 3d, will probably transpose y and z
		}
	//	showImage(image);

	for (std::vector<vec3>::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		(*itr).x() -= 3.0;
		(*itr).z() -= 3.0;
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
public:
GLuint getProgramID(){
	return _pid;
}

void init(int width, int height)
{
	//compile shaders
	_pid = opengp::load_shaders("C:/icg/lecture_viewing3d/_mesh/Terrain_vshader.glsl",
		"C:/icg/lecture_viewing3d/_mesh/Terrain_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	///--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	buildMeshVertices(width, height);

	///--- Buffer
	glGenBuffers(1, &_vbo_vpointTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpointTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*triangle_vec.size(), &triangle_vec[0], GL_STATIC_DRAW);

	///--- Normal Buffer

	///--- Attribute
	GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
	glEnableVertexAttribArray(vpoint_id);
	glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);


	///--- Texture coordinates
	

	///--- Load texture
	glGenTextures(1, &_tex_day);
	glBindTexture(GL_TEXTURE_2D, _tex_day);
	glfwLoadTexture2D("_mesh/plane.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_day"), 0 /*GL_TEXTURE0*/);

	///--- Load texture
	glGenTextures(1, &_tex_night);
	glBindTexture(GL_TEXTURE_2D, _tex_night);
	glfwLoadTexture2D("_mesh/night.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_night"), 1 /*GL_TEXTURE1*/);


	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
}

void cleanup()
{
	glBindVertexArray(0);
	glUseProgram(0);
	glDeleteBuffers(1, &_vbo_vpointTerrain);
	glDeleteBuffers(1, &_vbo_vtexcoord);
	glDeleteProgram(_pid);
	glDeleteVertexArrays(1, &_vao);
//	glDeleteTextures(1, &_tex);
}

void draw()
{
	bindShader();
	///--- Bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _tex_day);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tex_night);

	glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());

	glDrawArrays(GL_TRIANGLES, 0, triangle_vec.size());  //uncomment to see the mesh drawn as triangle strips
	
//	glDrawArraysInstanced(GL_LINE_LOOP, 0, triangle_vec.size() -1, 3);  //uncomment to see the mesh in wireframe

	 unbindShader();
}
private:
	void bindShader()
	{
		glUseProgram(_pid);
		glBindVertexArray(_vao);
		check_error_gl();

		/////--- Vertex Attribute ID for Positions
		GLint vpoint_id = glGetAttribLocation(_pid, "_vbo_vpointTerrain");
		if (vpoint_id >= 0) {
			glEnableVertexAttribArray(vpoint_id);
			check_error_gl();
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpointTerrain);
			glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
			check_error_gl();
		}

		/////--- Vertex Attribute ID for Normals
		GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
		if (vnormal_id >= 0) {
			glEnableVertexAttribArray(vnormal_id);
			glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
			glVertexAttribPointer(vnormal_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
			check_error_gl();
		}
	}

	void unbindShader() {
		GLint vpoint_id = glGetAttribLocation(_pid, "_vbo_vpointTerrain");
		if (vpoint_id >= 0)
			glDisableVertexAttribArray(vpoint_id);
		GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
//		if (vnormal_id >= 0)
		//	glDisableVertexAttribArray(vnormal_id);
		glUseProgram(0);
		glBindVertexArray(0);
	}
};
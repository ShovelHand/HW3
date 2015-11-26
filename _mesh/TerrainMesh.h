#pragma once
#include "icg_common.h"

#include "OpenGLImage/EigenVisualizer.h"
#include <math.h>

using namespace EigenVisualizer;

typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
#define MAX_OCTAVES 16
#define FBM_SIZE 512

class TerrainMesh{
private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo_vpointTerrain; ///< memory buffer
	
	GLuint _vbo_vtexcoord; ///< memory buffer
	GLuint _tex_grass; ///< Texture ID
	GLuint _tex_rock; ///< Texture ID
	GLuint _tex_sand; ///< Texture ID
	GLuint _tex_snow; ///< Texture ID
	GLuint _tex_water; ///< Texture ID
	GLuint _tex_uvDebug; ///< Texture ID
	GLuint _tex_heightMap;

	opengp::Surface_mesh mesh;
	GLuint _vnormal;   ///< memory buffer
	
	std::vector<vec3> vertices;
	std::vector<vec3> triangle_vec; //defines the order in which vertices are used in the triangle strips
	std::vector<vec2> vtexcoord;
//	std::vector<float> noiseVec;
	float noiseArray[600][600];
	/*Perlin noise helper functions*/
float mix(float x, float y, float alpha)
{
	return y * alpha + x * (1.0f - alpha);
}

float f(float t)
{
	return 6 * pow(t,5) - 15 * pow(t,4) + 10 * pow(t,3);
}

//return a random floating point number between [0, 1]
float rand0_1()
{
	return ((float)std::rand()) / ((float)RAND_MAX);
}


//Make the perlin noise image to upload as texture
RGBImage BuildNoiseImage(int width, int height)
{
	RGBImage base(width, height);
	//set vertex heights using Perlin noise
	int period = 64;
	float frequency = 1.0f / period;
	std::srand(19);
	vec3 randGradientVec;

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			//we use vec3 instead of vec2 but set the last component to zero
			randGradientVec.x() = cos(2 * M_PI * rand0_1());
			randGradientVec.y() = sin(2 * M_PI * rand0_1());
			randGradientVec.z() = 0;
			base(i, j) = randGradientVec; //make base noise image
		}

	for (int j = 0; j < width; ++j)
	{
		for (int i = 0; i < height; ++i)
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

			//		base(i,j) = vec3(noise, noise, noise);
			noiseArray[i][j] = noise;
		}
	}
	for (int i = 0; i < base.rows(); i++)
		for (int j = 0; j < base.cols(); j++)
		{
			float value = fBm(vec3(j, i,0), 0.6, 2, 16);
			base(j, i) = vec3(value, value, value);
		}
	


	return base;
}

float fBm(vec3 point, float H, int lacunarity, int octaves)
{
	float value, frequency, remainder; 
	int i;
	static bool first = true;
	static float exp_array[MAX_OCTAVES]; 

	if (first)
	{
		frequency = 0.2;

		for (i = 0; i< MAX_OCTAVES; i++) 
		{ /* compute weight for each frequency */
			exp_array[i] = pow(frequency, -H);
			frequency *= lacunarity;
		}

		first = false;
	}
	value = 0.0;

	/* inner loop of spectral construction */ 
	for (i = 0; i < octaves; i++)
	{
		value += noiseArray[int(point.x()) % FBM_SIZE][int(point.y()) % FBM_SIZE] * exp_array[i];
		point.x() *= lacunarity; point.y() *= lacunarity;// point.z() *= lacunarity;
	} /* for */

	remainder = octaves - (int)octaves;
	if (remainder) /* add in "octaves" remainder */ /* "i" and spatial freq. are preset in loop above */
		value += remainder * noiseArray[int(point.x()) / FBM_SIZE][int(point.y()) / FBM_SIZE] * exp_array[i];

	return value;
}

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

	//make the pixel coordinates for the height map texture
	for (std::vector<vec3>::iterator itr = triangle_vec.begin(); itr != triangle_vec.end(); ++itr)
		vtexcoord.push_back(vec2((*itr).x()/float(width), (*itr).z()/float(height)));

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
	
	MakeVertices(width, height);

	///--- Buffer
	glGenBuffers(1, &_vbo_vpointTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpointTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*triangle_vec.size(), &triangle_vec[0], GL_STATIC_DRAW);

	///--- Attribute
	GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
	glEnableVertexAttribArray(vpoint_id);
	glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

	glUniform1i(glGetUniformLocation(_pid, "width"), width);
	glUniform1i(glGetUniformLocation(_pid, "depth"), height);

	///--- Load texture grass
	glGenTextures(1, &_tex_grass);
	glBindTexture(GL_TEXTURE_2D, _tex_grass);
	glfwLoadTexture2D("_mesh/grass.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_grass"), 0 /*GL_TEXTURE0*/);

	///--- Load texture rock
	glGenTextures(1, &_tex_rock);
	glBindTexture(GL_TEXTURE_2D, _tex_rock);
	glfwLoadTexture2D("_mesh/rock.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_rock"), 1 /*GL_TEXTURE1*/);

	///--- Load texture debug
	glGenTextures(1, &_tex_uvDebug);
	glBindTexture(GL_TEXTURE_2D, _tex_uvDebug);
	glfwLoadTexture2D("_mesh/quad_texture.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_debug"), 2 /*GL_TEXTURE1*/);

	///--- Load texture sand
	glGenTextures(1, &_tex_sand);
	glBindTexture(GL_TEXTURE_2D, _tex_sand);
	glfwLoadTexture2D("_mesh/sand.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_sand"), 3 /*GL_TEXTURE3*/);

	///--- Load texture snow
	glGenTextures(1, &_tex_snow);
	glBindTexture(GL_TEXTURE_2D, _tex_snow);
	glfwLoadTexture2D("_mesh/snow.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_snow"), 4 /*GL_TEXTURE4*/);

	///--- Load texture water
	glGenTextures(1, &_tex_water);
	glBindTexture(GL_TEXTURE_2D, _tex_water);
	glfwLoadTexture2D("_mesh/water.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_pid, "tex_water"), 5 /*GL_TEXTURE5*/);

	RGBImage Noise = BuildNoiseImage(FBM_SIZE, FBM_SIZE); //lets give this some different dimensions

	//vertex buffer for heightmap texture
	{
		///--- Buffer
		glGenBuffers(1, &_vbo_vtexcoord);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_vtexcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*vtexcoord.size(), &vtexcoord[0], GL_STATIC_DRAW);

		///--- Attribute
		GLuint vtexcoord_id = glGetAttribLocation(_pid, "TexCoord");
		glEnableVertexAttribArray(vtexcoord_id);
		glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}
	///--- Load texture heightmap
	glGenTextures(1, &_tex_heightMap);
	
	glUniform1i(glGetUniformLocation(_pid, "tex_height"), 6 /*GL_TEXTURE6*/);
	glBindTexture(GL_TEXTURE_2D, _tex_heightMap);
//	glfwLoadTexture2D("_mesh/texture.tga", 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Noise.cols(),
		Noise.rows(), 0, GL_RGB, GL_FLOAT,
		Noise.data());
	
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
	glBindTexture(GL_TEXTURE_2D, _tex_grass);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tex_rock);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _tex_uvDebug);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _tex_sand);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _tex_snow);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _tex_water);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, _tex_heightMap);

	glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());


	///--- Upload transformation
	Transform M = Transform::Identity();
	//could scale the mesh to make it look less blocky.
//	M *= Eigen::AlignedScaling3f(0.1, 0.1, 0.1);
	GLuint M_id = glGetUniformLocation(_pid, "M");
	glUniformMatrix4fv(M_id, ONE, DONT_TRANSPOSE, M.data());
	

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
			glEnableVertexAttribArray(_vbo_vpointTerrain);
			check_error_gl();
			glGenBuffers(1, &_vbo_vpointTerrain);
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
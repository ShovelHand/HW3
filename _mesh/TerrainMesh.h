#pragma once
#include "icg_common.h"


class TerrainMesh{
private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo_vpointTerrain; ///< memory buffer
	GLuint _vbo_vtexcoord; ///< memory buffer
	GLuint _tex; ///< Texture ID
	
	std::vector<vec3> vertices;
	std::vector<vec3> triangle_vec; //defines the order in which vertices are used in the triangle strips

public:

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

	//loops for vertices and indices go here
	for (float j = float(height); j > 0.0; j -= 1)
	{
		for (float i = float(width); i > 0.0; i -= 1)
		{
			vertices.push_back(vec3(float(i) , float(j) , 0.0)); //once working in 3d, will probably transpose y and z

		}
	}

	for (std::vector<vec3>::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		(*itr).x() -= 8.0;
		(*itr).y() -= 8.0;
	}

	//triangle strip
	for	(int j = 0; j < (height - 1); ++j)
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
	
	///--- Buffer
	glGenBuffers(1, &_vbo_vpointTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_vpointTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*triangle_vec.size(), &triangle_vec[0], GL_STATIC_DRAW);


	///--- Attribute
	GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
	glEnableVertexAttribArray(vpoint_id);
	glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);


	///--- Texture coordinates


	///--- Load texture


	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
}

void cleanup()
{
	
}

void draw()
{
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	///--- Bind textures
	//     glActiveTexture(GL_TEXTURE0);
	//     glBindTexture(GL_TEXTURE_2D, _tex);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, 2);
//	glDrawArrays(GL_TRIANGLES, 0,  indices.size());
	
	glDrawArraysInstanced(GL_LINE_LOOP, 0, triangle_vec.size(), 4);
	/*
	for (int i = 0; i < indices.size(); i++)
	{
		glDrawArraysInstanced(GL_LINE_LOOP, i, 3, indices.size());
	}
	*/

	 glBindVertexArray(0);
	 glUseProgram(0);

}
};
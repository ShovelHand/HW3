#pragma once
#include "icg_common.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

class Mesh{
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _tex_day; ///< Texture ID
    GLuint _tex_night; ///< Texture ID

    opengp::Surface_mesh mesh;    
    GLuint _vpoint;    ///< memory buffer
    GLuint _vnormal;   ///< memory buffer

public:    
    void init(){
 //       const std::string& filename = "_mesh/sphere.obj";
        const std::string& filename = "C:\\icg\\lecture_viewing3d\\_mesh\\bunny.obj";
        check_error_gl();
        bool success = mesh.read(filename.c_str());
        assert(success);
        mesh.triangulate();
        mesh.update_vertex_normals();
        printf("Loaded mesh '%s' (#V=%d, #F=%d)\n", filename.c_str(), mesh.n_vertices(), mesh.n_faces());
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        check_error_gl();                      
        
        ///--- Vertex Buffer
        Surface_mesh::Vertex_property<Point> vpoints = mesh.get_vertex_property<Point>("v:point");
        glGenBuffers(ONE, &_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
        glBufferData(GL_ARRAY_BUFFER, mesh.n_vertices() * sizeof(vec3), vpoints.data(), GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Normal Buffer
        Surface_mesh::Vertex_property<Normal> vnormals = mesh.get_vertex_property<Normal>("v:normal");
        glGenBuffers(ONE, &_vnormal);
        glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
        glBufferData(GL_ARRAY_BUFFER, mesh.n_vertices() * sizeof(vec3), vnormals.data(), GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Index Buffer
        std::vector<unsigned int> indices;
        for(Surface_mesh::Face_iterator fit = mesh.faces_begin(); fit != mesh.faces_end(); ++fit) {
            unsigned int n = mesh.valence(*fit);
            Surface_mesh::Vertex_around_face_circulator vit = mesh.vertices(*fit);
            for(unsigned int v = 0; v < n; ++v) {
                indices.push_back((*vit).idx());
                ++vit;
            }
        }

        GLuint _vbo_indices;
        glGenBuffers(ONE, &_vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        check_error_gl();        

        ///--- Compile the shaders
        _pid = opengp::load_shaders("C:\\icg\\lecture_viewing3d\\_mesh\\Mesh_vshader.glsl", 
			"C:\\icg\\lecture_viewing3d\\_mesh\\Mesh_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);
        
        ///--- Load texture
        glGenTextures(1, &_tex_day);
        glBindTexture(GL_TEXTURE_2D, _tex_day);
        glfwLoadTexture2D("_mesh/day.tga", 0);
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
    }
           
    void cleanup(){
        /// TODO
    }

    void draw(){
        bindShader();  
            ///--- Setup the texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex_day);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _tex_night);
        
            glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());
            // glDrawElements(GL_TRIANGLES, /*#vertices*/ 3*mesh.n_faces(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            glDrawElementsInstanced(GL_TRIANGLES, /*#vertices*/ 3*mesh.n_faces(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET,2 /*#instances*/);
        unbindShader();
    }
    
private:
    void bindShader() {
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Positions
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0) {
            glEnableVertexAttribArray(vpoint_id);
            check_error_gl();
            glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
            glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        ///--- Vertex Attribute ID for Normals
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0) {
            glEnableVertexAttribArray(vnormal_id);
            glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
            glVertexAttribPointer(vnormal_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }
    }

    void unbindShader() {
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0)
            glDisableVertexAttribArray(vpoint_id);
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0)
            glDisableVertexAttribArray(vnormal_id);
        glUseProgram(0);
        glBindVertexArray(0);
    }
};

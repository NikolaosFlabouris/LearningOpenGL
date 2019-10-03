#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

///
/// This class controls the creation of shader programs and interfacing with the shaders, such as setting uniforms.
///
class Shader
{
public:

    ///
    /// Default empty constructor.
    ///
    Shader()
    {
    }

    ///
    /// Constructor with vertex, fragment and optional geometry shader file locations.
    ///
    Shader(const char* vertex_file_path,
           const char* fragment_file_path,
           const char* geometry_file_path = nullptr)
        : vertex_file_path_(vertex_file_path),
        fragment_file_path_(fragment_file_path),
        geomety_file_path_(geometry_file_path)
    {
        LoadShaders(vertex_file_path_, fragment_file_path_, geomety_file_path_);
    }

    ///
    /// Reads GLSL shader source from a file, then compiles it and links to create
    /// a shader program ready for use.
    ///
    /// \param vertex_file_path_ - path to vertex shader file.
    /// \param fragment_file_path_ - path to fragment shader file.
    /// \return - the ID of the shader program (assigned by OpenGL) or 0 if error.
    ///
    GLuint LoadShaders(const char* vertex_file_path, 
                       const char* fragment_file_path,
                       const char* geometry_file_path = nullptr)
    {
        // Create the shaders.
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);


        // Compile vertex and fragment shaders. Exit if compile errors.
        if(!CompileShader(vertex_file_path, vertex_shader_id) || !CompileShader(fragment_file_path, fragment_shader_id))
        {
            return 0;
        }

        // If geometry shader path is present, also try and load a geometry shader.
        if(geometry_file_path != nullptr)
        {
            if(!CompileShader(geometry_file_path, geometry_shader_id))
            {
                return 0;
            }
        }

        // Link the program.
        program_id_ = glCreateProgram();
        glAttachShader(program_id_, vertex_shader_id);
        glAttachShader(program_id_, fragment_shader_id);
        if(geometry_file_path != nullptr)
        {
            glAttachShader(program_id_, geometry_shader_id);
        }
        glLinkProgram(program_id_);

        // Check the program.
        GLint result = GL_FALSE;
        int info_log_length;

        glGetProgramiv(program_id_, GL_LINK_STATUS, &result);
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &info_log_length);
        if(info_log_length > 0)
        {
            std::vector<char> program_error_message(info_log_length + ( long long) 1);
            glGetProgramInfoLog(program_id_, info_log_length, NULL, &program_error_message[0]);
            std::cerr << &program_error_message[0] << std::endl;
        }

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
        if(geometry_file_path != nullptr)
        {
            glDeleteShader(geometry_shader_id);
        }

        return program_id_;
    }

    ///
    /// Returns path to the vertex shader.
    ///
    const char* VertexFilePath()
    {
        return vertex_file_path_;
    }

    ///
    /// Returns path to the fragment shader.
    ///
    const char* GeometryFilePath()
    {
        return geomety_file_path_;
    }

    ///
    /// Returns path to the geometry shader.
    ///
    const char* FragmentFilePath()
    {
        return fragment_file_path_;
    }

    ///
    /// Returns the shader program id.
    ///
    GLuint ProgramID()
    {
        return program_id_;
    }

    ///
    /// Utility set uniform functions.
    /// \param name - name of uniform in shader.
    /// \params - values to apply to uniform.
    ///
    void SetUniformBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(program_id_, name.c_str()), ( int) value);
    }
    
    void SetUniformInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value);
    }
    
    void SetUniformFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(program_id_, name.c_str()), value);
    }
    
    void SetUniformVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]);
    }
    
    void SetUniformVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(program_id_, name.c_str()), x, y);
    }
    
    void SetUniformVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]);
    }
    
    void SetUniformVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program_id_, name.c_str()), x, y, z);
    }
    
    void SetUniformVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]);
    }
    
    void SetUniformVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(program_id_, name.c_str()), x, y, z, w);
    }
    
    void SetUniformMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void SetUniformMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void SetUniformMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    const char* vertex_file_path_;
    const char* fragment_file_path_;
    const char* geomety_file_path_;
    GLuint program_id_;

    ///
    /// Compiles the shader code and checks for errors.
    ///
    /// \param shader_path - path to the shader.
    /// \param shader_id - shader ID to which the shader is being compiled to.
    /// \return - success or failure.
    ///
    int CompileShader(const char* shader_path, const GLuint shader_id)
    {
        // Read shader code from file.
        std::string shader_code;
        std::ifstream shader_stream(shader_path, std::ios::in);
        if(shader_stream.is_open())
        {
            std::string line = "";
            while(getline(shader_stream, line))
            {
                shader_code += "\n" + line;
            }
            shader_stream.close();
        }
        else
        {
            std::cerr << "Cannot open " << shader_path << ". Are you in the right directory?" << std::endl;
            return 0;
        }

        // Compile Shader.
        char const* source_pointer = shader_code.c_str();
        glShaderSource(shader_id, 1, &source_pointer, NULL);
        glCompileShader(shader_id);

        // Check Shader.
        GLint result = GL_FALSE;
        int info_log_length;

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        printf("compiled shader %d %d\n", result, info_log_length);
        if(info_log_length > 1)
        {
            char* shader_error_message = new char[info_log_length + ( long long) 1];
            glGetShaderInfoLog(shader_id,
                               info_log_length,
                               NULL,
                               &shader_error_message[0]);
            std::cerr << &shader_error_message[0] << std::endl;
            delete shader_error_message;
            return 0;
        }
        return 1;
    }
};
#endif
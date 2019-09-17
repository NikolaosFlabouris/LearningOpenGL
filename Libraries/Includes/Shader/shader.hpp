#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ShaderUtils
{
    int CompileShader(const char* ShaderPath, const GLuint ShaderID)
    {
        // Read shader code from file
        std::string ShaderCode;
        std::ifstream ShaderStream(ShaderPath, std::ios::in);
        if(ShaderStream.is_open())
        {
            std::string Line = "";
            while(getline(ShaderStream, Line))
            {
                ShaderCode += "\n" + Line;
            }
            ShaderStream.close();
        }
        else
        {
            std::cerr << "Cannot open " << ShaderPath << ". Are you in the right directory?" << std::endl;
            return 0;
        }

        // Compile Shader
        char const* SourcePointer = ShaderCode.c_str();
        glShaderSource(ShaderID, 1, &SourcePointer, NULL);
        glCompileShader(ShaderID);

        // Check Shader
        GLint Result = GL_FALSE;
        int InfoLogLength;

        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        printf("compiled shader %d %d\n", Result, InfoLogLength);
        if(InfoLogLength > 1)
        {
            char* ShaderErrorMessage = new char[InfoLogLength + ( long long) 1];
            glGetShaderInfoLog(ShaderID,
                               InfoLogLength,
                               NULL,
                               &ShaderErrorMessage[0]);
            std::cerr << &ShaderErrorMessage[0] << std::endl;
            delete ShaderErrorMessage;
            return 0;
        }
        return 1;
    }

    // /**************************************************
    //  * Simple function to read GLSL shader source from a file,
    //  * Then compile it and link to create a shader program ready for use.
    //  * Returns the ID of the shader program (assigned by OpenGL) or 0 if error.
    // **************************************************/
    GLuint LoadShaders(const char* vertex_file_path,
                       const char* fragment_file_path)
    {
        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Compile both shaders. Exit if compile errors.
        if(!CompileShader(vertex_file_path, VertexShaderID)
           || !CompileShader(fragment_file_path, FragmentShaderID))
        {
            return 0;
        }

        // Link the program
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        GLint Result = GL_FALSE;
        int InfoLogLength;

        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength + ( long long) 1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            std::cerr << &ProgramErrorMessage[0] << std::endl;
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
    }
}

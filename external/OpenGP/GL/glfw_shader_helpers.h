// Copyright (C) 2014 - LGG EPFL
// Copyright (C) 2014 - Andrea Tagliasacchi
// Copyright (C) 2014 - Sofien Bouaziz

#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

namespace opengp{

/// Compiles the vertex, geometry and fragment shaders stored in the given strings
inline GLuint compile_shaders(const char * vshader, 
                       const char * fshader, 
                       const char * gshader = NULL, 
                       const char* tcshader=NULL, 
                       const char* teshader=NULL) {
    const int SHADER_LOAD_FAILED = 0;
    GLint Success = GL_FALSE;
    int InfoLogLength;

    /// Create the Vertex Shader
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

    /// Compile Vertex Shader
    fprintf(stdout, "Compiling Vertex shader: ");
    char const * VertexSourcePointer = vshader;
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    /// Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Success);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(!Success) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "Failed:\n%s\n", &VertexShaderErrorMessage[0]);
        return SHADER_LOAD_FAILED;
    }
    else{
        fprintf(stdout, "Success\n");
    }

    GLuint TCShaderID = 0;
    if(tcshader != NULL) {
        /// Create the Shader
        TCShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);

        /// Compile Shader
        fprintf(stdout, "Compiling Tesselation Control shader: ");
        char const * TCSourcePointer = tcshader;
        glShaderSource(TCShaderID, 1, &TCSourcePointer , NULL);
        glCompileShader(TCShaderID);

        /// Check Shader
        glGetShaderiv(TCShaderID, GL_COMPILE_STATUS, &Success);
        glGetShaderiv(TCShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(!Success) {
            std::vector<char> TCShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(TCShaderID, InfoLogLength, NULL, &TCShaderErrorMessage[0]);
            fprintf(stdout, "Failed:\n%s\n", &TCShaderErrorMessage[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    GLuint TEShaderID = 0;
    if(teshader != NULL) {
        /// Create the Shader
        TEShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

        /// Compile Shader
        fprintf(stdout, "Compiling Tesselation Evaluation shader: ");
        char const * TESourcePointer = teshader;
        glShaderSource(TEShaderID, 1, &TESourcePointer , NULL);
        glCompileShader(TEShaderID);

        /// Check Shader
        glGetShaderiv(TEShaderID, GL_COMPILE_STATUS, &Success);
        glGetShaderiv(TEShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(!Success) {
            std::vector<char> TEShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(TEShaderID, InfoLogLength, NULL, &TEShaderErrorMessage[0]);
            fprintf(stdout, "Failed:\n%s\n", &TEShaderErrorMessage[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    GLuint GeometryShaderID = 0;
    if(gshader != NULL) {
        /// Create the Geometry Shader
        GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

        /// Compile Geometry Shader
        fprintf(stdout, "Compiling Geometry shader: ");
        char const * GeometrySourcePointer = gshader;
        glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer , NULL);
        glCompileShader(GeometryShaderID);

        /// Check Geometry Shader
        glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Success);
        glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(!Success) {
            std::vector<char> GeometryShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
            fprintf(stdout, "Failed:\n%s\n", &GeometryShaderErrorMessage[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    /// Create the Fragment Shader
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    /// Compile Fragment Shader
    fprintf(stdout, "Compiling Fragment shader: ");
    char const * FragmentSourcePointer = fshader;
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    /// Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Success);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(!Success) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "Failed:\n%s\n", &FragmentShaderErrorMessage[0]);
        return SHADER_LOAD_FAILED;
    }
    else
        fprintf(stdout, "Success\n");


    /// Link the program
    fprintf(stdout, "Linking shader program: ");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    if(tcshader != NULL) glAttachShader(ProgramID, TCShaderID);
    if(teshader != NULL) glAttachShader(ProgramID, TEShaderID);
    if(gshader  != NULL) glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    /// Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Success);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    if(!Success) {
        fprintf(stdout, "Failed:\n%s\n", &ProgramErrorMessage[0]);
        return SHADER_LOAD_FAILED;
    }
    else
        fprintf(stdout, "Success\n");

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    if(gshader != NULL) glDeleteShader(GeometryShaderID);

    /// make sure you see the text in terminal
    fflush(stdout);

    return ProgramID;
}


/// Compiles the vertex, geometry and fragment shaders using file path
inline GLuint load_shaders(const char * vertex_file_path, const char * fragment_file_path, const char * geometry_file_path = NULL) {
    const int SHADER_LOAD_FAILED = 0; 

    std::string VertexShaderCode, FragmentShaderCode, GeometryShaderCode;
    {
        /// Read the Vertex Shader code from the file
        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if(VertexShaderStream.is_open()) {
            VertexShaderCode = std::string(std::istreambuf_iterator<char>(VertexShaderStream),
                                           std::istreambuf_iterator<char>());
            VertexShaderStream.close();
        } else {
            printf("Could not open file: %s\n", vertex_file_path);
            return SHADER_LOAD_FAILED;
        }   
    
        /// Read the Fragment Shader code from the file
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if(FragmentShaderStream.is_open()) {
            FragmentShaderCode = std::string(std::istreambuf_iterator<char>(FragmentShaderStream),
                                             std::istreambuf_iterator<char>());
            FragmentShaderStream.close();
        } else {
            printf("Could not open file: %s\n", fragment_file_path);
            return SHADER_LOAD_FAILED;
        }

        /// Read the Geometry Shader code from the file
        if(geometry_file_path != NULL) {
            std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);
            if(GeometryShaderStream.is_open()) {
                GeometryShaderCode = std::string(std::istreambuf_iterator<char>(GeometryShaderStream),
                                                 std::istreambuf_iterator<char>());
                GeometryShaderStream.close();
            } else {
                printf("Could not open file: %s\n", geometry_file_path);
                return SHADER_LOAD_FAILED;
            }
        }
    }

    /// Compile them
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    char const * GeometrySourcePointer = NULL;
    if(geometry_file_path != NULL) GeometrySourcePointer = GeometryShaderCode.c_str();
    
    int status = compile_shaders(VertexSourcePointer, FragmentSourcePointer, GeometrySourcePointer);
    if(status == SHADER_LOAD_FAILED)
        printf("Failed linking:\n  vshader: %s\n  fshader: %s\n  gshader: %s\n", vertex_file_path, fragment_file_path, geometry_file_path);
    return status;
}

} //< opengp::

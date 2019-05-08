//
// Created by ounols on 19. 4. 30.
//

#pragma once

#include "../GLProgramHandle.h"

class ShaderUtil {
public:
    ShaderUtil();
    ~ShaderUtil();

    static GLProgramHandle* CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource);

    static GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource);
    static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);
    static GLuint loadShader(GLenum shaderType, const char *pSource);

    static std::map<std::string, std::string> GetImportantVariables(const GLchar* source);
    static void BindVariables(std::map<std::string, std::string> variables, GLProgramHandle* handle);
};


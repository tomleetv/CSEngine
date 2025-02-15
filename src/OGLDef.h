#pragma once
#ifdef _WIN32
//windows include

#include <gl/glew.h>
#include <iostream>


#elif __ANDROID__
//android include

#include <jni.h>
#include <android/log.h>


#include <GLES3/gl3.h>
#include <EGL/egl.h>

#elif __linux__

#include <GL/glew.h>
//#include <GLES2/gl2.h>
//#include <GLES3/gl3.h>
#include <iostream>

#elif __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

#elif __APPLE_CC__

#include <glad/glad.h>
#include <iostream>

#endif

#include "Util/SafeLog.h"
#include <string>

static void checkGlError(const char *op) {
    int error = glGetError();
    if (error != GL_NO_ERROR) {
#ifdef __ANDROID__

#else
        SafeLog::Log((std::string(op) + ": glError " + std::to_string(error)).c_str());
        std::cout << op << ": glError " << error << '\n';
#endif
    }
}

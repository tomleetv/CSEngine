#include "../Util/Matrix.h"
#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/AssetsDef.h"
#include "CameraMgr.h"

#ifdef __linux__

#include <iostream>

#endif

using namespace CSE;

OGLMgr::OGLMgr() : m_projectionRatio(-1), m_programId(-1) {

    m_vertexShader = OpenAssetsTxtFile(AssetsPath() + "Default.vert");
    m_fragmentShader = OpenAssetsTxtFile(AssetsPath() + "Default.frag");

}


OGLMgr::~OGLMgr() {
    releaseBuffers();
}


void OGLMgr::setShaderProgram(int id = HANDLE_NULL) {

    GLProgramHandle* gProgramhandle;

    if (id == HANDLE_NULL) {
        //기본값 쉐이더 적용
        gProgramhandle = new GLProgramHandle();
        m_programId = RESMGR->GetSize<ShaderProgramContainer, GLProgramHandle>() - 1;

        gProgramhandle->Program = createProgram(m_vertexShader.c_str(), m_fragmentShader.c_str());
        if (!gProgramhandle->Program) {
            return;
        }
    } else {
        //커스텀 쉐이더 적용
        gProgramhandle = RESMGR->getShaderProgramHandle(id);
        m_programId = id;
    }

    //const auto m_program = gProgramhandle->Program;

    AttachProgramHandle(m_programId);
}


void OGLMgr::setBuffers() {

    //렌더버퍼 생성
    glGenRenderbuffers(1, &m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

    //깊이버퍼 생성 및 바인딩(옮김)
    //glGenRenderbuffers(1, &m_depthRenderbuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);

    //프레임버퍼 생성
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_depthRenderbuffer);

    // 렌더버퍼 바인딩
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);

#ifndef PLATFORM_IOS
    //프레임버퍼 바인딩
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}


void OGLMgr::setupEGLGraphics(GLuint width, GLuint height) {

    setBuffers();
    setShaderProgram();
    setProjectionRatio();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    //glDepthRangef(0.0,1.0);
}


GLuint OGLMgr::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {


    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();

    if (program) {

        //쉐이더를 attach 합니다.
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {

            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

            if (bufLength) {

                auto buf = static_cast<char*>(malloc(bufLength));

                if (buf) {

                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    std::cout << "Could not link program:\n" << buf << '\n';
                    //LOGE("Could not link program:\n%s\n", buf);
                    free(buf);

                }
            }

            glDeleteProgram(program);
            program = 0;

        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, pixelShader);

    }

    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);

    return program;

}


GLuint OGLMgr::createProgramfromFile(const GLchar* vertexPath, const GLchar* fragmentPath) {

    return 0;
}


void OGLMgr::AttachProgramHandle(int shaderID) {

    GLProgramHandle* gProgramhandle;
    GLuint program;

    gProgramhandle = RESMGR->getShaderProgramHandle(shaderID);
    program = gProgramhandle->Program;

    glUseProgram(program);

    gProgramhandle->Attributes.Position = glGetAttribLocation(program, "a_position");
    gProgramhandle->Attributes.Normal = glGetAttribLocation(program, "a_normal");
    gProgramhandle->Attributes.DiffuseMaterial = glGetAttribLocation(program, "a_diffuseMaterial");
    gProgramhandle->Attributes.TextureCoord = glGetAttribLocation(program, "a_textureCoordIn");
    gProgramhandle->Attributes.Weight = glGetAttribLocation(program, "a_weights");
    gProgramhandle->Attributes.JointId = glGetAttribLocation(program, "a_jointIndices");


    gProgramhandle->Uniforms.Projection = glGetUniformLocation(program, "u_projectionMatrix");
    gProgramhandle->Uniforms.ModelNoCameraMatrix = glGetUniformLocation(program, "u_modelViewNoCameraMatrix");
    gProgramhandle->Uniforms.Modelview = glGetUniformLocation(program, "u_modelViewMatrix");
    gProgramhandle->Uniforms.NormalMatrix = glGetUniformLocation(program, "u_normalMatrix");

    gProgramhandle->Uniforms.AmbientMaterial = glGetUniformLocation(program, "u_ambientMaterial");
    gProgramhandle->Uniforms.SpecularMaterial = glGetUniformLocation(program, "u_specularMaterial");
    gProgramhandle->Uniforms.Shininess = glGetUniformLocation(program, "u_shininess");
    gProgramhandle->Uniforms.Interpolation_z = glGetUniformLocation(program, "u_interpolation_z");

    gProgramhandle->Uniforms.LightsSize = glGetUniformLocation(program, "u_lightsSize");
    gProgramhandle->Uniforms.IsSkinning = glGetUniformLocation(program, "u_isSkinning");


    for(int i = 0; i < MAX_LIGHTS; i++) {
        gProgramhandle->Uniforms.LightPosition[i] = glGetUniformLocation(program, ("u_lightPosition[" + std::to_string(i) +"]").c_str());
        gProgramhandle->Uniforms.DiffuseLight[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_diffuseLight").c_str());
        gProgramhandle->Uniforms.AmbientLight[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_ambientLight").c_str());
        gProgramhandle->Uniforms.SpecularLight[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_specularLight").c_str());

        gProgramhandle->Uniforms.AttenuationFactor[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_attenuationFactor").c_str());
        gProgramhandle->Uniforms.IsAttenuation[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_isAttenuation").c_str());
        gProgramhandle->Uniforms.LightRadius[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_lightRadius").c_str());
        gProgramhandle->Uniforms.SpotDirection[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_spotDirection").c_str());
        gProgramhandle->Uniforms.SpotExponent[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_spotExponent").c_str());
        gProgramhandle->Uniforms.SpotCutOffAngle[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_spotCutOffAngle").c_str());
        gProgramhandle->Uniforms.IsDirectional[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_isDirectional").c_str());
        gProgramhandle->Uniforms.LightMode[i] = glGetUniformLocation(program, ("u_lightSources[" + std::to_string(i) +"].u_lightMode").c_str());

    }

    gProgramhandle->Uniforms.JointMatrix = glGetUniformLocation(program, "u_jointMatrix");

}


GLuint OGLMgr::loadShader(GLenum shaderType, const char* pSource) {

    GLuint shader = glCreateShader(shaderType);

    if (shader) {

        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {

            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen) {

                auto buf = static_cast<char*>(malloc(infoLen));

                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    //LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    std::cout << "Could not compile shader:\n" << buf << '\n';
#ifdef WIN32
                    OutputDebugStringA(buf);
#endif
                    free(buf);
                }

                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

    return shader;

}


void OGLMgr::setProjectionRatio() {
    if (m_width > m_height)
        m_projectionRatio = (GLfloat) m_width / (GLfloat) m_height;
    else
        m_projectionRatio = (GLfloat) m_height / (GLfloat) m_width;


    CameraMgr::getInstance()->SetProjectionRatio(m_projectionRatio);

}


void OGLMgr::setVertexShader(GLchar* vertexSource) {
    m_vertexShader = vertexSource;
}


void OGLMgr::setFragmentShader(GLchar* fragmentSource) {
    m_fragmentShader = fragmentSource;
}


void OGLMgr::Render(float elapsedTime) {

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ////VBO 언바인딩
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}


void OGLMgr::ResizeWindow(GLuint width, GLuint height) {

    if (m_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }

    // Create the depth buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    /* Protect against a divide by zero */
    if (height == 0) {
        height = 1;
    }

    /* Setup our viewport. */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    m_width = width;
    m_height = height;

    setProjectionRatio();

}


void OGLMgr::releaseBuffers() {

    glDeleteFramebuffers(1, &m_framebuffer);
    m_framebuffer = 0;
    glDeleteRenderbuffers(1, &m_colorRenderbuffer);
    m_colorRenderbuffer = 0;

    if (m_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

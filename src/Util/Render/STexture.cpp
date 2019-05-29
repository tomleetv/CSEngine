//
// Created by ounols on 19. 4. 18.
//
#define STB_IMAGE_IMPLEMENTATION

#include "../../OGLDef.h"
#include "STexture.h"

#include "../Loader/STB/stb_image.h"

#include "../../Manager/TextureContainer.h"
#include "../../Manager/ResMgr.h"


STexture::STexture() {
    SetUndestroyable(true);
    ResMgr::getInstance()->Register<TextureContainer, STexture>(this);
}

STexture::~STexture() {

}

bool STexture::LoadFile(const char* path) {

    if(m_id != 0) return false;


    unsigned char *data = stbi_load(path, &m_width, &m_height, &m_channels, 0);

    return Load(data);
}
bool STexture::Load(unsigned char* data) {

    if(m_id != 0) {
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLuint channel = GL_RGB;
    if(m_channels == 4) channel = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, channel, m_width, m_height, 0, channel, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return true;
}

bool STexture::LoadEmpty() {
    if(m_id != 0) return false;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLubyte data[] = { 255, 255, 255, 255 };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return true;
}


bool STexture::ReloadFile(const char* path) {
    Release();
    LoadFile(path);
}

bool STexture::Reload(unsigned char* data) {
    Release();
    Load(data);
}

void STexture::Release() {
    glDeleteTextures(1, &m_id);
    m_id = 0;
    m_height = 0;
    m_width = 0;
}

void STexture::Exterminate() {
    Release();
//    ResMgr::getInstance()->Remove<TextureContainer, STexture>(this);
}

void STexture::Bind(GLint location, int layout) {
    if(m_id == 0) {
        LoadEmpty();
    }
    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(GL_TEXTURE_2D, m_id);
}




//
// Created by thoeni on 10/14/2018.
//

#include "SL.h"

#ifndef LIB_SLPROJECT_SLFBO_H
#define LIB_SLPROJECT_SLFBO_H

#endif //LIB_SLPROJECT_SLFBO_H

class SLGLFbo {
public:
    SLuint width, height, frameBuffer, textureColorBuffer, attachment, rbo;
    void activateAsTexture(int progId, SLstring samplerName, const int textureUnit = GL_TEXTURE0);
    SLGLFbo(GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
            GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);

    ~SLGLFbo();
};
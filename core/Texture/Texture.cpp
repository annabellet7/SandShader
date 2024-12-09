/*
    Author: Annabelle Thompson
*/
#include "Texture.h"

Texture2D::Texture2D(const char* filePath, int filterModeMin, int FliterModMag, int wrapModeS, int wrapModeT, int alpha)
{
    glGenTextures(1, &mId);
    glBindTexture(GL_TEXTURE_2D, mId);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterModeMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FliterModMag);

    // load and generate the texture
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath, &mWidth, &mHeight, &mNrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, alpha, mWidth, mHeight, 0, alpha, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << filePath << " Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

Texture2D::~Texture2D()
{
}

void Texture2D::bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, mId);
}

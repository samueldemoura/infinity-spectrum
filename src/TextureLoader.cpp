#include <stdio.h>
#include <GL/gl.h>
#include "stb_image.h"

GLuint LoadTexture(const char * bitmap_file)
{
    GLuint texture = 0;

    int width, height, nrChannels;
    unsigned char *data = stbi_load( bitmap_file, &width, &height, &nrChannels, 0);

    if (nrChannels == 0)
    {
        fprintf(stderr, "%s: error loading bitmap file\n", bitmap_file);
        return -1;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    stbi_image_free(data);
    printf("Texture loaded. Width is %d\n", width);

    return texture;
}

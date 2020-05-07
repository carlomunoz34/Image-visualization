#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"

typedef struct image
{
    char* filePath;
    unsigned char *img;
    int width, height, channels;

    image(char* filePath): filePath(filePath) 
    {
        img = stbi_load(filePath, &width, &height, &channels, 0);
        if (img == NULL)
        {
            printf("Error loading the image\n");
		    exit(1);
        }
    }

    ~image()
    {
        stbi_image_free(img);
    }

    unsigned char get(int i, int j, int k)
    {
        return img[i * channels * width + j * channels + k];
    }
}Image;

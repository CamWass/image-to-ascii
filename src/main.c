#include <stdio.h>
#include <stdint.h>

#define BUF_SIZE 8

typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAP_FILE_HEADER;

typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAP_INFO_HEADER;

int main(void)
{
    unsigned char buf[BUF_SIZE] = {0};
    FILE *fp = fopen("./test_images/letter_m.bmp", "rb");

    if (!fp)
    {
        fprintf(stderr, "error: file open failed.\n");
        return 1;
    }

    size_t bytes_read = fread(buf, sizeof(char), BUF_SIZE, fp);

    if (bytes_read < BUF_SIZE)
    {
        fprintf(stderr, "error: failed to fill buffer from file\n");
        return 1;
    }

    for (size_t i = 0; i < BUF_SIZE; i++)
    {
        printf("0x%02x ", buf[i]);
    }

    fclose(fp);

    return 0;
}
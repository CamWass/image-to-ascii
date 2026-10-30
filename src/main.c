#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <stddef.h>

#define BUF_SIZE 8

#pragma pack(push, 1)

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

#pragma pack(pop)

int main(void)
{
    FILE *fp = fopen("./test_images/letter_m.bmp", "rb");

    if (!fp)
    {
        fprintf(stderr, "error: file open failed.\n");
        fclose(fp);
        return 1;
    }

    BITMAP_FILE_HEADER fileHeader;

    if (fread(&fileHeader, sizeof(BITMAP_FILE_HEADER), 1, fp) != 1)
    {
        fprintf(stderr, "error: failed to read bitmap file header\n");
        fclose(fp);
        return 1;
    }

    // 0x4d42 is the ASCII string "BM".
    if (fileHeader.bfType != 0x4d42)
    {
        fprintf(stderr, "error: invalid BMP file\n");
        fclose(fp);
        return 1;
    }

    BITMAP_INFO_HEADER infoHeader;

    if (fread(&infoHeader, sizeof(BITMAP_INFO_HEADER), 1, fp) != 1)
    {
        fprintf(stderr, "error: failed to read bitmap info header\n");
        fclose(fp);
        return 1;
    }

    printf("Width: %" PRId32 "\n", infoHeader.biWidth);
    printf("Height: %" PRId32 "\n", infoHeader.biHeight);

    if (infoHeader.biBitCount != 24)
    {
        fprintf(stderr, "error: unsupported bits per pixel - must be 24\n");
        fclose(fp);
        return 1;
    }

    if (infoHeader.biCompression != 0)
    {
        fprintf(stderr, "error: bitmap compression is not supported\n");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}
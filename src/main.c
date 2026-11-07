#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

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

typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} RGB;

#pragma pack(pop)

int get_char_for_pixel(RGB pixel)
{
    uint64_t avg = (pixel.red + pixel.green + pixel.blue) / 3;

    return avg > 127 ? '@' : ' ';
}

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
        fprintf(stderr, "error: unsupported bits per pixel - must be 24, got %d\n", infoHeader.biBitCount);
        fclose(fp);
        return 1;
    }

    if (infoHeader.biCompression != 0)
    {
        fprintf(stderr, "error: bitmap compression is not supported: %d\n", infoHeader.biCompression);
        fclose(fp);
        return 1;
    }

    bool is_top_down = infoHeader.biHeight < 0;

    int64_t height = is_top_down
                         ? -(int64_t)infoHeader.biHeight
                         : infoHeader.biHeight;

    // +height since we have each row a newline.
    size_t output_size = infoHeader.biWidth * height + height;

    unsigned char *output = malloc(output_size);

    if (!output)
    {
        fprintf(stderr, "error: failed to alloc output buffer\n");
        fclose(fp);
        return 1;
    }

    uint64_t row_size = ((infoHeader.biBitCount * infoHeader.biWidth + 31) / 32) * 4;

    if (is_top_down)
    {
        // This branch is untested for now.

        unsigned char *output_pos = output;

        for (u_int64_t y = 0; y < height; y++)
        {
            if (fseek(fp, fileHeader.bfOffBits + row_size * y, SEEK_SET) != 0)
            {
                fprintf(stderr, "error: failed to seek\n");
                fclose(fp);
                return 1;
            }

            for (size_t x = 0; x < infoHeader.biWidth; x++)
            {
                RGB cur;

                if (fread(&cur, sizeof(RGB), 1, fp) != 1)
                {
                    fprintf(stderr, "error: failed to read pixel\n");
                    fclose(fp);
                    return 1;
                }

                *output_pos++ = get_char_for_pixel(cur);
            }
        }
    }
    else
    {
        uint64_t output_row = height - 1;

        for (uint64_t y = 0; y < height; y++)
        {
            if (fseek(fp, fileHeader.bfOffBits + row_size * y, SEEK_SET) != 0)
            {
                fprintf(stderr, "error: failed to seek\n");
                fclose(fp);
                return 1;
            }

            // +1 for \n at end of row.
            unsigned char *output_pos = output + (infoHeader.biWidth + 1) * output_row--;

            for (size_t x = 0; x < infoHeader.biWidth; x++)
            {
                RGB cur;

                if (fread(&cur, sizeof(RGB), 1, fp) != 1)
                {
                    fprintf(stderr, "error: failed to read pixel\n");
                    fclose(fp);
                    return 1;
                }

                *output_pos++ = get_char_for_pixel(cur);
            }

            *output_pos++ = '\n';
        }
    }

    FILE *out_fp = fopen("./out/out.txt", "wb");
    fwrite(output, output_size, 1, out_fp);
    fclose(out_fp);

    fclose(fp);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <search.h>
#pragma once

#pragma pack(push, 1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

typedef struct {
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
} BMPInfoHeader;

#pragma pack(pop)

typedef struct{
    BMPHeader * BMPHeader;
    BMPInfoHeader * BMPInfoHeader;
    uint8_t * pixels;
}BMP;

typedef struct{
    char component_name[5];
    int component_value;
}FilterParams;

typedef struct {
    int x;
    int y;
    int side_size;
    int thickness;
    int red, blue, green;
    bool fill;
}SquareParams;

typedef struct
{
    int red, green, blue;
}RGB;

typedef struct 
{
    int x, y;
}Point;


typedef struct 
{
    Point left_up, right_down;
    int mode;
}ExchangeParams;

typedef struct 
{
    Point left_up, right_down;
}FragmentParams;



#define ERROR_OPEN_FILE "Сouldn't open %s.\n"
#define ERROR_IS_NOT_BMP "%s format is not BMP.\n"
#define ERROR_BI_BIT_COUNT "BiBitCount isn't 24.\n" 
#define ERROR_UNKNOWN_COMPONENT_NAME "Unknown component_name: %s\nPossible options: red, blue, green.\n"
#define ERROR_UNCORRECT_COMPONENT_VALUE "Uncorrect component value: %d\nPossible values are from 0 to 255.\n"
#define ERROR_UNCORRECT_COORDINATES "Uncorrect coordinates x: %s y: %s\nThe coordinates must be positive integers.\n"
#define ERROR_UNCORRECT_SIDE_SIZE "Uncorrect side_size: %s\nMust be an integer greater than 0.\n"
#define ERROR_UNCORRECT_THICKNESS "Uncorrect thickness: %s\nMust be an integer greater than 0.\n"
#define ERROR_UNCORRECT_COLOR "Uncorrect color: %s.\n"
#define ERROR_UNCORRECT_FILL_VALUE "Uncorrect fill value: %s\nPossible options: true, false.\n"
#define ERROR_MORE_ACTIONS "You have entered more than one command.\n"
#define ERROR_INPUT_AND_OUTPUT_SIMILAR "Input and output file are similar.\n"
#define ERROR_UNCORRECT_EXCHANGE_TYPE "Uncorrect exchange_type: %s\nPossible options: clockwise, diagonals.\n"
#define BMP_TYPE 0x4D42 //"BM"
#define CLOCWISE_EXCHANGE_TYPE "clockwise"
#define DIAGONALS_EXCHANGE_TYPE "diagonals"

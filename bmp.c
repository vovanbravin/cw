#include "bmp.h"


BMP *read_bmp(const char *filename)
{
    FILE * file = fopen(filename, "rb");

    if(!file)
    {
        fprintf(stderr, ERROR_OPEN_FILE, filename);
        return NULL;
    }

    BMPHeader * bmpHeader = malloc(sizeof(BMPHeader));
    BMPInfoHeader * bmpInfoHeader = malloc(sizeof(BMPInfoHeader));

    fread(bmpHeader, sizeof(BMPHeader),1,file);
    fread(bmpInfoHeader, sizeof(BMPInfoHeader),1, file);

    if(bmpHeader->bfType != BMP_TYPE)
    {
        fprintf(stderr, ERROR_IS_NOT_BMP, filename);
        free(bmpHeader);
        free(bmpInfoHeader);
        return NULL;
    }
    else if(bmpInfoHeader->biBitCount != 24)
    {
        fprintf(stderr, ERROR_BI_BIT_COUNT);
        free(bmpHeader);
        free(bmpInfoHeader);
        return NULL;
    }

    BMP * bmp = malloc(sizeof(BMP));
    bmp->BMPHeader = bmpHeader;
    bmp->BMPInfoHeader = bmpInfoHeader;

    bmp->pixels = malloc(bmp->BMPInfoHeader->biSizeImage);
    fseek(file, bmp->BMPHeader->bfOffBits, SEEK_SET);
    fread(bmp->pixels,1, bmp->BMPInfoHeader->biSizeImage, file);

    fclose(file);

    return bmp;
}

bool save_bmp(const char *filename, BMP * bmp)
{
    FILE * file = fopen(filename, "wb");
    if(!file)
    {
        fprintf(stderr, ERROR_OPEN_FILE, filename);
        return false;
    }

    fwrite(bmp->BMPHeader, sizeof(BMPHeader), 1, file);
    fwrite(bmp->BMPInfoHeader, sizeof(BMPInfoHeader), 1, file);
    fseek(file, bmp->BMPHeader->bfOffBits, SEEK_SET);
    fwrite(bmp->pixels, 1, bmp->BMPInfoHeader->biSizeImage, file);

    fclose(file);

    return true;
}

int get_component_index(const char * component_name)
{
    if(strcmp(component_name, "blue"))
    {
        return 0;
    }
    else if(strcmp(component_name, "green"))
    {
        return 1;
    }
    return 2;
}

void filter(FilterParams * filter_params, BMP * bmp)
{
    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3 ) / 4) * 4;
    int component_index = get_component_index(filter_params->component_name); 

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth; x++)
        {
            int index = y * rowSize + x * 3;
            bmp->pixels[index + component_index] = filter_params->component_value;
        }
    }
}

void draw_square(SquareParams * square_params, BMP * bmp)
{
    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    printf("%d %d\n",square_params->y, square_params->y + square_params->side_size);

    for(int y = square_params->y; (y <= square_params->y + square_params->side_size) && y < bmp->BMPInfoHeader->biHeight;y++)
    {
        for(int x = square_params->x; (x <= square_params->x + square_params->side_size) && x < bmp->BMPInfoHeader->biWidth; x++)
        {
            if(square_params->fill == false && !(x < square_params->x + square_params->thickness) && !(x > square_params->x + square_params->side_size - square_params->thickness)
                && !(y < square_params->y + square_params->thickness) && !(y > square_params->y + square_params->side_size - square_params->thickness))
                continue;
            else{
                uint8_t * pixel = &bmp->pixels[(bmp->BMPInfoHeader->biHeight - y -1) * rowSize + x * 3];
                pixel[0] = square_params->blue;
                pixel[1] = square_params->green;
                pixel[2] = square_params->red;
            }
        }
    }
}

void freq_color(int red, int green, int blue, BMP * bmp)
{
    int *** colors = calloc(256, sizeof(int **));
    for(int i = 0; i < 256;i++)
    {
        colors[i] = calloc(256, sizeof(int *));
        for(int j = 0;j < 256;j++)
        {
            colors[i][j] = calloc(256, sizeof(int)); 
        }
    }

    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth;x++)
        {
            uint8_t  * pixel = &bmp->pixels[y * rowSize + x * 3];
            colors[pixel[0]][pixel[1]][pixel[2]]++;
        }
    }

    RGB max_color = {0, 0,0};
    int max_count = 0;

    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            for(int k = 0; k < 256; k++)
            {
                if(colors[i][j][k] > max_count)
                {
                    max_color.red = k;
                    max_color.green = j;
                    max_color.blue = i;
                    max_count = colors[i][j][k];
                }
            }
        }
    }

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth; x++)
        {
            uint8_t * pixel = &bmp->pixels[y * rowSize + x * 3];
            if(pixel[0] == max_color.blue & pixel[1] == max_color.green & pixel[2] == max_color.red)
            {
                pixel[0] = blue;
                pixel[1] = green;
                pixel[2] = red;
            }
        }
    }

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            free(colors[i][j]);
        }
        free(colors[i]);
    }
    free(colors);
}

void swap_fragment(FragmentParams * first_fragment, FragmentParams * second_fragment, BMP * bmp)
{
    int rowSizeBmp = ((bmp->BMPInfoHeader->biWidth * + 3) / 4) * 4;


    int width = first_fragment->left_up.x - first_fragment->right_down.x;
    int height = first_fragment->right_down.y - first_fragment->left_up.y;
    int rowSizeTmp = width * 3 + 3;
    uint8_t * pixels_tmp = malloc(rowSizeTmp * height);
    
    for(int y = first_fragment->left_up.y, j = 0; y < first_fragment->right_down.y && j < height;y++, j++)
    {
        for(int x = first_fragment->left_up.x, i = 0; x < first_fragment->right_down.x && i < width;x++, i++)
        {
            uint8_t * pixel_bmp = &bmp->pixels[y * rowSizeBmp + x * 3];
            uint8_t * pixel_tmp = &pixels_tmp[j * rowSizeTmp + i * 3];
            pixel_tmp[0] = pixel_bmp[0];
            pixel_tmp[1] = pixel_bmp[1];
            pixel_tmp[2] = pixel_bmp[2];
        }
    }
}

bool exchange(ExchangeParams *exchange_params, BMP *bmp)
{
    
}

bool ckeck_correct_component_name(const char *component_name)
{
    if(strcmp(component_name, "red") == 0 || strcmp(component_name, "blue") == 0 || strcmp(component_name, "green") == 0)
        return true;
    fprintf(stderr, ERROR_UNKNOWN_COMPONENT_NAME, component_name);
    return false;
}

bool check_correct_component_value(int component_value)
{
    if(component_value >= 0 && component_value <= 255)
        return true;
    fprintf(stderr, ERROR_UNCORRECT_COMPONENT_VALUE, component_value);
    return false;
}

bool is_integer(const char * str)
{
    char * endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

bool toCoordinate(int *x, int *y, char *params)
{
    char * x_str = strtok(params, ".");
    char * y_str = strtok(NULL, ".");
    if(is_integer(x_str) & is_integer(y_str) & atoi(x_str) >= 0 & atoi(y_str) >= 0)
    {
        *x = atoi(x_str);
        *y = atoi(y_str);
        return true;
    }
    else
    {
        fprintf(stderr, ERROR_UNCORRECT_COORDINATES, x_str, y_str);
        return false;
    }
}

bool toSideSize(int * size, const char * side_size)
{
    if(is_integer(side_size) & atoi(side_size) >= 0)
    {
        *size = atoi(side_size);
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_SIDE_SIZE, side_size);
    return false;
}

bool toThickness(int *thickness, const char *thickness_str)
{
    if(is_integer(thickness_str) & atoi(thickness_str) > 0)
    {
        *thickness = atoi(thickness_str);
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_THICKNESS, thickness_str);
    return false;
}


bool toColor(int *red, int * green, int * blue, char *color_str)
{
    char * red_str = strtok(color_str, ".");
    char * green_str = strtok(NULL, ".");
    char * blue_str = strtok(NULL, ".");
    if(is_integer(red_str) & is_integer(green_str) & is_integer(blue_str))
    {
        *red = atoi(red_str);
        *green = atoi(green_str);
        *blue = atoi(blue_str);
        if(check_correct_component_value(*red) & check_correct_component_value(*green) & check_correct_component_value(*blue))
        {
            return true;
        }
        return false;
    }
    fprintf(stderr, ERROR_UNCORRECT_COLOR, color_str);
    return false;
}

bool to_exchange_type(int * exchange_type, const char *exchange_type_str)
{
    if(strcmp(exchange_type_str, CLOCWISE_EXCHANGE_TYPE) == 0)
    {
        *exchange_type = 1;
        return true;
    }
    else if(strcmp(exchange_type_str, DIAGONALS_EXCHANGE_TYPE) == 0)
    {
        *exchange_type = 2;
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_EXCHANGE_TYPE, exchange_type_str);
    return false;
}

void info(BMP *bmp)
{
    printf("Width: %d\n", bmp->BMPInfoHeader->biWidth);
    printf("Height: %d\n", bmp->BMPInfoHeader->biHeight);
    printf("Size image: %d\n", bmp->BMPInfoHeader->biSizeImage);
}



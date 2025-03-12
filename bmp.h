#include "libs.h"

BMP * read_bmp(const char * filename);

bool save_bmp(const char * filename, BMP * bmp);

void filter(FilterParams * filter_params, BMP * bmp);

void draw_square(SquareParams * square_params, BMP * bmp);

void freq_color(int red, int green, int blue, BMP * bmp);

bool exchange(ExchangeParams * exchange_params, BMP * bmp);

bool ckeck_correct_component_name(const char * component_name);

bool check_correct_component_value(int component_value);

bool toCoordinate(int * x, int * y, char * params);

bool toSideSize(int * size, const char * side_size);

bool toThickness(int * thickness, const char * thickness_str);

bool toColor(int * red, int * green, int * blue, char * color_str);

bool to_exchange_type(int * exchange_type,const char * exchange_type_str);


void info(BMP * bmp);
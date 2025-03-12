#include "libs.h"
#include "bmp.h"

void help()
{
    printf("Course work for option 5.3, created by Vladimir Brovin\n");
    printf("The first function is the RGB filter component.The flag for performing this operation is `--rgbfilter'.\nThis tool should allow you to either set the value of a given component for the entire image in the range from 0 to 255.\n");
    printf("The second function is Drawing a square. Flag for performing this operation: `--square`.\n");
    printf("The third function is to swap 4 pieces of the area. The flag for performing this operation is `--exchange'.\nThe rectangular area selected by the user is divided into 4 parts and these parts are swapped.\n");
    printf("The fourth function: Finds the most common color and replaces it with another preset color.\nThe flag for performing this operation is `--freq_color'.\n");
}       


int main(int agrc, char * agrs[])
{
    struct option long_options[] = 
    {
        {"rgbfilter", no_argument, NULL, 'r'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"component_name", required_argument, NULL, 'n'},
        {"component_value", required_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"info", no_argument, NULL, 'I'},
        {"square", no_argument, NULL, 's'},
        {"left_up", required_argument, NULL, 'l'},
        {"side_size", required_argument, NULL, 'S'},
        {"thickness", required_argument, NULL , 't'},
        {"color", required_argument, NULL, 'c'},
        {"fill", no_argument, NULL, 'f'},
        {"fill_color", required_argument, NULL, 'F'},
        {"freq_color", no_argument, NULL, 'q'},
        {"exchange", no_argument, NULL, 'e'},
        {"right_down", required_argument, NULL, 'R'},
        {"exchange_type", required_argument, NULL, 'T'},
        {0, 0, 0, 0}
    };

    char input_file[30] = "";
    char output_file[30];
    FilterParams * filter_params = NULL;
    SquareParams * square_params = NULL;
    ExchangeParams * exchange_params = NULL;
    BMP * bmp;
    char mode = ' ';
    int red, blue, green;

    int opt;
    while((opt = getopt_long(agrc, agrs, "ri:o:n:v:l:c:t:", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'i':
            strcpy(input_file, optarg);
            break;
        case 'o':
            strcpy(output_file, optarg);
            break;
        case 'n':
            strcpy(filter_params->component_name, optarg);
            if(!ckeck_correct_component_name(filter_params->component_name))
                return 40;
            if(filter_params->component_value != -1)
            {
                filter(filter_params, bmp);
            }
            break;
        case 'v':
            filter_params->component_value = atoi(optarg);
            if(!check_correct_component_value(filter_params->component_value))
                return 41;
            if(strcmp(filter_params->component_name, "") != 0)
            {
                filter(filter_params, bmp);
            }
            break;
        case 'r':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'f';
            filter_params = malloc(sizeof(FilterParams));
            strcpy(filter_params->component_name, "");
            filter_params->component_value = -1;
            break;
        case 'I':
            if(bmp != NULL)
            {
                info(bmp);
            }
            break;
        case 's':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 's';
            square_params = malloc(sizeof(SquareParams));
            square_params->fill = false;
            break;
        case 'l':
            if(square_params != NULL)
            {
                if(!toCoordinate(&square_params->x, &square_params->y, optarg))
                    return 41;
            }
            else
            {
                if(!toCoordinate(&exchange_params->left_up.x, &exchange_params->left_up.y, optarg))
                    return 41;
            }
            break;
        case 'S':
            if(!toSideSize(&square_params->side_size, optarg))
                return 41;
            break;
        case 't':
            if(!toThickness(&square_params->thickness, optarg))
                return 41;
            break;
        case 'c':
            if(square_params != NULL)
            {
                if(!toColor(&square_params->red, &square_params->green, &square_params->blue, optarg))
                    return 41;
            }
            else
            {
                if(!toColor(&red, &green, &blue, optarg))
                {
                    return 41;
                }
            }
            break;
        case 'f':
            square_params->fill = true;
            break;
        case 'F':
            if(!toColor(&square_params->red, &square_params->green, &square_params->blue, optarg))
                return 41;
            break;
        case 'q':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'q';
            break;
        case 'e':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'e';
            exchange_params = malloc(sizeof(ExchangeParams));
            break;
        case 'R':
            if(!toCoordinate(&exchange_params->right_down.x, &exchange_params->right_down.y, optarg))
                return 41;
            break;
        case 'T':
            if(!to_exchange_type(&exchange_params->mode, optarg))
                return 41;
            break;
        case 'h':
            help();
            return 0;
            break;
        default:
            break;
        }
    }

    if(strcmp(input_file, "") == 0)
    {
        strcpy(input_file, agrs[agrc - 1]);
        printf("Input file: %s\n", input_file);
    }

    bmp = read_bmp(input_file);
    if(!bmp) return 41;

    if(strcmp(input_file, output_file) == 0)
    {
        fprintf(stderr, ERROR_INPUT_AND_OUTPUT_SIMILAR);
        return 41;
    }
    
    switch (mode)
    {
    case 's':
        draw_square(square_params, bmp);
        break;
    case 'f':
        filter(filter_params, bmp);
        break;
    case 'q':
        freq_color(red, green, blue, bmp);
        break;
    case 'e':

        break;
    default:
        break;
    }
    if(!save_bmp(output_file, bmp))
    {
        return 41;
    }

    return 0;
}
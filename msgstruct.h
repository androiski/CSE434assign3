#include <stdint.h>
#include <time.h>

typedef struct{

    uint8_t first_mn;
    uint8_t second_mn;
    uint8_t opcode;
    struct tm time;
    unsigned int length;
    char msg[200];

}msgstruct;

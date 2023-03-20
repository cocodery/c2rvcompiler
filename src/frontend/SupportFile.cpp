#include "SupportFile.hh"

void parseIntLiteral(const char *number1_str, int32_t *number1) {
    if (number1_str[0] == '0') {
        if (number1_str[1] == 'x') {
            sscanf(number1_str, "%x", number1);
        } else {
            sscanf(number1_str, "%o", number1);
        }
    } else {
        sscanf(number1_str, "%d", number1);
    }
}
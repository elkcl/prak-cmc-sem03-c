#include <string.h> // strlen()

const char TEMPLATE[] = "rwxrwxrwx";

enum
{
    MASK_SIZE = sizeof(TEMPLATE) / sizeof(TEMPLATE[0]) - 1
};

int
parse_rwx_permissions(const char *str)
{
    if (str == NULL) {
        return -1;
    }
    size_t len = strlen(str);
    if (len != MASK_SIZE) {
        return -1;
    }
    int ans = 0;
    for (int i = 0; i < MASK_SIZE; ++i) {
        int str_pos = MASK_SIZE - 1 - i;
        char curr = str[str_pos];
        if (curr == TEMPLATE[str_pos]) {
            ans |= 1u << i;
        } else if (curr != '-') {
            return -1;
        }
    }
    return ans;
}

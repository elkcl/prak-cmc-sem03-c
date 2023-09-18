STYPE
bit_reverse(STYPE value)
{
    UTYPE cnt = 0;
    cnt = ~cnt;
    UTYPE ans = 0;
    UTYPE val = value;
    for (; cnt != 0; cnt >>= 1) {
        ans <<= 1;
        ans += val & 1;
        val >>= 1;
    }
    return (STYPE) ans;
}

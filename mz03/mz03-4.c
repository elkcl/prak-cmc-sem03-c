int
satsum(int v1, int v2)
{
    enum
    {
        MY_INT_MAX = 2147483647,
        MY_INT_MIN = -2147483648
    };
    if (v1 > 0 && v2 > 0 && MY_INT_MAX - v1 <= v2) {
        return MY_INT_MAX;
    } else if (v1 < 0 && v2 < 0 && MY_INT_MIN - v1 >= v2) {
        return MY_INT_MIN;
    } else {
        return v1 + v2;
    }
}

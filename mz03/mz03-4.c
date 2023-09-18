int
satsum(int v1, int v2)
{
    enum
    {
        MINUS_ONE = ~0,
        ONE = -MINUS_ONE,
        THIRTY_TWO = ONE << (ONE + ONE + ONE + ONE + ONE),
        MY_INT_MIN = (int) ((unsigned int) ONE << (THIRTY_TWO - ONE)),
        MY_INT_MAX = -(MY_INT_MIN + ONE)
    };
    if (v1 > 0 && v2 > 0 && MY_INT_MAX - v1 <= v2) {
        return MY_INT_MAX;
    } else if (v1 < 0 && v2 < 0 && MY_INT_MIN - v1 >= v2) {
        return MY_INT_MIN;
    } else {
        return v1 + v2;
    }
}

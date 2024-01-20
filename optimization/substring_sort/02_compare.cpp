// Comparison function for substring sort
bool compare_int(const char* s1, const char* s2) {
    char c1, c2;
    for (int i1 = 0, i2 = 0; ; ++i1, ++i2) {
        c1 = s1[i1]; c2 = s2[i2];
        if (c1 != c2) return c1 > c2;
    }
}

bool compare_uint(const char* s1, const char* s2) {
    char c1, c2;
    for (unsigned int i1 = 0, i2 = 0; ; ++i1, ++i2) {
        c1 = s1[i1]; c2 = s2[i2];
        if (c1 != c2) return c1 > c2;
    }
}

bool compare_uint_l(const char* s1, const char* s2, unsigned int l) {
    if (s1 == s2) return false;
    char c1, c2;
    for (unsigned int i1 = 0, i2 = 0; i1 < l; ++i1, ++i2) {
        c1 = s1[i1]; c2 = s2[i2];
        if (c1 != c2) return c1 > c2;
    }
    return false;
}
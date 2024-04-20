int func(int *a, char b) {

}

int test() {
    int a[4];
    int c = 1;
    a[a[c] * 42] = 5 + func(&c, a[c + 2]) * 12;

}
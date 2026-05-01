#include <stdio.h>
int main() {
    double sw; char c1[16], c2[16];
    int n = sscanf("cq 1.0px Olive Moccasin", "cq %lfpx %15s %15s", &sw, c1, c2);
    printf("parsed %d: %f %s %s\n", n, sw, c1, c2);
    int n2 = sscanf("cq 1.0px Olive Moccasin", "cq %lf %15s %15s", &sw, c1, c2);
    printf("parsed %d: %f %s %s\n", n2, sw, c1, c2);
    return 0;
}

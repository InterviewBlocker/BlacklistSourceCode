#include <stdio.h>

int test(int m,int n)
{
    int k;
    k=m>n?m:n;
    for(;(k%m!=0)||(k%n!=0);k++);
    return k;
}


int main()
{
    test(9,6);
    return 0;
}
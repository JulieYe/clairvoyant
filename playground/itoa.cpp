#include <cstdio>
#include <cstdlib>

int main()
{
    int i;
    char buffer [33];
    printf ("Enter a number: ");
    scanf ("%d",&i);
    sprintf(buffer,"%d",i);
    printf ("decimal: %s\n",buffer);
    return 0;
}

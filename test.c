#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int i,j,k,l;
    char alfabeto[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};

    int len = strlen(alfabeto);
    printf("Alfabeto: %i\n", len);

    for(i = 0; i < strlen(alfabeto); i ++)
    {
        for (j = 0; j < strlen(alfabeto); j++)
        {
            for(k = 0; k < strlen(alfabeto); k++)
            {
                for(l = 0; l < strlen(alfabeto); l++)
                {
                    printf("%i\t", j);
                    printf("%c", alfabeto[i]);
                    printf("%c", alfabeto[j]);
                    printf("%c", alfabeto[k]);
                    printf("%c\n", alfabeto[l]);
                }
            }   
        }
    }

    printf("Todas las combinaciones de AXTEL :D\n");
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
typedef struct {
    double *data;
    unsigned int nrows, ncols;
} Matrix;

int main()
{
    // int i, j;

    // // printf("Enter number of rows: ");
    // // scanf("%d",&rows);
    // int rows = 7;

    // for(i=1; i<=rows; ++i)
    // {
    //     for(j=1; j<=i; ++j)
    //     {
    //         printf("* ");
    //     }
    //     printf("\n");
    // }

    Matrix *ptr;
    ptr = (Matrix *) malloc(sizeof(Matrix));

    printf("%i\n", (int)ptr);
    printf("%i\n", sizeof(Matrix));
    printf("%i\n", sizeof(double));
    printf("%i\n", sizeof(int));

    return 0;
}
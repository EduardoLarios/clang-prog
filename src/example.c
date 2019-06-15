<<<<<<< HEAD:src/example.c
#include <stdio.h>

int main()
{
    int i, j, rows;

    printf("Enter number of rows: ");
    scanf("%d",&rows);

    for(i=1; i<=rows; ++i)
    {
        for(j=1; j<=i; ++j)
        {
            printf("* ");
        }
        printf("\n");
    }
    return 0;
=======
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
>>>>>>> dba24e00f929e61a369d49f9f360660afde49cd2:example.c
}
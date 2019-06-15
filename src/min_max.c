// Eduardo Larios Fernández
// A00569364 - ISC

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void traverse_dir(char *dirname, char *program)
{
    DIR *dir;
    struct dirent *direntry;
    int min = 0;
    int max = 0; 
    int a;

    FILE *file = NULL;

    if ((dir = opendir(dirname)) == NULL)
    {
        perror(program);
        exit(-1);
    }

    printf("Directory: %s\n\n", dirname);

    while ((direntry = readdir(dir)) != NULL)
    {
        printf("Filename: %s/%s\n", dirname, direntry->d_name);
        file = fopen(direntry->d_name, "r");

        fscanf(file, "%d,", &a);
        min = max = a;
        while( fscanf(file, "%d", &a) )
        {
            if (min > a) min = a;
            if (max < a) max = a;
        }

        printf("Maximum: %d\n", max);
        printf("Minimum: %d\n", min);
        
        fclose(file);
        //getchar();
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    const char *dir;
    char directory[NAME_MAX + 1];
    struct stat sb;

    if (argc != 2)
    {
        printf("usage: %s [directory]\n", argv[0]);
        return -1;
    }

    getcwd(directory, NAME_MAX);

    if (argc == 2)
    {
        dir = argv[1];
        if (stat(dir, &sb) == 0)
        {
            if (S_ISDIR(sb.st_mode))
            {
                traverse_dir(argv[1], argv[0]);
            }
            else
            {
                printf("%s: Not a directory\n", argv[0]);
            }
        }

        else
        {
            printf("%s: No such file or directory\n", argv[0]);
        }
    }

    return 0;
}
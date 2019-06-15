// Eduardo Larios
// A00569364 - ISC
// ITESM CQ 11/05/2018

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    int encrypted = 0;
    int decrypted = 0; 

    char *input;
    char *output;

    unsigned char letter;

    // Verifies usage and number of parameters
    if(argc != 3)
    {
        printf("Usage: decrypt -[source] -[destination]\n");
        return -1;
    }

    input = argv[1];
    output = argv[2];

    // Verifies input file existance and opens it
    if ((encrypted = open(input, O_RDONLY)) < 0)
    {
        printf("decript: the file %s does not exist.\n", input);
        return -1;
    }

    // Verifies output existance or creates it
    if ((decrypted = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
        printf("There was an error creating the file %s\n", output);
        return -1;
    }

    // Reads per byte and shifts its bits, rightmost 4 to the left 
    // and leftmost wrapping-around to the other side
    while(read(encrypted, &letter, 1))
    {
        letter = (letter << 4) | (letter >> 4);
        write(decrypted, &letter, 1);
    }

    // Closes both file descriptors
    printf("Decryption complete.\n");
    close(encrypted);
    close(decrypted);

    return 0;
}

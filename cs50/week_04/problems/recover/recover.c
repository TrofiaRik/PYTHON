#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Accept a single command-line argument
    if (argc != 2)
    {
        printf("Usage: ./recover FILENAME\n");
        return 1;
    }
    // Open the memory card & declare vars
    FILE *card = fopen(argv[1], "r");
    FILE *n_file;
    char filename[8];
    uint8_t data_buffer[512]; //-> bef. fread, decl&assign databuffer
    int idx = 0; //-> filename index

    // While there's still data left to read from the memory card
    while (fread(data_buffer, 1, 512, card) == 512)
    {
        if (data_buffer[0] == 0xff && data_buffer[1] == 0xd8 &&
            data_buffer[2] == 0xff && ((data_buffer[3] & 0xf0) == 0xe0))
        {       
            if (idx == 0)
            {    
                sprintf(filename, "%03i.jpg", idx);
                n_file = fopen(filename, "w");
                if (n_file == NULL)
                    return 1;
                fwrite(data_buffer, 1, 512, n_file);
                idx++;
            }
            else 
            {
                fclose(n_file);
                n_file = NULL;
                sprintf(filename, "%03i.jpg", idx);
                n_file = fopen(filename, "w");
                if (n_file == NULL)
                    return 1;
                fwrite(data_buffer, 1, 512, n_file);
                idx++;
            }
        }
        else
        {
            if (idx != 0)
            {
                fwrite(data_buffer, 1, 512, n_file);
            }
        }    
    }
    // close the file
    fclose(card);
    fclose(n_file);    
}

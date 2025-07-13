#include <stdint.h>
#include <stdio.h>
#include <string.h>

// required definitions

// Instance pointer or something like. Not used internally, 
// only passed to READ_FILE_AT, PROCESS_APP, PROCESS_KEY
#define FILE_HANDLE                         FILE*
// Error codes, returned at INI processing
#define BINI_SUCCESS                        0
#define BINI_DO_KEYS                        1
#define BINI_ERR_MEM                        3
#define BINI_ERR_FILE                       4
#define BINI_ERR_IO                         5

// file read function and wrapper for it
int read_file_at(FILE *f, uint32_t file_pos, uint8_t *buf, uint32_t len);
#define BINI_READ_FILE_AT(hf, pos, buf, len) read_file_at((hf), (pos), (buf), (len))

// callback from parser into application, passes file handle and found APP name string,
// must return !0 - process KEYs of that APP, 0 - goto next APP
int process_app(FILE *f, uint8_t *app);
#define BINI_PROCESS_APP(hf, app)            process_app((hf), (app))
// callback from parser into application, passes file handle, KEY name string and KEY value
// must return !0 - continue KEYs processing for that APP, 0 - goto next APP
int process_key(FILE *f, uint8_t *key, uint8_t *val, uint32_t val_len);
#define BINI_PROCESS_KEY(hf, key, val, len)  process_key((hf), (key), (val), (len))

#define BINI_IMPLEMENT
#include "bini.h"

#define BUF_SIZE                            65536UL

FILE *ini;
int  ret;

uint8_t  buffer[BUF_SIZE];
uint8_t  *user_app = NULL; 
uint8_t  *user_key = NULL;

int read_file_at(FILE * f, uint32_t file_pos, uint8_t *buf, uint32_t len)
{
    if (fseek(f, file_pos, SEEK_SET)) 
    {
        return BINI_ERR_IO;
    }
    if (len && (!fread(buf, len, 1, f))) 
    {
        return BINI_ERR_IO;
    }
    return BINI_SUCCESS;
}

int process_app(FILE *f, uint8_t *app)
{
    // are we are looking for specific APP?
    if (user_app)
    {
        // is that APP we are looking for?
        if (!strcmp(user_app, app))
        {
            // yes, need to process keys
            fprintf( stdout, "%s\n", app); 
            return BINI_DO_KEYS;
        }
    }
    else
    {
        // just print the APP name
        fprintf( stdout, "%s\n", app); 
    }
    // look up next APP
    return BINI_SUCCESS;
}

int process_key(FILE *f, uint8_t *key, uint8_t *val, uint32_t val_len)
{
    // specific KEY was requested?
    if (user_key)
    {
        // is that KEY we are looking for? 
        if (strcmp(user_key, key))
        {
            // no, continue searching
            return BINI_DO_KEYS;
        }
        // yes, it is
        fprintf( stdout, "    %s\n", key); 
        fprintf( stdout, "        ");
        // limit output to reasonable length
        if (val_len > 16) 
        {
            val_len = 16;
        }
        while (val_len--)
        {
            fprintf( stdout, "0x%02hx ", *val++);
        }
        fprintf( stdout, "\n");
        return BINI_SUCCESS;
    }
    else
    {
        // just print the KEY name
        fprintf( stdout, "    %s\n", key); 
        return BINI_DO_KEYS;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "USAGE: %s <ini-file> [<app-name>] [[<value-name>]]\n", argv[0]);
        return 1;
    }
    if (argc > 2)
    {
        // if app name supplied, take it
        user_app = (uint8_t*)argv[2]; 
    }
    if (argc > 3)
    {
        // if key name supplied, take it
        user_key = (uint8_t*)argv[3]; 
    }
    ini = fopen(argv[1], "rb");
    if (!ini)
    {
        fprintf(stderr, "Can't open file: %s\n", argv[1]);
        return 2;
    }
    ret = read_bini(ini, buffer, BUF_SIZE);
    fclose(ini);

    fprintf(stderr, "\n");
    switch(ret)
    {
        case BINI_SUCCESS:
            fprintf(stderr, "Done\n");
            break;

        case BINI_ERR_MEM:
            fprintf(stderr, "Buffer of %d bytes is too small\n", BUF_SIZE); 
            break;

        case BINI_ERR_FILE:
            fprintf(stderr, "File %s is incorrect\n", argv[1]); 
            break;

        default:
            fprintf(stderr, "Unknown return code! %d\n", ret);
            break;
    }
    return ret;
}

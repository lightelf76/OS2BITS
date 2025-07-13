#include <stdint.h>

#ifndef BINI_IMPLEMENT
// Prototype for the single INI-read function
int read_bini(void* inst, uint8_t *buf, uint32_t length);

// definitions, to be provided by user

// Instance pointer or something like. Not used internally, 
// only passed to READ_FILE_AT, PROCESS_APP, PROCESS_KEY
// #define FILE_HANDLE                         FILE*

// Error codes, returned at INI processing
// #define BINI_SUCCESS                        0
// #define BINI_DO_KEYS                        1
// #define BINI_ERR_MEM                        3
// #define BINI_ERR_FILE                       4
// #define BINI_ERR_IO                         5

// Macros to be called from read_bini
// #define BINI_READ_FILE_AT(handle, position, buffer, length)

// callback from parser, passes the handle and the string of APP found,
// must return !0 - process KEYs of that APP, 0 - goto next APP
// #define BINI_PROCESS_APP(handle, app_string)

// callback from parser into application, passes file handle, KEY name string and KEY value
// must return !0 - continue KEYs processing for that APP, 0 - goto next APP
// #define BINI_PROCESS_KEY(handle, key_string, value_buf, value_length)

#else //BINI_IMPLEMENT

#define BINI_SIGNATURE         0xFFFFFFFFUL
// INI-file internal structures
// All offsets are relative to the start of the file.
// uint16_t-typed fields are duplicated, both copies contains equal values. 
// Purpose of the duplication unknown.

// Header at the start of the BINI file
#pragma pack(push,1)
typedef struct bini_hdr_s
{
    uint32_t    signature;       // seems like signature, always 0xFFFFFFFF
    uint32_t    first_app;       // offset of the first APP structure
    uint32_t    file_size;       // total size of the BINI file
    uint32_t    zero[2];         // always zero, intended purpose still unknown
} bini_hdr_t;
#pragma pack(pop)

// APP structure
#pragma pack(push,1)
typedef struct bini_app_s
{
    uint32_t    next_app;        // offset of the next APP, zero at last APP
    uint32_t    key_offset;      // offset of the first KEY, zero if none
    uint32_t    zero;            // always zero, intended purpose still unknown
    uint16_t    name_length[2];  // APP name length, includes trailing zero byte
    uint32_t    name_offset;     // offset of the APP name string
} bini_app_t;
#pragma pack(pop)

// KEY structure
#pragma pack(push,1)
typedef struct bini_key_s
{
    uint32_t    next_key;        // offset of the next KEY, zero at last KEY of that APP
    uint32_t    zero;            // always zero, intended purpose still unknown
    uint16_t    name_length[2];  // KEY name length, includes trailing zero 
    uint32_t    name_offset;     // offset of the KEY name string
    uint16_t    val_length[2];   // length of the value binary array
    uint32_t    val_offset;      // offset of the value
} bini_key_t;
#pragma pack(pop)

int read_bini(void* inst, uint8_t *buf, uint32_t length)
{
    FILE_HANDLE hf = (FILE_HANDLE)inst;
    bini_hdr_t  hdr;
    bini_app_t  app;
    uint32_t    app_offset;
    bini_key_t  key;
    uint32_t    key_offset;

    // read INI header
    if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, 0, (void*)&hdr, sizeof(bini_hdr_t)))
    {
        return BINI_ERR_IO;
    }

    if ( (hdr.signature != BINI_SIGNATURE) || 
         (hdr.zero[0] != 0)            || 
         (hdr.zero[1] != 0)            || 
         (hdr.first_app >= hdr.file_size) 
       )
    {
        // wrong INI file header
        return BINI_ERR_FILE;
    }
    // traverse the list of all APPs
    app_offset = hdr.first_app;

    while (app_offset)
    {
        // read next APP
        if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, app_offset, (void*)&app, sizeof(bini_app_t)))
        {
            return BINI_ERR_IO;
        }
        // validate it's fields
        if ( (app.zero != 0)                            || 
             (app.name_length[0] != app.name_length[1]) || 
             (app.key_offset >= hdr.file_size)          || 
             (app.name_offset >= hdr.file_size) 
           )
        {
            // wrong APP content
            return BINI_ERR_FILE;
        }
        if (app.name_length[0] > length)
        {
            // too small buffer provided
            return BINI_ERR_MEM;
        }
        // read APP name
        if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, app.name_offset, (void*)buf, app.name_length[0]))
        {
            return BINI_ERR_IO;
        }
        // pass APP name to application for processing decision
        if (BINI_DO_KEYS == BINI_PROCESS_APP(hf, buf))
        {
            // KEYs processing requested, traverse the list
            key_offset = app.key_offset;
            while (key_offset)
            {
                if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, key_offset, (void*)&key, sizeof(bini_key_t)))
                {
                    return BINI_ERR_IO;
                }
                // validate KEY fields
                if ( (key.zero != 0)                            || 
                     (key.name_length[0] != key.name_length[1]) ||
                     (key.val_length[0] != key.val_length[1])   ||
                     (key.name_offset > hdr.file_size)          ||
                     (key.val_offset  > hdr.file_size)
                   )
                {
                    return BINI_ERR_FILE;
                }
                // check the buffer size is enough for KEY name and value
                if ((key.name_length[0] + key.val_length[0]) > length)
                {
                    return BINI_ERR_MEM;
                }
                if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, key.name_offset, (void*)buf, key.name_length[0]))
                {
                    return BINI_ERR_IO;
                }
                if (BINI_SUCCESS != BINI_READ_FILE_AT(hf, key.val_offset, (void*)(buf + key.name_length[0]), key.val_length[0]))
                {
                    return BINI_ERR_IO;
                }
                if (BINI_DO_KEYS != BINI_PROCESS_KEY(hf, buf, buf + key.name_length[0], key.val_length[0]))
                {
                    break;
                }
                // next KEY
                key_offset = key.next_key;
            }
        }
        // next APP
        app_offset = app.next_app;
    }
    return BINI_SUCCESS;
}
#endif // BINI_IMPLEMENT

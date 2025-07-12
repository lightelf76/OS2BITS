#ifndef __H_LX_UNPACK__
#define __H_LX_UNPACK__

#ifdef __cplusplus
#extern "C" {
#endif

#include <stdint.h>

// LX executable format tied to 4K pages
#define LX_PAGE_SIZE                   4096

// unpack one page, packed with EXEPACK:1
int16_t lx_unpack1(uint8_t *dst, uint8_t *src, int16_t src_size);
// unpack one page, packed with EXEPACK:2 
int16_t lx_unpack2(uint8_t *dst, uint8_t *src, int16_t src_size);

#ifdef __cplusplus
}
#endif

#ifdef LX_UNPACK_IMPLEMENTATION

// simple byte-by-byte copy (memcpy-like)
static void bcopy(uint8_t *dst, uint8_t *src, uint16_t len)
{
    for (; len > 0; len--)
    {
        *dst++ = *src++;
    }
}

// simple byte fill (memset-like)
static void bfill(uint8_t *dst, uint8_t val, uint16_t len)
{
    for (; len > 0; len--)
    {
        *dst++ = val;
    }
}

// unpack one page, packed with EXEPACK:1
int16_t lx_unpack1(uint8_t *dst, uint8_t *src, int16_t src_size)
{
    int16_t dst_size = LX_PAGE_SIZE;
    uint16_t nr;
    uint16_t len;

    while (src_size > 0)
    {
        nr = src[0] | ( (uint16_t)src[1] << 16);
        len = src[2] | ( (uint16_t)src[3] << 16);
        src += 4;
        src_size -= len + 4;
        if (src_size < 0)
        {
            // out of bounds
            return -1;
        }
        while (nr--)
        {
            dst_size -= len;
            if (dst_size < 0)
            {
                // out of bounds
                return -1;
            }
            bcopy(dst, src, len);
            dst += len;
        }
        src += len;
    }
    return LX_PAGE_SIZE - dst_size;
}

// unpack one page, packed with EXEPACK:2 
// algorithm based on the A.Wynn and J.Wu paper 
// with clarifications from archiveteam.org
int16_t lx_unpack2(uint8_t *dst, uint8_t *src, int16_t src_size)
{
    int16_t dst_size = LX_PAGE_SIZE;

    while (src_size > 0)
    {
        switch (src[0] & 3)
        {
            case 0: // nroots
                {
                    uint8_t len;

                    len = src[0] >> 2;
                    if (len)
                    {
                        // non-compressed bytes
                        src_size -= len + 1;
                        dst_size -= len;
                        if ((src_size < 0) || (dst_size < 0))
                        {
                            // out of bounds
                            return -1;
                        }
                        bcopy(dst, &src[1], len);
                        dst += len;
                        src += len + 1;
                    }
                    else if (src_size >= 2)
                    {
                        // iterated bytes
                        len = src[1];
                        if (len)
                        {
                            src_size -= 3;
                            dst_size -= len;
                            if ((src_size < 0) || (dst_size < 0))
                            {
                                // out of bounds
                                return -1;
                            }
                            bfill(dst, src[2], len);
                            dst += len;
                            src += 3;
                        }
                        else
                        {
                            // end marker
                            return LX_PAGE_SIZE - dst_size;
                        }
                    }
                    else
                    {
                        // bad data
                        return -1;
                    }
                }
                break;

            case 1: // short string token 
                {
                    uint8_t nr, len;
                    uint16_t off;

                    // uncompressed bytes count
                    nr = (src[0] & 0x0C) >> 2;
                    // repeated string length
                    len = ((src[0] & 0x70) >> 4) + 3;
                    // 9-bit offset into dst
                    off = ((uint16_t)src[1] << 1) | (src[0] >> 7);
                    src += 2;
                    src_size -= nr + 2;
                    dst_size -= nr;
                    if ((src_size < 0) || (dst_size < 0))
                    {
                        // out of bounds
                        return -1;
                    }
                    // copy uncomressed bytes, if any
                    bcopy(dst, src, nr);
                    dst += nr;
                    src += nr;
                    if (off > (LX_PAGE_SIZE - dst_size))
                    {
                        // out of bounds
                        return -1;
                    }
                    dst_size -= len;
                    if (dst_size < 0)
                    {
                        // out of bounds
                        return -1;
                    }
                    // copy repeated bytes
                    bcopy(dst, dst - off, len);
                    dst += len;
                }
                break;

            case 2: // mid string token
                {
                    uint8_t len;
                    uint16_t off;

                    // repeated string length
                    len = ((src[0] & 0x0C) >> 2) + 3;
                    // 12-bit offset into dst
                    off = ( (uint16_t)src[1] << 4) | (src[0] >> 4);
                    src += 2;
                    src_size -= 2;
                    if (off > (LX_PAGE_SIZE - dst_size))
                    {
                        // out of bounds
                        return -1;
                    }
                    dst_size -= len;
                    if (dst_size < 0)
                    {
                        // out of bounds
                        return -1;
                    }
                    bcopy(dst, dst - off, len);
                    dst += len;
                }
                break;

            case 3: // long string
                {
                    uint8_t nr, len;
                    uint16_t off;

                    // uncompressed bytes count
                    nr = (src[0] & 0x3C) >> 2;
                    // repeated string length
                    len = ( ( (uint16_t)src[1] & 0x0F) << 2) | ((src[0] & 0xC0) >> 6);
                    // 12-bit offset into dst
                    off = ( (src[1] & 0xF0) >> 4) | ( (uint16_t)src[2] << 4);
                    src += 3;
                    src_size -= 3;
                    dst_size -= nr;
                    if ((src_size < 0) || (dst_size < 0))
                    {
                        // out of bounds
                        return -1;
                    }
                    // copy uncomressed bytes, if any
                    bcopy(dst, src, nr);
                    dst += nr;
                    src += nr;
                    src_size -= nr;
                    if (off > (LX_PAGE_SIZE - dst_size))
                    {
                        // out of bounds
                        return -1;
                    }
                    dst_size -= len;
                    if (dst_size < 0)
                    {
                        // out of bounds
                        return -1;
                    }
                    // copy repeated bytes
                    bcopy(dst, dst - off, len);
                    dst += len;
                }
                break;
        }
    }
    return LX_PAGE_SIZE - dst_size;
}

#endif // LX_UNPACK_IMPLEMENTATION

#endif // __H_LX_UNPACK__

#pragma once

#include "utility.h"

namespace Compression
{
    int compress(const u8* src, int srclen, u8* dest, int destlen); // returns the amount of space used to compress; 0 on error
    void decompress(const u8* src, int srclen, u8* dest, int destlen);
}


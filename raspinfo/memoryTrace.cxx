//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <inttypes.h>
#include <unistd.h>

#include "bcm_host.h"

#include "font.h"
#include "memoryTrace.h"

//-------------------------------------------------------------------------

void
CMemoryTrace::
getMemoryStats(
    SMemoryStats& memoryStats)
{
    FILE *fp = fopen("/proc/meminfo", "r");

    if (fp == 0)
    {
        perror("unable to open /proc/stat");
        exit(EXIT_FAILURE);
    }

    char buffer[128];

    while (fgets(buffer, sizeof(buffer), fp))
    {
        char name[64];
        uint32_t value;

        if (sscanf(buffer, "%[a-zA-Z]: %" SCNu32 " kB", name, &value) == 2)
        {
            if (strcmp(name, "MemTotal") == 0)
            {
                memoryStats.total = value;
            }
            else if (strcmp(name, "MemFree") == 0)
            {
                memoryStats.free = value;
            }
            else if (strcmp(name, "Buffers") == 0)
            {
                memoryStats.buffers = value;
            }
            else if (strcmp(name, "Cached") == 0)
            {
                memoryStats.cached = value;
            }
        }
    }

    fclose(fp);

    memoryStats.used = memoryStats.total
                     - memoryStats.free
                     - memoryStats.buffers
                     - memoryStats.cached;
}

//-------------------------------------------------------------------------

CMemoryTrace::
CMemoryTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight)
:
    CPanel(width, traceHeight + sc_fontHeight + 4, yPosition),
    m_traceHeight(traceHeight),
    m_gridHeight(gridHeight),
    m_values(0),
    m_used(width),
    m_buffers(width),
    m_cached(width),
    m_time(width),
    m_usedColour(0, 109, 44),
    m_usedGridColour(0, 109, 44),
    m_buffersColour(102, 194, 164),
    m_buffersGridColour(102, 194, 164),
    m_cachedColour(237, 248, 251),
    m_cachedGridColour(237, 248, 251),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0),
    m_gridColour(48, 48, 48)
{
    m_usedGridColour = CRGB565::blend(63, m_gridColour, m_usedColour);
    m_buffersGridColour = CRGB565::blend(63, m_gridColour, m_buffersColour);
    m_cachedGridColour = CRGB565::blend(63, m_gridColour, m_cachedColour);

    //---------------------------------------------------------------------

    getImage().clear(m_background);

    uint8_t smallSquare = 0xFE;

    SFontPosition position = 
        drawString(0,
                   getImage().getHeight() - 2 - sc_fontHeight,
                   "Memory",
                   m_foreground,
                   getImage());

    position = drawString(position.x,
                          position.y,
                          " (used:",
                          m_foreground,
                          getImage());

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_usedColour,
                        getImage());

    position = drawString(position.x,
                          position.y,
                          " buffers:",
                          m_foreground,
                          getImage());

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_buffersColour,
                        getImage());

    position = drawString(position.x,
                          position.y,
                          " cached:",
                          m_foreground,
                          getImage());

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_cachedColour,
                        getImage());

    position = drawString(position.x,
                          position.y,
                          ")",
                          m_foreground,
                          getImage());

    for (int32_t j = 0 ; j < traceHeight + 1 ; j+= m_gridHeight)
    {
        for (int32_t i = 0 ; i < getImage().getWidth() ;  ++i)
        {
            getImage().setPixel(i, j, m_gridColour);
        }
    }
}

//-------------------------------------------------------------------------

void
CMemoryTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    SMemoryStats memoryStats;
    getMemoryStats(memoryStats);

    int8_t used = (memoryStats.used * m_traceHeight) / memoryStats.total;
    int8_t buffers = (memoryStats.buffers * m_traceHeight)
                   / memoryStats.total;
    int8_t cached = (memoryStats.cached * m_traceHeight)
                 / memoryStats.total;

    int16_t index;

    if (m_values < getImage().getWidth())
    {
        index = m_values++;
    }
    else
    {
        index = getImage().getWidth() - 1;

        std::rotate(m_used.begin(),
                    m_used.begin() + 1,
                    m_used.end());

        std::rotate(m_buffers.begin(),
                    m_buffers.begin() + 1,
                    m_buffers.end());

        std::rotate(m_cached.begin(),
                    m_cached.begin() + 1,
                    m_cached.end());

        std::rotate(m_time.begin(),
                    m_time.begin() + 1,
                    m_time.end());
    }

    m_used[index] = used;
    m_buffers[index] = buffers;
    m_cached[index] = cached;
    m_time[index] = now % 60;

    //-----------------------------------------------------------------

    for (int16_t i = 0 ; i < m_values ; ++i)
    {
        int16_t j = m_traceHeight - 1;

        for (int16_t u = 0 ; u < m_used[i] ; ++u)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                getImage().setPixel(i, j--, m_usedGridColour);
            }
            else
            {
                getImage().setPixel(i, j--, m_usedColour);
            }
        }

        for (int16_t b = 0 ; b < m_buffers[i] ; ++b)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                getImage().setPixel(i, j--, m_buffersGridColour);
            }
            else
            {
                getImage().setPixel(i, j--, m_buffersColour);
            }
        }

        for (int16_t c = 0 ; c < m_cached[i] ; ++c)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                getImage().setPixel(i, j--, m_cachedGridColour);
            }
            else
            {
                getImage().setPixel(i, j--, m_cachedColour);
            }
        }

        for ( ; j >= 0 ; --j)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                getImage().setPixel(i, j, m_gridColour);
            }
            else
            {
                getImage().setPixel(i, j, m_background);
            }
        }
    }

    putImage(fb);
}


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

#ifndef DYNAMIC_INFO_H
#define DYNAMIC_INFO_H

//-------------------------------------------------------------------------

#include <cstdint>
#include <string>

#include "framebuffer565.h"
#include "image565.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

class CDynamicInfo
{
public:

    CDynamicInfo(int16_t width, int16_t yPosition);

    int16_t getBottom() const { return m_yPosition + m_image.getHeight(); }
    void show(const CFrameBuffer565& fb);

private:

    int16_t m_yPosition;
    CImage565 m_image;

    CRGB565 m_heading;
    CRGB565 m_foreground;
    CRGB565 m_background;

    static std::string getIpAddress(char& interface);
    static std::string getMemorySplit();
    static std::string getTemperature();
    static std::string getTime();
};

//-------------------------------------------------------------------------

#endif

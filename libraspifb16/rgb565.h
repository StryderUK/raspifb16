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

#ifndef RGB565_H
#define RGB565_H

//-------------------------------------------------------------------------

#include <cstdint>

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class CRGB565
{
public:

    CRGB565(uint8_t red, uint8_t green, uint8_t blue);

    explicit CRGB565(uint16_t rgb);

    uint8_t getRed() const;
    uint8_t getGreen() const;
    uint8_t getBlue() const;

    uint16_t get565() const { return m_rgb; }

    void setRGB(uint8_t red, uint8_t green, uint8_t blue);

    void set565(uint16_t rgb) { m_rgb = rgb; }

    static CRGB565 blend(uint8_t alpha, const CRGB565& a, const CRGB565& b);

private:

    uint16_t m_rgb;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

#endif
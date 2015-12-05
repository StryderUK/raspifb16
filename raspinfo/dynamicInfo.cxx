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

#include <ifaddrs.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <bcm_host.h>
#pragma GCC diagnostic pop

#include "font.h"
#include "dynamicInfo.h"

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getIpAddress(
    char& interface)
{
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    interface = 'X';

    std::string address = "   .   .   .   ";

    getifaddrs(&ifaddr);

    for (ifa = ifaddr ; ifa != NULL ; ifa = ifa->ifa_next)
    {
        if (ifa ->ifa_addr->sa_family == AF_INET)
        {
            void *addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            if (strcmp(ifa->ifa_name, "lo") != 0)
            {
                char buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
                address = buffer;
                interface = ifa->ifa_name[0];
                break;
            }
        }
    }

    if (ifaddr != NULL)
    {
        freeifaddrs(ifaddr);
    }

    return address;
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getMemorySplit()
{
    int arm_mem = 0;
    int gpu_mem = 0;

    char buffer[128];

    memset(buffer, 0, sizeof(buffer));

    if (vc_gencmd(buffer, sizeof(buffer), "get_mem arm") == 0)
    {
        sscanf(buffer, "arm=%dM", &arm_mem);
    }

    if (vc_gencmd(buffer, sizeof(buffer), "get_mem gpu") == 0)
    {
        sscanf(buffer, "gpu=%dM", &gpu_mem);
    }

    if ((arm_mem != 0) && (gpu_mem != 0))
    {
        snprintf(buffer, sizeof(buffer), "%d/%d", gpu_mem, arm_mem);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), " / ");
    }

    return buffer;
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getTemperature()
{
    double temperature = 0.0;

    char buffer[128];

    memset(buffer, 0, sizeof(buffer));

    if (vc_gencmd(buffer, sizeof(buffer), "measure_temp") == 0)
    {
        sscanf(buffer, "temp=%lf'C", &temperature);
    }

    snprintf(buffer, sizeof(buffer), "%2.0f", temperature);

    return buffer;
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getTime(
    time_t now)
{
    char buffer[128];

    struct tm result;
    struct tm *lt = localtime_r(&now, &result);
    strftime(buffer, sizeof(buffer), "%T", lt);

    return buffer;
}

//-------------------------------------------------------------------------

CDynamicInfo::
CDynamicInfo(
    int16_t width,
    int16_t yPosition)
:
    CPanel{width, 2 * (sc_fontHeight + 4), yPosition},
    m_heading(255, 255, 0),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0)
{
}

//-------------------------------------------------------------------------

void
CDynamicInfo::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    getImage().clear(m_background);

    //---------------------------------------------------------------------

    SFontPosition position = { 0, 0 };

    position = drawString(position.x,
                          position.y,
                          "ip(",
                          m_heading,
                          getImage());

    char interface = ' ';
    std::string ipaddress = getIpAddress(interface);

    position = drawChar(position.x,
                        position.y,
                        interface,
                        m_foreground,
                        getImage());

    position = drawString(position.x,
                          position.y,
                          ") ",
                          m_heading,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          ipaddress,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " memory ",
                          m_heading,
                          getImage());

    std::string memorySplit = getMemorySplit();

    position = drawString(position.x,
                          position.y,
                          memorySplit,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " MB",
                          m_foreground,
                          getImage());

    //---------------------------------------------------------------------

    position.x = 0;
    position.y += sc_fontHeight + 4;

    position = drawString(position.x,
                          position.y,
                          "time ",
                          m_heading,
                          getImage());

    std::string timeString = getTime(now);

    position = drawString(position.x,
                          position.y,
                          timeString,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " temperature ",
                          m_heading,
                          getImage());

    std::string temperatureString = getTemperature();

    position = drawString(position.x,
                          position.y,
                          temperatureString,
                          m_foreground,
                          getImage());

    uint8_t degreeSymbol = 0xF8;

    position = drawChar(position.x,
                        position.y,
                        degreeSymbol,
                        m_foreground,
                        getImage());


    position = drawString(position.x,
                          position.y,
                          "C",
                          m_foreground,
                          getImage());

    //---------------------------------------------------------------------
    
    putImage(fb);
}


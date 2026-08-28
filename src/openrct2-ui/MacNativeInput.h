#pragma once

#ifdef __APPLE__
namespace OpenRCT2::Ui
{
    void PollNativeMacOSScroll(float& x, float& y);
    int PollNativeMacOSPinch();
}
#endif

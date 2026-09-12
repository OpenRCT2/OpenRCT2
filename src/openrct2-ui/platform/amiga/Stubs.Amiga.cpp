/*****************************************************************************
 * AmigaOS stand-ins for the SDL-only parts of openrct2-ui that are not built:
 * SDL audio (Ui.cpp already falls back to the dummy audio context when
 * CreateAudioContext throws) and the SDL_LoadBMP bitmap reader.
 *****************************************************************************/
#ifdef __amigaos__

    #include "../../SDLException.h"
    #include "../../audio/AudioContext.h"
    #include "../../drawing/BitmapReader.h"

    #include <istream>
    #include <memory>
    #include <openrct2/core/Imaging.h>
    #include <stdexcept>


namespace OpenRCT2::Ui
{
    static Image ReadBitmapUnsupported(std::istream&, ImageFormat)
    {
        throw std::runtime_error("BMP images are not supported on AmigaOS yet.");
    }

    void RegisterBitmapReader()
    {
        Imaging::SetReader(ImageFormat::bitmap, ReadBitmapUnsupported);
    }
} // namespace OpenRCT2::Ui

#endif // __amigaos__

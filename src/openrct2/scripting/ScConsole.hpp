#include <dukglue/dukglue.h>
#include "../interface/Console.h"

namespace OpenRCT2::Scripting
{
    class ScConsole
    {
    private:
        InteractiveConsole& _console;
    public:
        ScConsole(InteractiveConsole& console) :
            _console(console)
        {
        }

        void clear()
        {
            _console.Clear();
        }

        void log(const std::string &s)
        {
            _console.WriteLine(s);
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_method(ctx, &ScConsole::clear, "clear");
            dukglue_register_method(ctx, &ScConsole::log, "log");
        }
    };
}

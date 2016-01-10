extern "C"
{
    #include "../platform/platform.h"
}

#include "../core/Console.hpp"
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "CommandLine.hpp"

CommandLineArgEnumerator::CommandLineArgEnumerator(const char * const * arguments, int count)
{
    _arguments = arguments;
    _count = count;
    _index = 0;
}

void CommandLineArgEnumerator::Reset()
{
    _index = 0;
}

bool CommandLineArgEnumerator::TryPop()
{
    if (_index < _count)
    {
        _index++;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandLineArgEnumerator::TryPopInteger(sint32 * result)
{
    char const * arg;
    if (TryPopString(&arg))
    {
        *result = (sint32)atol(arg);
        return true;
    }
    
    return false;
}

bool CommandLineArgEnumerator::TryPopReal(float * result)
{
    char const * arg;
    if (TryPopString(&arg))
    {
        *result = (float)atof(arg);
        return true;
    }
    
    return false;
}

bool CommandLineArgEnumerator::TryPopString(const char * * result)
{
    if (_index < _count)
    {
        *result = _arguments[_index];
        _index++;
        return true;
    }
    else
    {
        return false;
    }
}

namespace CommandLine
{
    constexpr const char * HelpText = "openrct2 -ha shows help for all commands. "
                                      "openrct2 <command> -h will show help and details for a given command.";

    static void   PrintOptions(const CommandLineOptionDefinition *options);
    static void   PrintExamples(const CommandLineExample *examples);
    static utf8 * GetOptionCaption(utf8 * buffer, size_t bufferSize, const CommandLineOptionDefinition *option);

    void PrintHelp()
    {
        const CommandLineCommand * command;
        size_t maxNameLength = 0;
        size_t maxParamsLength = 0;

        // Get the largest command name length and parameter length
        for (command = RootCommands; command->Name != nullptr; command++)
        {
            maxNameLength = Math::Max(maxNameLength, String::LengthOf(command->Name));
            maxParamsLength = Math::Max(maxParamsLength, String::LengthOf(command->Parameters));
        }

        // Print usage / main commands
        const char * usageString = "usage: openrct2 ";
        const size_t usageStringLength = String::LengthOf(usageString);

        Console::Write(usageString);
        for (command = RootCommands; command->Name != nullptr; command++)
        {
            if (command != RootCommands)
            {
                Console::WriteSpace(usageStringLength);
            }

            Console::Write(command->Name);
            Console::WriteSpace(maxNameLength - String::LengthOf(command->Name) + 1);

            if (command->SubCommands == nullptr)
            {
                Console::Write(command->Parameters);
                Console::WriteSpace(maxParamsLength - String::LengthOf(command->Parameters));

                if (command->Options != nullptr)
                {
                    Console::Write(" [options]");
                }
            }
            else
            {
                Console::Write("...");
            }
            Console::WriteLine();
        }
        Console::WriteLine();

        PrintOptions(RootCommands->Options);
        PrintExamples(RootExamples);

        Console::WriteLine(HelpText);
    }

    static void PrintOptions(const CommandLineOptionDefinition *options)
    {
        // Print options for main commands
        size_t maxOptionLength = 0;
        const CommandLineOptionDefinition * option = options;
        for (; option->Type != 255; option++)
        {
            char buffer[128];
            GetOptionCaption(buffer, sizeof(buffer), option);
            size_t optionCaptionLength = String::LengthOf(buffer);
            maxOptionLength = Math::Max(maxOptionLength, optionCaptionLength);
        }

        option = RootCommands->Options;
        for (; option->Type != 255; option++)
        {
            Console::WriteSpace(4);

            char buffer[128];
            GetOptionCaption(buffer, sizeof(buffer), option);
            size_t optionCaptionLength = String::LengthOf(buffer);
            Console::Write(buffer);

            Console::WriteSpace(maxOptionLength - optionCaptionLength + 4);
            Console::Write(option->Description);
            Console::WriteLine();
        }
        Console::WriteLine();
    }

    static void PrintExamples(const CommandLineExample *examples)
    {
        size_t maxArgumentsLength = 0;

        const CommandLineExample * example;
        for (example = examples; example->Arguments != nullptr; example++)
        {
            size_t argumentsLength = String::LengthOf(example->Arguments);
            maxArgumentsLength = Math::Max(maxArgumentsLength, argumentsLength);
        }

        Console::WriteLine("examples:");
        for (example = examples; example->Arguments != nullptr; example++)
        {
            Console::Write("  openrct2 ");
            Console::Write(example->Arguments);

            size_t argumentsLength = String::LengthOf(example->Arguments);
            Console::WriteSpace(maxArgumentsLength - argumentsLength + 4);
            Console::Write(example->Description);
            Console::WriteLine();
        }

        Console::WriteLine();
    }

    static utf8 * GetOptionCaption(utf8 * buffer, size_t bufferSize, const CommandLineOptionDefinition *option)
    {
        buffer[0] = 0;

        if (option->ShortName != '\0')
        {
            String::AppendFormat(buffer, bufferSize, "-%c, ", option->ShortName);
        }

        String::Append(buffer, bufferSize, "--");
        String::Append(buffer, bufferSize, option->LongName);

        switch (option->Type) {
        case CMDLINE_TYPE_INTEGER:
            String::Append(buffer, bufferSize, "=<int>");
            break;
        case CMDLINE_TYPE_REAL:
            String::Append(buffer, bufferSize, "=<real>");
            break;
        case CMDLINE_TYPE_STRING:
            String::Append(buffer, bufferSize, "=<str>");
            break;
        }

        return buffer;
    }

    const CommandLineCommand * FindCommandFor(const CommandLineCommand * commands, const char * const * arguments, int count)
    {
        // Check if there are any arguments
        if (count == 0)
        {
            return nullptr;
        }

        // Check if options have started
        const char * firstArgument = arguments[0];
        if (firstArgument[0] == '-')
        {
            return nullptr;
        }

        // Search through defined commands for one that matches
        const CommandLineCommand * fallback = nullptr;
        const CommandLineCommand * command = commands;
        for (; command->Name != nullptr; command++)
        {
            if (command->Name[0] == '\0')
            {
                // If we don't find a command, this should be used
                fallback = command;
            }
            else if (String::Equals(command->Name, firstArgument))
            {
                if (command->SubCommands == nullptr)
                {
                    // Found matching command
                    return command;
                }
                else
                {
                    // Recurse for the sub command table
                    return FindCommandFor(command->SubCommands, &arguments[1], count - 1);
                }
            }
        }

        return fallback;
    }
}

void CommandLineDisplayUsageFor(const char * command)
{

}

extern "C"
{
    int cmdline_run(const char * * argv, int argc)
    {
        CommandLine::PrintHelp();
        return 0;
    }
}

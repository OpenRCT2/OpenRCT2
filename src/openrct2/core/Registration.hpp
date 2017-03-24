#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

namespace OpenRCT2
{
    /**
     * Represents a registration of some service which when deleted will be
     * unregistered.
     */
    interface IRegistration
    {
        virtual ~IRegistration() = default;
    };

    class Registration
    {
    private:
        /**
         * Class which can wrap a function in an IRegistration.
         */
        template<typename T>
        struct CallbackRegistration : public IRegistration
        {
        private:
            T _callback;

        public:
            CallbackRegistration(T callback) :
                _callback(callback)
            {
            }

            virtual ~CallbackRegistration() override
            {
                _callback();
            }
        };

    public:
        /**
         * Creates a new IRegistration which when deleted, calls the given
         * function.
         */
        template<typename T>
        static IRegistration * Create(T unregisterCallback)
        {
            return new CallbackRegistration<T>(unregisterCallback);
        }
    };
}

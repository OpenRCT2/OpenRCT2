/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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

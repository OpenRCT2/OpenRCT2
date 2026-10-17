#pragma once

#ifdef ENABLE_SCRIPTING

    #include "ScEntity.hpp"

namespace OpenRCT2
{
    struct Duck;
}

namespace OpenRCT2::Scripting
{
    class ScDuck;
    extern ScDuck gScDuck;

    class ScDuck final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);
        void Register(JSContext* ctx);

    private:
        static Duck* GetDuck(JSValue thisVal);

        static JSValue positionFrozen_get(JSContext* ctx, JSValue thisVal);
        static JSValue positionFrozen_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue animationFrozen_get(JSContext* ctx, JSValue thisVal);
        static JSValue animationFrozen_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue orientation_get(JSContext* ctx, JSValue thisVal);
        static JSValue orientation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue animationFrame_get(JSContext* ctx, JSValue thisVal);
        static JSValue animationFrame_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue animation_get(JSContext* ctx, JSValue thisVal);
        static JSValue animation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);
    };
} // namespace OpenRCT2::Scripting

#endif

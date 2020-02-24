#pragma once

#include "detail_method.h"

// const getter, setter
template <typename Cls, typename RetT, typename ArgT>
void dukglue_register_property(duk_context* ctx,
	RetT(Cls::*getter)() const,
	void(Cls::*setter)(ArgT),
	const char* name)
{
	dukglue_register_property<true, Cls, RetT, ArgT>(ctx, getter, setter, name);
}

// const getter, no setter
template <typename Cls, typename RetT>
void dukglue_register_property(duk_context* ctx,
	RetT(Cls::*getter)() const,
	std::nullptr_t setter,
	const char* name)
{
	dukglue_register_property<true, Cls, RetT, RetT>(ctx, getter, setter, name);
}

// non-const getter, setter
template <typename Cls, typename RetT, typename ArgT>
void dukglue_register_property(duk_context* ctx,
	RetT(Cls::*getter)(),
	void(Cls::*setter)(ArgT),
	const char* name)
{
	dukglue_register_property<false, Cls, RetT, ArgT>(ctx, getter, setter, name);
}

// non-const getter, no setter
template <typename Cls, typename RetT>
void dukglue_register_property(duk_context* ctx,
	RetT(Cls::*getter)(),
	std::nullptr_t setter,
	const char* name)
{
	dukglue_register_property<false, Cls, RetT, RetT>(ctx, getter, setter, name);
}

// no getter, setter
template <typename Cls, typename ArgT>
void dukglue_register_property(duk_context* ctx,
	std::nullptr_t getter,
	void(Cls::*setter)(ArgT),
	const char* name)
{
	dukglue_register_property<false, Cls, ArgT, ArgT>(ctx, getter, setter, name);
}

// no getter, no setter
template <typename Cls, typename ValT>
void dukglue_register_property(duk_context* ctx, std::nullptr_t getter, std::nullptr_t setter, const char* name)
{
	// strictly speaking I think duktape can probably handle neither
	// (according to the wonderful API docs), but I don't know why you
	// would want to do this in the first place
	static_assert(std::is_void<Cls>::value, "Must have getter or setter");
}

inline duk_ret_t dukglue_throw_error(duk_context* ctx)
{
	duk_error(ctx, DUK_ERR_TYPE_ERROR, "Property does not have getter or setter.");
}

template <bool isConstGetter, typename Cls, typename RetT, typename ArgT>
void dukglue_register_property(duk_context* ctx,
	typename std::conditional<isConstGetter, RetT(Cls::*)() const, RetT(Cls::*)()>::type getter,
	void(Cls::*setter)(ArgT),
	const char* name)
{
	using namespace dukglue::detail;
	typedef MethodInfo<isConstGetter, Cls, RetT> GetterMethodInfo;
	typedef MethodInfo<false, Cls, void, ArgT> SetterMethodInfo;

	ProtoManager::push_prototype<Cls>(ctx);

	// push key
	duk_push_string(ctx, name);

	// push getter
	if (getter != nullptr) {
		duk_c_function method_func = GetterMethodInfo::MethodRuntime::call_native_method;

		duk_push_c_function(ctx, method_func, 0);

		duk_push_pointer(ctx, new typename GetterMethodInfo::MethodHolder{ getter });
		duk_put_prop_string(ctx, -2, "\xFF" "method_holder"); // consumes raw method pointer

		// make sure we free the method_holder when this function is removed
		duk_push_c_function(ctx, GetterMethodInfo::MethodRuntime::finalize_method, 1);
		duk_set_finalizer(ctx, -2);
	} else {
		duk_push_c_function(ctx, dukglue_throw_error, 1);
	}

	if (setter != nullptr) {
		duk_c_function method_func = SetterMethodInfo::MethodRuntime::call_native_method;

		duk_push_c_function(ctx, method_func, 1);

		duk_push_pointer(ctx, new typename SetterMethodInfo::MethodHolder{ setter });
		duk_put_prop_string(ctx, -2, "\xFF" "method_holder"); // consumes raw method pointer

		// make sure we free the method_holder when this function is removed
		duk_push_c_function(ctx, SetterMethodInfo::MethodRuntime::finalize_method, 1);
		duk_set_finalizer(ctx, -2);
	} else {
		duk_push_c_function(ctx, dukglue_throw_error, 1);
	}

	duk_uint_t flags = DUK_DEFPROP_HAVE_GETTER
		| DUK_DEFPROP_HAVE_SETTER
		| DUK_DEFPROP_SET_ENUMERABLE
		| DUK_DEFPROP_HAVE_CONFIGURABLE /* set not configurable (from JS) */
		| DUK_DEFPROP_FORCE /* allow overriding built-ins and previously defined properties */;

	duk_def_prop(ctx, -4, flags);
	duk_pop(ctx);  // pop prototype
}

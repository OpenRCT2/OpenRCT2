#pragma once

#include "detail_function.h"

// Register a function, embedding the function address at compile time.
// According to benchmarks, there's really not much reason to do this
// (inconsistent 2-3% performance improvement for a 10,000 function call stress test averaged over 100 runs),
// since it has much uglier syntax and may bloat executable size if you have many functions with identical signatures.
template<typename T, T Value, typename RetType, typename... Ts>
void dukglue_register_function_compiletime(duk_context* ctx, RetType(*)(Ts...), const char* name)
{
	static_assert(std::is_same<T, RetType(Ts...)>::value,
		"Mismatching function pointer template parameter and function pointer argument types. "
		"Try: dukglue_register_function<decltype(func), func>(ctx, \"funcName\", func)");

	duk_c_function evalFunc = dukglue::detail::FuncInfoHolder<RetType, Ts...>::template FuncActual<Value>::call_native_function;

	duk_push_c_function(ctx, evalFunc, sizeof...(Ts));
	duk_put_global_string(ctx, name);
}

// Register a function.
template<typename RetType, typename... Ts>
void dukglue_register_function(duk_context* ctx, RetType(*funcToCall)(Ts...), const char* name)
{
	duk_c_function evalFunc = dukglue::detail::FuncInfoHolder<RetType, Ts...>::FuncRuntime::call_native_function;

	duk_push_c_function(ctx, evalFunc, sizeof...(Ts));

  static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void*), "Function pointer and data pointer are different sizes");
	duk_push_pointer(ctx, reinterpret_cast<void*>(funcToCall));
	duk_put_prop_string(ctx, -2, "\xFF" "func_ptr");

	duk_put_global_string(ctx, name);
}

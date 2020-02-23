#pragma once

#include <string>

#include "detail_traits.h"
#include "detail_types.h"

#include <duktape.h>

namespace dukglue
{
	namespace detail
	{
		// Helper to get the argument tuple type, with correct storage types.
		template<typename... Args>
		struct ArgsTuple {
			typedef std::tuple<typename dukglue::types::ArgStorage<Args>::type...> type;
		};

		// Helper to get argument indices.
		// Call read for every Ts[i], for matching argument index Index[i].
		// The traits::index_tuple is used for type inference.
		// A concrete example:
		//   get_values<int, bool>(duktape_context)
		//     get_values_helper<{int, bool}, {0, 1}>(ctx, ignored)
		//       std::make_tuple<int, bool>(read<int>(ctx, 0), read<bool>(ctx, 1))
		template<typename... Args, size_t... Indexes>
		typename ArgsTuple<Args...>::type get_stack_values_helper(duk_context* ctx, dukglue::detail::index_tuple<Indexes...>)
		{
			using namespace dukglue::types;
			return std::forward_as_tuple(DukType<typename Bare<Args>::type>::template read<typename ArgStorage<Args>::type>(ctx, Indexes)...);
		}

		// Returns an std::tuple of the values asked for in the template parameters.
		// Values will remain on the stack.
		// Values are indexed from the bottom of the stack up (0, 1, ...).
		// If a value does not exist or does not have the expected type, an error is thrown
		// through Duktape (with duk_error(...)), and the function does not return
		template<typename... Args>
		typename ArgsTuple<Args...>::type get_stack_values(duk_context* ctx)
		{
			// We need the argument indices for read_value, and we need to be able
			// to unpack them as a template argument to match Ts.
			// So, we use traits::make_indexes<Ts...>, which returns a traits::index_tuple<0, 1, 2, ...> object.
			// We pass that into a helper function so we can put a name to that <0, 1, ...> template argument.
			// Here, the type of Args isn't important, the length of it is.
			auto indices = typename dukglue::detail::make_indexes<Args...>::type();
			return get_stack_values_helper<Args...>(ctx, indices);
		}
	}
}

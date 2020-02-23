#pragma once

#include <type_traits>

#include "detail_refs.h"
#include "detail_typeinfo.h"
#include "detail_class_proto.h"

// TODO try adding a using namespace std in here if I can scope it to just this file

namespace dukglue {
	namespace types {

		// Bare<T>::type is T stripped of reference, pointer, and const off a type, like so:
		// Bare<Dog>::type          = Dog
		// Bare<const Dog>::type    = Dog
		// Bare<Dog*>::type         = Dog
		// Bare<const Dog*>::type   = Dog
		// Bare<Dog&>::type         = Dog
		// Bare<const Dog&>::type   = Dog
		template<typename T>
		struct Bare {
			typedef typename std::remove_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::type type;
		};

		// DukType<T> provides functions for reading and writing T from the Duktape stack.
		// T is always a "bare type," i.e. "Dog" rather than "Dog*".

		// There are two kinds of DukTypes:
		//   1. "Native" DukTypes. This is the default.
		//      These types use an underlying native object allocated on the heap.
		//      A pointer to the object (of type T*) is expected at script_object.\xFFobj_ptr.
		//      "Native" DukTypes can return a value (returns a copy-constructed T from the native object),
		//      a pointer (just returns script_object.\xFFobj_ptr), or a reference (dereferences script_object.\xFFobj_ptr if it is not null).

		//   2. "Value" DukTypes. These are implemented through template specialization.
		//      This is how primitive types are implemented (int, float, const char*).
		//      These types can only be returned by value (T) or by const reference (const T&).
		//      Attempting to read a pointer (T*) or non-const reference (T&) will give a compile-time error.
		//      You can also use this to implement your own lightweight types, such as a 3D vector.
		//      (Strictly speaking, non-const references (T&) *could* be returned, but any changes to the reference would
		//      be discarded. So, I wrote a static assert to disable the option. If you understand the implications,
		//      you should be able to safely comment out the static_assert in ArgStorage.)
		template<typename T>
		struct DukType {
			static_assert(std::is_same<T, typename Bare<T>::type >::value, "Invalid base type, expected bare type");

			typedef std::false_type IsValueType;

			// read pointer
			template<typename FullT, typename = typename std::enable_if< std::is_pointer<FullT>::value>::type >
			static T* read(duk_context* ctx, duk_idx_t arg_idx) {
				using namespace dukglue::detail;

				if (duk_is_null(ctx, arg_idx))
					return nullptr;

				if (!duk_is_object(ctx, arg_idx)) {
					duk_int_t type_idx = duk_get_type(ctx, arg_idx);
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected native object, got %s", arg_idx, get_type_name(type_idx));
				}

				duk_get_prop_string(ctx, arg_idx, "\xFF" "type_info");
				if (!duk_is_pointer(ctx, -1))  // missing type_info, must not be a native object
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected native object (missing type_info)", arg_idx);

				// make sure this object can be safely returned as a T*
				TypeInfo* info = static_cast<TypeInfo*>(duk_get_pointer(ctx, -1));
				if (!info->can_cast<T>())
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: wrong type of native object", arg_idx);

				duk_pop(ctx);  // pop type_info

				duk_get_prop_string(ctx, arg_idx, "\xFF" "obj_ptr");
				if (!duk_is_pointer(ctx, -1))
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: invalid native object.", arg_idx);

				T* obj = static_cast<T*>(duk_get_pointer(ctx, -1));

				duk_pop(ctx);  // pop obj_ptr

				return obj;
			}

			// read reference
			template<typename FullT, typename = typename std::enable_if< std::is_reference<FullT>::value>::type >
			static T& read(duk_context* ctx, duk_idx_t arg_idx) {
				T* obj = read<T*>(ctx, arg_idx);
				if (obj == nullptr)
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: cannot be null (native function expects reference)", arg_idx);

				return *obj;
			}

			// read value
			// commented out because it breaks for abstract classes
			/*template<typename FullT, typename = typename std::enable_if< std::is_same<T, typename std::remove_const<FullT>::type >::value>::type >
			static T read(duk_context* ctx, duk_idx_t arg_idx) {
				static_assert(std::is_copy_constructible<T>::value, "Reading a value requires a copy-constructable type");
				const T& obj = read<T&>(ctx, arg_idx);
				return T(obj);
			}*/

			// -----------------------------------------------------
			// Writing

			// Reference
			template<typename FullT, typename = typename std::enable_if< std::is_reference<FullT>::value>::type >
			static void push(duk_context* ctx, T& value) {
				using namespace dukglue::detail;

				if (!RefManager::find_and_push_native_object(ctx, &value)) {
					// need to create new script object
					ProtoManager::make_script_object<T>(ctx, &value);
					RefManager::register_native_object(ctx, &value);
				}
			}

			// Pointer
			template<typename FullT, typename = typename std::enable_if< std::is_pointer<FullT>::value>::type >
			static void push(duk_context* ctx, T* value) {
				if (value == nullptr)
					duk_push_null(ctx);
				else
					push<T&>(ctx, *value);
			}

			// Value (create new instance on the heap)
			// commented out because this is an easy way to accidentally cause a memory leak
			/*template<typename FullT, typename = typename std::enable_if< std::is_same<T, typename std::remove_const<FullT>::type >::value>::type >
			static void push(duk_context* ctx, T value) {
				static_assert(std::is_copy_constructible<T>::value, "Cannot push value for non-copy-constructable type.");
				return push<T*>(ctx, new T(value));
			}*/
		};

		// Figure out what the type for an argument should be inside the tuple.
		// If a function expects a reference to a value type, we need temporary storage for the value.
		// For example, a reference to a value type (const int&) will need to be temporarily
		// stored in the tuple, so ArgStorage<const int&>::type == int.
		// Native objects are already allocated on the heap, so there's no problem storing, say, const Dog& in the tuple.
		template<typename T>
		struct ArgStorage {
		private:
			typedef typename Bare<T>::type BareType;
			//typedef DukType<BareType> ThisDukType;
			typedef typename DukType<BareType>::IsValueType IsValueType;
		
			static_assert(!IsValueType::value || !std::is_pointer<T>::value, "Cannot return pointer to value type.");
			static_assert(!IsValueType::value ||
				(!std::is_reference<T>::value || std::is_const<typename std::remove_reference<T>::type>::value),
				"Value types can only be returned as const references.");

		public:
			typedef typename std::conditional<IsValueType::value, BareType, T>::type type;
		};
	}
}

#include "detail_primitive_types.h"

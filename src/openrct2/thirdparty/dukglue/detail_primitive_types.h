#pragma once

#include "detail_types.h"
#include "detail_typeinfo.h"
#include "dukvalue.h"

#include <vector>
#include <stdint.h>
#include <memory>  // for std::shared_ptr

namespace dukglue {
	namespace types {

#define DUKGLUE_SIMPLE_VALUE_TYPE(TYPE, DUK_IS_FUNC, DUK_GET_FUNC, DUK_PUSH_FUNC, PUSH_VALUE) \
		template<> \
		struct DukType<TYPE> { \
			typedef std::true_type IsValueType; \
			\
			template<typename FullT> \
			static TYPE read(duk_context* ctx, duk_idx_t arg_idx) { \
				if (DUK_IS_FUNC(ctx, arg_idx)) { \
					return static_cast<TYPE>(DUK_GET_FUNC(ctx, arg_idx)); \
				} else { \
					duk_int_t type_idx = duk_get_type(ctx, arg_idx); \
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected " #TYPE ", got %s", arg_idx, detail::get_type_name(type_idx)); \
				} \
			} \
			\
			template<typename FullT> \
			static void push(duk_context* ctx, TYPE value) { \
				DUK_PUSH_FUNC(ctx, PUSH_VALUE); \
			} \
		};

		DUKGLUE_SIMPLE_VALUE_TYPE(bool, duk_is_boolean, 0 != duk_get_boolean, duk_push_boolean, value)

		DUKGLUE_SIMPLE_VALUE_TYPE(uint8_t,  duk_is_number, duk_get_uint, duk_push_uint, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(uint16_t, duk_is_number, duk_get_uint, duk_push_uint, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(uint32_t, duk_is_number, duk_get_uint, duk_push_uint, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(uint64_t, duk_is_number, duk_get_number, duk_push_number, value) // have to cast to double

		DUKGLUE_SIMPLE_VALUE_TYPE(int8_t,  duk_is_number, duk_get_int, duk_push_int, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(int16_t, duk_is_number, duk_get_int, duk_push_int, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(int32_t, duk_is_number, duk_get_int, duk_push_int, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(int64_t, duk_is_number, duk_get_number, duk_push_number, value) // have to cast to double

		// signed char and unsigned char are surprisingly *both* different from char, at least in MSVC
		DUKGLUE_SIMPLE_VALUE_TYPE(char, duk_is_number, duk_get_int, duk_push_int, value)

		DUKGLUE_SIMPLE_VALUE_TYPE(float, duk_is_number, duk_get_number, duk_push_number, value)
		DUKGLUE_SIMPLE_VALUE_TYPE(double, duk_is_number, duk_get_number, duk_push_number, value)

		DUKGLUE_SIMPLE_VALUE_TYPE(std::string, duk_is_string, duk_get_string, duk_push_string, value.c_str())

		// We have to do some magic for const char* to work correctly.
		// We override the "bare type" and "storage type" to both be const char*.
		// char* is a bit tricky because its "bare type" should still be const char*, to differentiate it from just char
		template<>
		struct Bare<char*> {
			typedef const char* type;
		};
		template<>
		struct Bare<const char*> {
			typedef const char* type;
		};

		// the storage type should also be const char* - if we don't do this, it will end up as just "char"
		template<>
		struct ArgStorage<const char*> {
			typedef const char* type;
		};

		template<>
		struct DukType<const char*> {
			typedef std::true_type IsValueType;

			template<typename FullT>
			static const char* read(duk_context* ctx, duk_idx_t arg_idx) {
				if (duk_is_string(ctx, arg_idx)) {
					return duk_get_string(ctx, arg_idx);
				} else {
					duk_int_t type_idx = duk_get_type(ctx, arg_idx);
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected string, got %s", arg_idx, detail::get_type_name(type_idx));
				}
			}

			template<typename FullT>
			static void push(duk_context* ctx, const char* value) {
				duk_push_string(ctx, value);
			}
		};

		// DukValue
		template<>
		struct DukType<DukValue> {
			typedef std::true_type IsValueType;

			template <typename FullT>
			static DukValue read(duk_context* ctx, duk_idx_t arg_idx) {
				try {
					return DukValue::copy_from_stack(ctx, arg_idx);
				} catch (DukException& e) {
					// only DukException can be thrown by DukValue::copy_from_stack
					duk_error(ctx, DUK_ERR_ERROR, e.what());
				}
			}

			template <typename FullT>
			static void push(duk_context* ctx, const DukValue& value) {
				if (value.context() == NULL) {
					duk_error(ctx, DUK_ERR_ERROR, "DukValue is uninitialized");
					return;
				}

				if (value.context() != ctx) {
					duk_error(ctx, DUK_ERR_ERROR, "DukValue comes from a different context");
					return;
				}

				try {
					value.push();
				} catch (DukException& e) {
					// only DukException can be thrown by DukValue::copy_from_stack
					duk_error(ctx, DUK_ERR_ERROR, e.what());
				}
			}
		};

		// std::vector (as value)
		template<typename T>
		struct DukType< std::vector<T> > {
			typedef std::true_type IsValueType;

			template <typename FullT>
			static std::vector<T> read(duk_context* ctx, duk_idx_t arg_idx) {
				if (!duk_is_array(ctx, arg_idx)) {
					duk_int_t type_idx = duk_get_type(ctx, arg_idx);
					duk_error(ctx, DUK_ERR_TYPE_ERROR, "Argument %d: expected array, got %s", arg_idx, detail::get_type_name(type_idx));
				}

				duk_size_t len = duk_get_length(ctx, arg_idx);
				const duk_idx_t elem_idx = duk_get_top(ctx);

				std::vector<T> vec;
				vec.reserve(len);
				for (duk_size_t i = 0; i < len; i++) {
					duk_get_prop_index(ctx, arg_idx, i);
					vec.push_back(DukType< typename Bare<T>::type >::template read<T>(ctx, elem_idx));
					duk_pop(ctx);
				}
				return vec;
			}

			template <typename FullT>
			static void push(duk_context* ctx, const std::vector<T>& value) {
				duk_idx_t obj_idx = duk_push_array(ctx);

				for (size_t i = 0; i < value.size(); i++) {
					DukType< typename Bare<T>::type >::template push<T>(ctx, value[i]);
					duk_put_prop_index(ctx, obj_idx, i);
				}
			}
		};

		// std::shared_ptr (as value)
		template<typename T>
		struct DukType< std::shared_ptr<T> > {
			typedef std::true_type IsValueType;

			static_assert(std::is_same<typename DukType<T>::IsValueType, std::false_type>::value, "Dukglue can only use std::shared_ptr to non-value types!");

			template <typename FullT>
			static std::shared_ptr<T> read(duk_context* ctx, duk_idx_t arg_idx) {
				if (duk_is_null(ctx, arg_idx))
					return nullptr;

				if (!duk_is_object(ctx, arg_idx)) {
					duk_int_t type_idx = duk_get_type(ctx, arg_idx);
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected shared_ptr object, got ", arg_idx, detail::get_type_name(type_idx));
				}

				duk_get_prop_string(ctx, arg_idx, "\xFF" "type_info");
				if (!duk_is_pointer(ctx, -1))  // missing type_info, must not be a native object
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected shared_ptr object (missing type_info)", arg_idx);

				// make sure this object can be safely returned as a T*
				dukglue::detail::TypeInfo* info = static_cast<dukglue::detail::TypeInfo*>(duk_get_pointer(ctx, -1));
				if (!info->can_cast<T>())
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: wrong type of shared_ptr object", arg_idx);
				duk_pop(ctx);  // pop type_info

				duk_get_prop_string(ctx, arg_idx, "\xFF" "shared_ptr");
				if (!duk_is_pointer(ctx, -1))
					duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: not a shared_ptr object (missing shared_ptr)", arg_idx);
				void* ptr = duk_get_pointer(ctx, -1);
				duk_pop(ctx);  // pop pointer to shared_ptr

				return *((std::shared_ptr<T>*) ptr);
			}

			static duk_ret_t shared_ptr_finalizer(duk_context* ctx)
			{
				duk_get_prop_string(ctx, 0, "\xFF" "shared_ptr");
				std::shared_ptr<T>* ptr = (std::shared_ptr<T>*) duk_require_pointer(ctx, -1);
				duk_pop(ctx);  // pop shared_ptr ptr

				if (ptr != NULL) {
					delete ptr;

					// for safety, set the pointer to undefined
					// (finalizers can run multiple times)
					duk_push_undefined(ctx);
					duk_put_prop_string(ctx, 0, "\xFF" "shared_ptr");
				}

				return 0;
			}

			template <typename FullT>
			static void push(duk_context* ctx, const std::shared_ptr<T>& value) {
				if (value == nullptr) {
					duk_push_null(ctx);
				} else {
					dukglue::detail::ProtoManager::make_script_object(ctx, value.get());

					// create + set shared_ptr
					duk_push_pointer(ctx, new std::shared_ptr<T>(value));
					duk_put_prop_string(ctx, -2, "\xFF" "shared_ptr");

					// set shared_ptr finalizer
					duk_push_c_function(ctx, &shared_ptr_finalizer, 1);
					duk_set_finalizer(ctx, -2);
				}
			}
		};

		// std::function
		/*template <typename RetT, typename... ArgTs>
		struct DukType< std::function<RetT(ArgTs...)> > {
			typedef std::true_type IsValueType;

			template<typename FullT>
			static std::function<RetT(ArgTs...)> read(duk_context* ctx, duk_idx_t arg_idx) {
				DukValue callable = DukValue::copy_from_stack(ctx, -1, DUK_TYPE_MASK_OBJECT);
				return [ctx, callable] (ArgTs... args) -> RetT {
					dukglue_call<RetT>(ctx, callable, args...);
				};
			}

			template<typename FullT>
			static void push(duk_context* ctx, std::function<RetT(ArgTs...)> value) {
				static_assert(false, "Pushing an std::function has not been implemented yet. Sorry!");
			}
		};*/
	}
}

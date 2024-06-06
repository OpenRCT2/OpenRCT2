#pragma once

#include <duktape.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "dukexception.h"

// A variant class for Duktape values.
// This class is not really dependant on the rest of dukglue, but the rest of dukglue is integrated to support it.
// Script objects are persisted by copying a reference to the object into an array in the heap stash.
// When we need to push a reference to the object, we just look up that reference in the stash.

// DukValues can be copied freely. We use reference counting behind the scenes to keep track of when we need
// to remove our reference from the heap stash. Memory for reference counting is only allocated once a DukValue
// is copied (either by copy constructor or operator=). std::move can be used if you are trying to avoid ref counting
// for some reason.

// One script object can have multiple, completely separate DukValues "pointing" to it - in this case, there will be
// multiple entries in the "ref array" that point to the same object. This will happen if the same script object is
// put on the stack and turned into a DukValue multiple times independently (copy-constructing/operator=-ing 
// DukValues will not do this!). This is okay, as we are only keeping track of these objects to prevent garbage
// collection (and access them later). This could be changed to use a map structure to look up one canonical entry per
// script object in the "ref array" (I guess it would be more like a ref map in this case), but this would require a map
// lookup every time we construct a DukValue. The performance difference probably isn't *that* noticeable (a good map
// would probably be amortized constant-time lookup), but I am guessing constructing many separate DukValues that point
// to the same script object isn't a very common thing.
class DukValue {
public:
	enum Type : uint8_t {
		//NONE = DUK_TYPE_NONE,
		UNDEFINED = DUK_TYPE_UNDEFINED,
		NULLREF = DUK_TYPE_NULL,
		BOOLEAN = DUK_TYPE_BOOLEAN,
		NUMBER = DUK_TYPE_NUMBER,
		STRING = DUK_TYPE_STRING,
		OBJECT = DUK_TYPE_OBJECT,
		BUFFER = DUK_TYPE_BUFFER,  // not implemented
		POINTER = DUK_TYPE_POINTER,
		LIGHTFUNC = DUK_TYPE_LIGHTFUNC  // not implemented
	};

	// default constructor just makes an undefined-type DukValue
	inline DukValue() : mContext(NULL), mType(UNDEFINED), mRefCount(NULL) {}

	virtual ~DukValue() {
		// release any references we have
		release_ref_count();
	}

	// move constructor
	inline DukValue(DukValue&& move) {
		mContext = move.mContext;
		mType = move.mType;
		mPOD = move.mPOD;
		mRefCount = move.mRefCount;

		if (mType == STRING)
			mString = std::move(move.mString);

		move.mType = UNDEFINED;
		move.mRefCount = NULL;
	}

	inline DukValue& operator=(const DukValue& rhs) {
		// free whatever we had
		release_ref_count();

		// copy things
		mContext = rhs.mContext;
		mType = rhs.mType;
		mPOD = rhs.mPOD;

		if (mType == STRING)
			mString = rhs.mString;

		if (mType == OBJECT)
		{
			// ref counting increment
			if (rhs.mRefCount == NULL) {
				// not ref counted before, need to allocate memory
				const_cast<DukValue&>(rhs).mRefCount = new int(2);
				mRefCount = rhs.mRefCount;
			} else {
				// already refcounting, just increment
				mRefCount = rhs.mRefCount;
				*mRefCount = *mRefCount + 1;
			}
		}

		return *this;
	}

	// copy constructor
	inline DukValue(const DukValue& copy) : DukValue() {
		*this = copy;
	}

	// equality operator
	inline bool operator==(const DukValue& rhs) const
	{
		if (mType != rhs.mType || mContext != rhs.mContext)
			return false;

		switch (mType) {
		case UNDEFINED:
		case NULLREF:
			return true;
		case BOOLEAN:
			return mPOD.boolean == rhs.mPOD.boolean;
		case NUMBER:
			return mPOD.number == rhs.mPOD.number;
		case STRING:
			return mString == rhs.mString;

		case OBJECT:
		{
			// this could be optimized to only push ref_array once...
			this->push();
			rhs.push();
			bool equal = duk_equals(mContext, -1, -2) ? true : false;
			duk_pop_2(mContext);
			return equal;
		}

		case POINTER:
			return mPOD.pointer == rhs.mPOD.pointer;

		case BUFFER:
		case LIGHTFUNC:
		default:
			throw DukException() << "operator== not implemented (" << type_name() << ")";
		}
	}

	inline bool operator!=(const DukValue& rhs) const {
		return !(*this == rhs);
	}

	// copies the object at idx on the stack into a new DukValue and returns it
	static DukValue copy_from_stack(duk_context* ctx, duk_idx_t idx = -1) {
		DukValue value;
		value.mContext = ctx;
		value.mType = (Type) duk_get_type(ctx, idx);
		switch (value.mType) {
		case UNDEFINED:
			break;

		case NULLREF:
			value.mPOD.pointer = NULL;
			break;

		case BOOLEAN:
			value.mPOD.boolean = duk_require_boolean(ctx, idx) ? true : false;
			break;

		case NUMBER:
			value.mPOD.number = duk_require_number(ctx, idx);
			break;

		case STRING:
		{
			duk_size_t len;
			const char* data = duk_get_lstring(ctx, idx, &len);
			value.mString.assign(data, len);
			break;
		}

		case OBJECT:
			value.mPOD.ref_array_idx = stash_ref(ctx, idx);
			break;

		case POINTER:
			value.mPOD.pointer = duk_require_pointer(ctx, idx);
			break;

		case BUFFER:
		case LIGHTFUNC:
		default:
			throw DukException() << "Cannot turn type into DukValue (" << value.type_name() << ")";
		}

		return value;
	}

protected:
	static duk_ret_t json_decode_safe(duk_context* ctx, void* user_data)
	{
		duk_json_decode(ctx, -1);
		return 1;
	}

public:
	static_assert(sizeof(char) == 1, "Serialization probably broke");
	static DukValue deserialize(duk_context* ctx, const char* data, size_t data_len) {
		DukValue v;
		v.mContext = ctx;
		v.mType = *((Type*)data);

		const char* data_ptr = data + sizeof(Type);
		data_len -= sizeof(Type);

		switch (v.mType) {
		case UNDEFINED:
		case NULLREF:
			break;

		case BOOLEAN:
		{
			if (data_len < 1)
				throw DukException() << "Malformed boolean data";

			v.mPOD.boolean = data[1] == 1 ? true : false;
			break;
		}

		case NUMBER:
		{
			if (data_len < sizeof(double))
				throw DukException() << "Malformed number data";

			v.mPOD.number = *((double*)data_ptr);
			break;
		}

		case STRING:
		{
			if (data_len < sizeof(uint32_t))
				throw DukException() << "Malformed string data (no length)";
			uint32_t str_len = *((uint32_t*)data_ptr);

			if (data_len < sizeof(uint32_t) + str_len)
				throw DukException() << "Malformed string data (appears truncated)";

			const char* str_data = (data_ptr + sizeof(uint32_t));
			v.mString.assign(str_data, str_len);
			break;
		}

		case OBJECT:
		{
			if (data_len < sizeof(uint32_t))
				throw DukException() << "Malformed object JSON data (no length)";
			uint32_t json_len = *((uint32_t*)data_ptr);

			if (data_len < sizeof(uint32_t) + json_len)
				throw DukException() << "Malformed object JSON data (appears truncated)";

			const char* json_data = (data_ptr + sizeof(uint32_t));
			duk_push_lstring(ctx, json_data, json_len);
			int rc = duk_safe_call(ctx, &json_decode_safe, NULL, 1, 1);
			if (rc) {
				throw DukErrorException(ctx, rc) << "Could not decode JSON";
			} else {
				v.mPOD.ref_array_idx = stash_ref(ctx, -1);
				duk_pop(ctx);
			}
			break;
		}

		default:
			throw DukException() << "not implemented";
		}

		return v;
	}

	// same as above (copy_from_stack), but also removes the value we copied from the stack
	static DukValue take_from_stack(duk_context* ctx, duk_idx_t idx = -1) {
		DukValue val = copy_from_stack(ctx, idx);
		duk_remove(ctx, idx);
		return val;
	}

	// push the value we hold onto the stack
	inline void push() const {
		duk_context* ctx = mContext;

		switch (mType) {
		case UNDEFINED:
			duk_push_undefined(ctx);
			break;
		case NULLREF:
			duk_push_null(ctx);
			break;

		case BOOLEAN:
			duk_push_boolean(ctx, mPOD.boolean);
			break;

		case NUMBER:
			duk_push_number(ctx, mPOD.number);
			break;

		case STRING:
			duk_push_lstring(ctx, mString.data(), mString.size());
			break;

		case OBJECT:
			push_ref_array(ctx);
			duk_get_prop_index(ctx, -1, mPOD.ref_array_idx);
			duk_remove(ctx, -2);
			break;

		case POINTER:
			duk_push_pointer(ctx, mPOD.pointer);
			break;

		case BUFFER:
		case LIGHTFUNC:
		default:
			throw DukException() << "DukValue.push() not implemented for type (" << type_name() << ")";
		}
	}

	// various (type-safe) getters
	inline bool as_bool() const {
		if (mType != BOOLEAN)
			throw DukException() << "Expected boolean, got " << type_name();
		return mPOD.boolean;
	}

	inline double as_double() const {
		if (mType != NUMBER)
			throw DukException() << "Expected number, got " << type_name();
		return mPOD.number;
	}

	inline float as_float() const {
		if (mType != NUMBER)
			throw DukException() << "Expected number, got " << type_name();
		return static_cast<float>(mPOD.number);
	}

	inline duk_int_t as_int() const {
		if (mType != NUMBER)
			throw DukException() << "Expected number, got " << type_name();
		return static_cast<int32_t>(mPOD.number);
	}

	inline duk_uint_t as_uint() const {
		if (mType != NUMBER)
			throw DukException() << "Expected number, got " << type_name();
		return static_cast<uint32_t>(mPOD.number);
	}

	inline void* as_pointer() const {
		if (mType != POINTER && mType != NULLREF)
			throw DukException() << "Expected pointer or null, got " << type_name();
		return mPOD.pointer;
	}

	inline const std::string& as_string() const {
		if (mType != STRING)
			throw DukException() << "Expected string, got " << type_name();
		return mString;
	}

	inline const char* as_c_string() const {
		if (mType != STRING)
			throw DukException() << "Expected string, got " << type_name();
		return mString.data();
	}

	inline Type type() const {
		return mType;
	}

	// same as duk_get_type_name(), but that's internal to Duktape, so we shouldn't use it
	inline const char* type_name() const {
		switch (mType) {
		case UNDEFINED: return "undefined";
		case NULLREF: return "null";
		case BOOLEAN: return "boolean";
		case NUMBER: return "number";
		case STRING: return "string";
		case OBJECT: return "object";
		case BUFFER: return "buffer";
		case POINTER: return "pointer";
		case LIGHTFUNC: return "lightfunc";
		}
		return "?";
	}

	inline duk_context* context() const {
		return mContext;
	}

	// Important limitations:
	// - The returned value is binary and will not behave well if you treat it like a string (it will almost certainly contain '\0').
	//   If you need to transport it like a string, maybe try encoding it as base64.
	// - Strings longer than 2^32 (UINT32_MAX) characters will throw an exception. You can raise this to be a uint64_t if you need
	//   really long strings for some reason (be sure to change DukValue::deserialize() as well).
	// - Objects are encoded to JSON and then sent like a string. If your object can't be encoded as JSON (i.e. it's a function),
	//   this will not work. This can be done, but I chose not to because it poses a security issue if you deserializing untrusted data.
	//   If you require this functionality, you'll have to add it yourself with using duk_dump_function(...).
	static_assert(sizeof(char) == 1, "Serialization probably broke");
	std::vector<char> serialize() const {
		std::vector<char> buff;
		buff.resize(sizeof(Type));
		*((Type*)buff.data()) = mType;

		switch (mType) {
		case UNDEFINED:
		case NULLREF:
			break;

		case BOOLEAN:
		{
			buff.push_back(mPOD.boolean ? 1 : 0);
			break;
		}

		case NUMBER:
		{
			buff.resize(buff.size() + sizeof(double));
			*((double*)(buff.data() + sizeof(Type))) = mPOD.number;
			break;
		}

		case STRING:
		{
			if (mString.length() > static_cast<size_t>(UINT32_MAX))
				throw DukException() << "String length larger than uint32_t max";

			uint32_t len = mString.length();
			buff.resize(buff.size() + sizeof(uint32_t) + len);

			uint32_t* len_ptr = (uint32_t*)(buff.data() + sizeof(Type));
			*len_ptr = len;

			char* out_ptr = (char*)(buff.data() + sizeof(Type) + sizeof(uint32_t));
			strncpy(out_ptr, mString.data(), len);  // note: this will NOT be null-terminated
			break;
		}

		case OBJECT:
		{
			push();
			if (duk_is_function(mContext, -1)) {
				duk_pop(mContext);
				throw DukException() << "Functions cannot be serialized";
				// well, technically they can...see the comments at the start of this method
			}

			std::string json = duk_json_encode(mContext, -1);
			duk_pop(mContext);

			if (json.length() > static_cast<size_t>(UINT32_MAX))
				throw DukException() << "JSON length larger than uint32_t max";

			uint32_t len = json.length();
			buff.resize(buff.size() + sizeof(uint32_t) + len);

			uint32_t* len_ptr = (uint32_t*)(buff.data() + sizeof(Type));
			*len_ptr = len;

			char* out_ptr = (char*)(buff.data() + sizeof(Type) + sizeof(uint32_t));
			strncpy(out_ptr, json.data(), len);  // note: this will NOT be null-terminated
			break;
		}

		default:
			throw DukException() << "Type not implemented for serialization.";
		}

		return buff;
	}

private:
	// THIS IS COMPLETELY UNRELATED TO DETAIL_REFS.H.
	// detail_refs.h stores a mapping of native object -> script object.
	// This just stores arbitrary script objects (which likely have no native object backing them).
	// If I was smarter I might merge the two implementations, but this one is simpler
	// (since we don't need the std::map here).
	static void push_ref_array(duk_context* ctx)
	{
		static const char* DUKVALUE_REF_ARRAY = "dukglue_dukvalue_refs";
		duk_push_heap_stash(ctx);

		if (!duk_has_prop_string(ctx, -1, DUKVALUE_REF_ARRAY)) {
			duk_push_array(ctx);

			// ref_array[0] = 0 (initialize free list as empty)
			duk_push_int(ctx, 0);
			duk_put_prop_index(ctx, -2, 0);

			duk_put_prop_string(ctx, -2, DUKVALUE_REF_ARRAY);
		}

		duk_get_prop_string(ctx, -1, DUKVALUE_REF_ARRAY);
		duk_remove(ctx, -2); // pop heap stash
	}

	// put a new reference into the ref array and return its index in the array
	static duk_uint_t stash_ref(duk_context* ctx, duk_idx_t idx)
	{
		push_ref_array(ctx);

		// if idx is relative, we need to adjust it to deal with the array we just pushed
		if (idx < 0)
			idx--;

		// find next free index
		// free indices are kept in a linked list, starting at ref_array[0]
		duk_get_prop_index(ctx, -1, 0);
		duk_uarridx_t next_free_idx = duk_get_uint(ctx, -1);
		duk_pop(ctx);

		if (next_free_idx == 0) {
			// no free spots in the array, make a new one at arr.length
			next_free_idx = duk_get_length(ctx, -1);
		} else {
			// free spot found, need to remove it from the free list
			// ref_array[0] = ref_array[next_free_idx]
			duk_get_prop_index(ctx, -1, next_free_idx);
			duk_put_prop_index(ctx, -2, 0);
		}

		duk_dup(ctx, idx);  // copy value we are storing (since store consumes it)
		duk_put_prop_index(ctx, -2, next_free_idx);  // store it (consumes duplicated value)
		duk_pop(ctx);  // pop ref array

		return next_free_idx;
	}

	// remove ref_array_idx from the ref array and add its spot to the free list (at refs[0])
	static void free_ref(duk_context* ctx, duk_uarridx_t ref_array_idx)
	{
		push_ref_array(ctx);

		// add this spot to the free list
		// refs[old_obj_idx] = refs[0] (implicitly gives up our reference)
		duk_get_prop_index(ctx, -1, 0);
		duk_put_prop_index(ctx, -2, ref_array_idx);

		// refs[0] = old_obj_idx
		duk_push_uint(ctx, ref_array_idx);
		duk_put_prop_index(ctx, -2, 0);

		duk_pop(ctx);  // pop ref array
	}

	// this is for reference counting - used to release our reference based on the state
	// of mRefCount. If mRefCount is NULL, we never got copy constructed, so we have ownership
	// of our reference and can free it. If it's not null and above 1, we decrement the counter
	// (someone else owns the reference). If it's not null and equal to 1, we are the last owner
	// of a previously shared reference, so we can free it.
	void release_ref_count()
	{
		if (mType == OBJECT)
		{
			if (mRefCount != NULL)
			{
				// sharing with another DukValue, are we the only one left?
				if (*mRefCount > 1) {  // still someone else referencing this
					*mRefCount = *mRefCount - 1;
				} else {
					// not sharing anymore, we can free it
					free_ref(mContext, mPOD.ref_array_idx);
					delete mRefCount;
				}

				mRefCount = NULL;
			} else {
				// not sharing with any other DukValue, free it
				free_ref(mContext, mPOD.ref_array_idx);
			}

			mType = UNDEFINED;
		}
	}

	duk_context* mContext;
	Type mType;  // our type - one of the standard Duktape DUK_TYPE_* values

	// This holds the plain-old-data types. Since this is a variant,
	// we hold only one value at a time, so this is a union to save
	// a bit of space.
	union ValueTypes {
		bool boolean;
		double number;
		void* pointer;  // if mType == NULLREF, this is 0 (otherwise holds pointer value when mType == POINTER)
		duk_uarridx_t ref_array_idx;
	} mPOD;

	std::string mString;  // if it's a string, we store it with std::string
	int* mRefCount;  // if mType == OBJECT and we're sharing, this will point to our ref counter

public:
	DukValue operator [](const std::string_view &key) const
	{
		push();
		duk_get_prop_lstring(mContext, -1, key.data(), key.size());
		auto result = DukValue::take_from_stack(mContext);
		duk_pop(mContext);
        return result;
	}

	bool is_array() const
	{
		push();
		bool result = duk_is_array(mContext, -1);
		duk_pop(mContext);
		return result;
	}

	std::vector<DukValue> as_array() const
	{
		push();
		if (!duk_is_array(mContext, -1))
		{
			duk_pop(mContext);
			throw DukException() << "Expected array, got " << type_name();
		}

		auto arrayLength = duk_get_length(mContext, -1);
		std::vector<DukValue> result;
		result.reserve(arrayLength);
		for (size_t i = 0; i < arrayLength; i++)
		{
			duk_get_prop_index(mContext, -1, i);
			result.push_back(take_from_stack(mContext));
		}

		duk_pop(mContext);
		return result;
	}

	bool is_function() const
	{
		push();
		bool result = duk_is_function(mContext, -1);
		duk_pop(mContext);
		return result;
	}
};

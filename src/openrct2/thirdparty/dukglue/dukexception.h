#pragma once

#include <sstream>
#include <exception>

class DukException : public std::exception
{
public:
	virtual const char* what() const noexcept override
	{
		return mMsg.c_str();
	}

	template <typename T>
	DukException& operator<<(T rhs)
	{
		std::stringstream ss;
		ss << mMsg << rhs;
		mMsg = ss.str();
		return *this;
	}

protected:
	std::string mMsg;
};

class DukErrorException : public DukException
{
public:
	DukErrorException(duk_context* ctx, int return_code, bool pop_error = true) {
		if (return_code != 0) {
			duk_get_prop_string(ctx, -1, "stack");
			mMsg = duk_safe_to_string(ctx, -1);
			duk_pop(ctx);

			if (pop_error)
				duk_pop(ctx);
		}
	}
};
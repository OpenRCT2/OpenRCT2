#pragma once

#include <typeindex>

namespace dukglue
{
	namespace detail
	{
		// same as duk_get_type_name, which is private for some reason *shakes fist*
		static const char* get_type_name(duk_int_t type_idx) {
			static const char* names[] = {
				"none",
				"undefined",
				"null",
				"boolean",
				"number",
				"string",
				"object",
				"buffer",
				"pointer",
				"lightfunc"
			};

			if (type_idx >= 0 && type_idx < sizeof(names) / sizeof(names[0]))
				return names[type_idx];
			else
				return "unknown";
		}

		class TypeInfo
		{
		public:
			TypeInfo(std::type_index&& idx) : index_(idx), base_(nullptr) {}
			TypeInfo(const TypeInfo& rhs) : index_(rhs.index_), base_(rhs.base_) {}

			inline void set_base(TypeInfo* base) {
				base_ = base;
			}

			template<typename T>
			bool can_cast() const {
				if (index_ == typeid(T))
					return true;

				if (base_)
					return base_->can_cast<T>();

				return false;
			}

			inline bool operator<(const TypeInfo& rhs) const { return index_ < rhs.index_; }
			inline bool operator<=(const TypeInfo& rhs) const { return index_ <= rhs.index_; }
			inline bool operator>(const TypeInfo& rhs) const { return index_ > rhs.index_; }
			inline bool operator>=(const TypeInfo& rhs) const { return index_ >= rhs.index_; }
			inline bool operator==(const TypeInfo& rhs) const { return index_ == rhs.index_; }
			inline bool operator!=(const TypeInfo& rhs) const { return index_ != rhs.index_; }

		private:
			std::type_index index_;
			TypeInfo* base_;
		};
	}
}

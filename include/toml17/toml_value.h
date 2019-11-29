#pragma once
#include "toml_common.h"

namespace TOML17_NAMESPACE
{
	template <typename T>
	class value final
		: public node
	{
		static_assert(
			is_value_type<T>,
			"Template type argument must be one of the basic TOML value types"
		);

		private:
			template <typename T>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			value_ptr<T> value_ptr_from_base() const noexcept
			{
				return std::static_pointer_cast<node<T>>(shared_from_this());
			}

		public:
			T value;

			[[nodiscard]] bool is_value() const noexcept override { return true; }
			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_int() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_float() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_bool() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_datetime() const noexcept override { return std::is_same_v<T, datetime>; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_table_array() const noexcept override { return false; }

			[[nodiscard]]
			value_ptr<string> as_string() const noexcept override
			{
				if constexpr (std::is_same_v<T, string>)
					return value_ptr_from_base<string>();
				else
					return {};
			}

			[[nodiscard]]
			value_ptr<int64_t> as_int() const noexcept override
			{
				if constexpr (std::is_same_v<T, int64_t>)
					return value_ptr_from_base<int64_t>();
				else
					return {};
			}

			[[nodiscard]]
			value_ptr<double> as_float() const noexcept override
			{
				if constexpr (std::is_same_v<T, double>)
					return value_ptr_from_base<double>();
				else
					return {};
			}

			[[nodiscard]]
			value_ptr<bool> as_bool() const noexcept override
			{
				if constexpr (std::is_same_v<T, bool>)
					return value_ptr_from_base<bool>();
				else
					return {};
			}

			[[nodiscard]]
			value_ptr<datetime> as_datetime() const noexcept override
			{
				if constexpr (std::is_same_v<T, datetime>)
					return value_ptr_from_base<datetime>();
				else
					return {};
			}

			[[nodiscard]] std::shared_ptr<array> as_array() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table> as_table() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table_array> as_table_array() const noexcept override { return {}; }

			node(const T& val) noexcept
				: value{ val }
			{}

			node(T&& val) noexcept
				: value{ std::move(val) }
			{}

			~node() noexcept = default;
	};
}

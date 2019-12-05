#pragma once
#include "toml_common.h"

namespace TOML_NAMESPACE
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

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			std::shared_ptr<toml::value<U>> value_ptr_from_base() noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return std::static_pointer_cast<toml::value<U>>(shared_from_this());
				else
					return {};
			}

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			std::shared_ptr<const toml::value<U>> value_ptr_from_base() const noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return std::static_pointer_cast<const toml::value<U>>(shared_from_this());
				else
					return {};
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

			[[nodiscard]] std::shared_ptr<toml::value<string>> as_string() noexcept override { return value_ptr_from_base<string>(); };
			[[nodiscard]] std::shared_ptr<toml::value<int64_t>> as_int() noexcept override { return value_ptr_from_base<int64_t>(); };
			[[nodiscard]] std::shared_ptr<toml::value<double>> as_float() noexcept override { return value_ptr_from_base<double>(); };
			[[nodiscard]] std::shared_ptr<toml::value<bool>> as_bool() noexcept override { return value_ptr_from_base<bool>(); };
			[[nodiscard]] std::shared_ptr<toml::value<datetime>> as_datetime() noexcept override { return value_ptr_from_base<datetime>(); };
			[[nodiscard]] std::shared_ptr<array> as_array() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table> as_table() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table_array> as_table_array() noexcept override { return {}; }

			[[nodiscard]] std::shared_ptr<const toml::value<string>> as_string() const noexcept override { return value_ptr_from_base<string>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<int64_t>> as_int() const noexcept override { return value_ptr_from_base<int64_t>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<double>> as_float() const noexcept override { return value_ptr_from_base<double>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<bool>> as_bool() const noexcept override { return value_ptr_from_base<bool>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<datetime>> as_datetime() const noexcept override { return value_ptr_from_base<datetime>(); };
			[[nodiscard]] std::shared_ptr<const array> as_array() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const table> as_table() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const table_array> as_table_array() const noexcept override { return {}; }

			value(const T& val) noexcept
				: value{ val }
			{}

			value(T&& val) noexcept
				: value{ std::move(val) }
			{}

			~value() noexcept = default;
	};
}

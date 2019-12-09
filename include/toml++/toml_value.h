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

			T val_;

		public:

			[[nodiscard]] bool is_value() const noexcept override { return true; }
			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_int() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_float() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_bool() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_datetime() const noexcept override { return std::is_same_v<T, datetime>; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_table_array() const noexcept override { return false; }

			[[nodiscard]] std::shared_ptr<toml::value<string>> as_string() noexcept override { return value_ptr_from_base<string>(); };
			[[nodiscard]] std::shared_ptr<toml::value<int64_t>> as_int() noexcept override { return value_ptr_from_base<int64_t>(); };
			[[nodiscard]] std::shared_ptr<toml::value<double>> as_float() noexcept override { return value_ptr_from_base<double>(); };
			[[nodiscard]] std::shared_ptr<toml::value<bool>> as_bool() noexcept override { return value_ptr_from_base<bool>(); };
			[[nodiscard]] std::shared_ptr<toml::value<date>> as_date() noexcept override { return value_ptr_from_base<date>(); };
			[[nodiscard]] std::shared_ptr<toml::value<time>> as_time() noexcept override { return value_ptr_from_base<time>(); };
			[[nodiscard]] std::shared_ptr<toml::value<datetime>> as_datetime() noexcept override { return value_ptr_from_base<datetime>(); };
			[[nodiscard]] std::shared_ptr<array> as_array() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table> as_table() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table_array> as_table_array() noexcept override { return {}; }

			[[nodiscard]] std::shared_ptr<const toml::value<string>> as_string() const noexcept override { return value_ptr_from_base<string>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<int64_t>> as_int() const noexcept override { return value_ptr_from_base<int64_t>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<double>> as_float() const noexcept override { return value_ptr_from_base<double>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<bool>> as_bool() const noexcept override { return value_ptr_from_base<bool>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<date>> as_date() const noexcept override { return value_ptr_from_base<date>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<time>> as_time() const noexcept override { return value_ptr_from_base<time>(); };
			[[nodiscard]] std::shared_ptr<const toml::value<datetime>> as_datetime() const noexcept override { return value_ptr_from_base<datetime>(); };
			[[nodiscard]] std::shared_ptr<const array> as_array() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const table> as_table() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const table_array> as_table_array() const noexcept override { return {}; }

			[[nodiscard]] int type_id() const noexcept override
			{
					 if constexpr (std::is_same_v<T, string>) return 3;
				else if constexpr (std::is_same_v<T, int64_t>) return 4;
				else if constexpr (std::is_same_v<T, double>) return 5;
				else if constexpr (std::is_same_v<T, bool>) return 6;
				else if constexpr (std::is_same_v<T, date>) return 7;
				else if constexpr (std::is_same_v<T, time>) return 8;
				else if constexpr (std::is_same_v<T, datetime>) return 9;
			}

			template <typename U>
			value(U&& val) noexcept
				: val_{ std::forward<U>(val) }
			{}

			~value() noexcept = default;
	};

	value(const string_char*) -> value<string>;
	value(string_view) -> value<string>;
	value(string) -> value<string>;
	value(bool) -> value<bool>;
	value(float) -> value<double>;
	value(double) -> value<double>;
	value(int8_t) -> value<int64_t>;
	value(int16_t) -> value<int64_t>;
	value(int32_t) -> value<int64_t>;
	value(int64_t) -> value<int64_t>;
	value(uint8_t) -> value<int64_t>;
	value(uint16_t) -> value<int64_t>;
	value(uint32_t) -> value<int64_t>;
}

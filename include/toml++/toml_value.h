#pragma once
#include "toml_node.h"

namespace toml
{
	template <typename T>
	class value final
		: public node
	{
		static_assert(
			impl::is_value<T>,
			"Template type parameter must be one of the basic value types"
		);

		private:
			friend class impl::parser;

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			toml::value<U>* as_value() noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return this;
				else
					return nullptr;
			}

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const toml::value<U>* as_value() const noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return this;
				else
					return nullptr;
			}

			T val_;

		public:

			template <typename... U>
			TOML_NODISCARD_CTOR
			explicit value(U&&... args) TOML_CONDITIONAL_NOEXCEPT(std::is_nothrow_constructible_v<T, U &&...>)
				: val_{ std::forward<U>(args)... }
			{}

			TOML_NODISCARD_CTOR
			value(value&& other) noexcept
				: node{ std::move(other) },
				val_{ std::move(other.val_) }
			{}

			value& operator= (value&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				val_ = std::move(rhs.val_);
				return *this;
			}

			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_integer() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_floating_point() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_boolean() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_date_time() const noexcept override { return std::is_same_v<T, date_time>; }

			[[nodiscard]] value<string>* as_string() noexcept override { return as_value<string>(); }
			[[nodiscard]] value<int64_t>* as_integer() noexcept override { return as_value<int64_t>(); }
			[[nodiscard]] value<double>* as_floating_point() noexcept override { return as_value<double>(); }
			[[nodiscard]] value<bool>* as_boolean() noexcept override { return as_value<bool>(); }
			[[nodiscard]] value<date>* as_date() noexcept override { return as_value<date>(); }
			[[nodiscard]] value<time>* as_time() noexcept override { return as_value<time>(); }
			[[nodiscard]] value<date_time>* as_date_time() noexcept override { return as_value<date_time>(); }

			[[nodiscard]] const value<string>* as_string() const noexcept override { return as_value<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept override { return as_value<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept override { return as_value<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept override { return as_value<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return as_value<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return as_value<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept override { return as_value<date_time>(); }

			[[nodiscard]] node_type type() const noexcept override
			{
					 if constexpr (std::is_same_v<T, string>) return node_type::string;
				else if constexpr (std::is_same_v<T, int64_t>) return node_type::integer;
				else if constexpr (std::is_same_v<T, double>) return node_type::floating_point;
				else if constexpr (std::is_same_v<T, bool>) return node_type::boolean;
				else if constexpr (std::is_same_v<T, date>) return node_type::date;
				else if constexpr (std::is_same_v<T, time>) return node_type::time;
				else if constexpr (std::is_same_v<T, date_time>) return node_type::date_time;
			}

			[[nodiscard]] T& get() noexcept { return val_; }
			[[nodiscard]] const T& get() const noexcept { return val_; }
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

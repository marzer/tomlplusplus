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

			template <typename U, typename V>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static auto as_value([[maybe_unused]] V* ptr) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return ptr;
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

			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_value() const noexcept override { return true; }

			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_integer() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_floating_point() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_boolean() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_date_time() const noexcept override { return std::is_same_v<T, date_time>; }

			[[nodiscard]] value<string>* as_string() noexcept override { return as_value<string>(this); }
			[[nodiscard]] value<int64_t>* as_integer() noexcept override { return as_value<int64_t>(this); }
			[[nodiscard]] value<double>* as_floating_point() noexcept override { return as_value<double>(this); }
			[[nodiscard]] value<bool>* as_boolean() noexcept override { return as_value<bool>(this); }
			[[nodiscard]] value<date>* as_date() noexcept override { return as_value<date>(this); }
			[[nodiscard]] value<time>* as_time() noexcept override { return as_value<time>(this); }
			[[nodiscard]] value<date_time>* as_date_time() noexcept override { return as_value<date_time>(this); }

			[[nodiscard]] const value<string>* as_string() const noexcept override { return as_value<string>(this); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept override { return as_value<int64_t>(this); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept override { return as_value<double>(this); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept override { return as_value<bool>(this); }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return as_value<date>(this); }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return as_value<time>(this); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept override { return as_value<date_time>(this); }

			[[nodiscard]] T& get() & noexcept { return val_; }
			[[nodiscard]] T&& get() && noexcept { return std::move(val_); }
			[[nodiscard]] const T& get() const & noexcept { return val_; }

			[[nodiscard]] T& operator* () & noexcept { return val_; }
			[[nodiscard]] T&& operator* () && noexcept { return std::move(val_); }
			[[nodiscard]] const T& operator* () const& noexcept { return val_; }

			[[nodiscard]] operator T& () & noexcept { return val_; }
			[[nodiscard]] operator T&& () && noexcept { return std::move(val_); }
			[[nodiscard]] operator const T& () const& noexcept { return val_; }

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value& rhs) TOML_MAY_THROW
			{
				impl::print_to_stream(rhs.val_, lhs);
				return lhs;
			}
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

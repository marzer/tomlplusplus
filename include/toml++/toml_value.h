//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_node.h"
#include "toml_print_to_stream.h"

namespace toml
{
	template <typename CHAR, typename T>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const value<T>&);

	/// \brief	A TOML value.
	/// 		
	/// \tparam	T	The value's data type. Can be one of:
	/// 			- toml::string
	/// 			- int64_t
	/// 			- double
	/// 			- bool
	/// 			- toml::date
	/// 			- toml::time
	/// 			- toml::date_time
	template <typename T>
	class TOML_API value final : public node
	{
		static_assert(
			impl::is_value<T>,
			"Template type parameter must be one of the TOML value types"
		);

		private:
			friend class TOML_PARSER_TYPENAME;

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

			/// \brief	The value's underlying data type.
			using value_type = T;

			/// \brief	A type alias for 'value arguments'.
			/// \details This differs according to the value's type argument:
			/// 		 - ints, floats, booleans: `value_type`
			/// 		 - strings: `string_view`
			/// 		 - everything else: `const value_type&`
			using value_arg = std::conditional_t<
				std::is_same_v<T, string>,
				string_view,
				std::conditional_t<impl::is_one_of<T, double, int64_t, bool>, T, const T&>
			>;

			/// \brief	Constructs a toml value.
			///
			/// \tparam	U	Constructor argument types.
			/// \param 	args	Arguments to forward to the internal value's constructor.
			template <typename... U>
			TOML_NODISCARD_CTOR
			explicit value(U&&... args) noexcept(std::is_nothrow_constructible_v<T, U&&...>)
				: val_{ std::forward<U>(args)... }
			{}

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			value(value&& other) noexcept
				: node{ std::move(other) },
				val_{ std::move(other.val_) }
			{}

			/// \brief	Move-assignment operator.
			value& operator= (value&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				val_ = std::move(rhs.val_);
				return *this;
			}

			value(const value&) = delete;
			value& operator= (const value&) = delete;

			/// \brief	Returns the value's node type identifier.
			///
			/// \returns	One of:
			/// 			- node_type::string
			/// 			- node_type::integer
			/// 			- node_type::floating_point
			/// 			- node_type::boolean
			/// 			- node_type::date
			/// 			- node_type::time
			/// 			- node_type::date_time
			[[nodiscard]] node_type type() const noexcept override { return impl::node_type_of<T>; }

			/// \brief	Always returns `false` for value nodes.
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			/// \brief	Always returns `false` for value nodes.
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			/// \brief	Always returns `true` for value nodes.
			[[nodiscard]] bool is_value() const noexcept override { return true; }

			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_integer() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_floating_point() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_number() const noexcept override { return impl::is_one_of<T, int64_t, double>; }
			[[nodiscard]] bool is_boolean() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_date_time() const noexcept override { return std::is_same_v<T, date_time>; }

			/// \brief	Returns a pointer to the value if the data type is a string.
			[[nodiscard]] value<string>* as_string() noexcept override { return as_value<string>(this); }
			/// \brief	Returns a pointer to the value if the data type is an integer.
			[[nodiscard]] value<int64_t>* as_integer() noexcept override { return as_value<int64_t>(this); }
			/// \brief	Returns a pointer to the value if the data type is a floating-point.
			[[nodiscard]] value<double>* as_floating_point() noexcept override { return as_value<double>(this); }
			/// \brief	Returns a pointer to the value if the data type is boolean.
			[[nodiscard]] value<bool>* as_boolean() noexcept override { return as_value<bool>(this); }
			/// \brief	Returns a pointer to the value if the data type is a date.
			[[nodiscard]] value<date>* as_date() noexcept override { return as_value<date>(this); }
			/// \brief	Returns a pointer to the value if the data type is a time.
			[[nodiscard]] value<time>* as_time() noexcept override { return as_value<time>(this); }
			/// \brief	Returns a pointer to the value if the data type is date-time.
			[[nodiscard]] value<date_time>* as_date_time() noexcept override { return as_value<date_time>(this); }

			/// \brief	Returns a const pointer to the value if the data type is a string.
			[[nodiscard]] const value<string>* as_string() const noexcept override { return as_value<string>(this); }
			/// \brief	Returns a const pointer to the value if the data type is an integer.
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept override { return as_value<int64_t>(this); }
			/// \brief	Returns a const pointer to the value if the data type is a floating-point.
			[[nodiscard]] const value<double>* as_floating_point() const noexcept override { return as_value<double>(this); }
			/// \brief	Returns a const pointer to the value if the data type is a boolean.
			[[nodiscard]] const value<bool>* as_boolean() const noexcept override { return as_value<bool>(this); }
			/// \brief	Returns a const pointer to the value if the data type is a date.
			[[nodiscard]] const value<date>* as_date() const noexcept override { return as_value<date>(this); }
			/// \brief	Returns a const pointer to the value if the data type is a time.
			[[nodiscard]] const value<time>* as_time() const noexcept override { return as_value<time>(this); }
			/// \brief	Returns a const pointer to the value if the data type is a date-time.
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept override { return as_value<date_time>(this); }

			/// \brief	Returns a reference to the underlying value.
			[[nodiscard]] T& get() & noexcept { return val_; }
			/// \brief	Returns a reference to the underlying value (rvalue overload).
			[[nodiscard]] T&& get() && noexcept { return std::move(val_); }
			/// \brief	Returns a reference to the underlying value (const overload).
			[[nodiscard]] const T& get() const & noexcept { return val_; }

			/// \brief	Returns a reference to the underlying value.
			[[nodiscard]] T& operator* () & noexcept { return val_; }
			/// \brief	Returns a reference to the underlying value (rvalue overload).
			[[nodiscard]] T&& operator* () && noexcept { return std::move(val_); }
			/// \brief	Returns a reference to the underlying value (const overload).
			[[nodiscard]] const T& operator* () const& noexcept { return val_; }

			/// \brief	Returns a reference to the underlying value.
			[[nodiscard]] explicit operator T& () & noexcept { return val_; }
			/// \brief	Returns a reference to the underlying value (rvalue overload).
			[[nodiscard]] explicit operator T&& () && noexcept { return std::move(val_); }
			/// \brief	Returns a reference to the underlying value (const overload).
			[[nodiscard]] explicit operator const T& () const& noexcept { return val_; }

			template <typename CHAR, typename U>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value<U>& rhs);

			/// \brief	Value-assignment operator.
			value& operator= (value_arg rhs) noexcept
			{
				if constexpr (std::is_same_v<T, string>)
					val_.assign(rhs);
				else
					val_ = rhs;
				return *this;
			}

			template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, string>>>
			value& operator= (string&& rhs) noexcept
			{
				val_ = std::move(rhs);
				return *this;
			}

			/// \brief	Value equality operator.
			[[nodiscard]] friend bool operator == (const value& lhs, value_arg rhs) noexcept { return lhs.val_ == rhs; }
			TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, )

			/// \brief	Value less-than operator.
			[[nodiscard]] friend bool operator <  (const value& lhs, value_arg rhs) noexcept { return lhs.val_ < rhs; }
			/// \brief	Value less-than operator.
			[[nodiscard]] friend bool operator <  (value_arg lhs, const value& rhs) noexcept { return lhs < rhs.val_; }
			/// \brief	Value less-than-or-equal-to operator.
			[[nodiscard]] friend bool operator <= (const value& lhs, value_arg rhs) noexcept { return lhs.val_ <= rhs; }
			/// \brief	Value less-than-or-equal-to operator.
			[[nodiscard]] friend bool operator <= (value_arg lhs, const value& rhs) noexcept { return lhs <= rhs.val_; }

			/// \brief	Value greater-than operator.
			[[nodiscard]] friend bool operator >  (const value& lhs, value_arg rhs) noexcept { return lhs.val_ > rhs; }
			/// \brief	Value greater-than operator.
			[[nodiscard]] friend bool operator >  (value_arg lhs, const value& rhs) noexcept { return lhs > rhs.val_; }
			/// \brief	Value greater-than-or-equal-to operator.
			[[nodiscard]] friend bool operator >= (const value& lhs, value_arg rhs) noexcept { return lhs.val_ >= rhs; }
			/// \brief	Value greater-than-or-equal-to operator.
			[[nodiscard]] friend bool operator >= (value_arg lhs, const value& rhs) noexcept { return lhs >= rhs.val_; }

			/// \brief	Equality operator.
			///
			/// \param 	lhs	The LHS value.
			/// \param 	rhs	The RHS value.
			///
			/// \returns	True if the values were of the same type and contained the same value.
			template <typename U>
			[[nodiscard]] friend bool operator == (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ == rhs.val_;
				else
					return false;
			}

			/// \brief	Inequality operator.
			///
			/// \param 	lhs	The LHS value.
			/// \param 	rhs	The RHS value.
			///
			/// \returns	True if the values were not of the same type, or did not contain the same value.
			template <typename U>
			[[nodiscard]] friend bool operator != (const value& lhs, const value<U>& rhs) noexcept
			{
				return !(lhs == rhs);
			}

			/// \brief	Less-than operator.
			///
			/// \param 	lhs	The LHS toml::value.
			/// \param 	rhs	The RHS toml::value.
			///
			/// \returns	<strong><em>Same value types:</em></strong> `lhs.get() < rhs.get()` <br>
			/// 			<strong><em>Different value types:</em></strong> `lhs.type() < rhs.type()`
			template <typename U>
			[[nodiscard]] friend bool operator < (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ < rhs.val_;
				else
					return impl::node_type_of<T> < impl::node_type_of<U>;
			}

			/// \brief	Less-than-or-equal-to operator.
			///
			/// \param 	lhs	The LHS toml::value.
			/// \param 	rhs	The RHS toml::value.
			///
			/// \returns	<strong><em>Same value types:</em></strong> `lhs.get() <= rhs.get()` <br>
			/// 			<strong><em>Different value types:</em></strong> `lhs.type() <= rhs.type()`
			template <typename U>
			[[nodiscard]] friend bool operator <= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ <= rhs.val_;
				else
					return impl::node_type_of<T> <= impl::node_type_of<U>;
			}

			/// \brief	Greater-than operator.
			///
			/// \param 	lhs	The LHS toml::value.
			/// \param 	rhs	The RHS toml::value.
			///
			/// \returns	<strong><em>Same value types:</em></strong> `lhs.get() > rhs.get()` <br>
			/// 			<strong><em>Different value types:</em></strong> `lhs.type() > rhs.type()`
			template <typename U>
			[[nodiscard]] friend bool operator > (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ > rhs.val_;
				else
					return impl::node_type_of<T> > impl::node_type_of<U>;
			}

			/// \brief	Greater-than-or-equal-to operator.
			///
			/// \param 	lhs	The LHS toml::value.
			/// \param 	rhs	The RHS toml::value.
			///
			/// \returns	<strong><em>Same value types:</em></strong> `lhs.get() >= rhs.get()` <br>
			/// 			<strong><em>Different value types:</em></strong> `lhs.type() >= rhs.type()`
			template <typename U>
			[[nodiscard]] friend bool operator >= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ >= rhs.val_;
				else
					return impl::node_type_of<T> >= impl::node_type_of<U>;
			}
	};

	#if !TOML_ALL_INLINE
	extern template class TOML_API value<string>;
	extern template class TOML_API value<int64_t>;
	extern template class TOML_API value<double>;
	extern template class TOML_API value<bool>;
	extern template class TOML_API value<date>;
	extern template class TOML_API value<time>;
	extern template class TOML_API value<date_time>;
	#endif

	template <size_t N> value(const string_char(&)[N]) -> value<string>;
	template <size_t N> value(const string_char(&&)[N]) -> value<string>;
	value(const string_char*) -> value<string>;
	template <size_t N> value(string_char(&)[N]) -> value<string>;
	template <size_t N> value(string_char(&&)[N]) -> value<string>;
	value(string_char*) -> value<string>;
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
	value(date) -> value<date>;
	value(time) -> value<time>;
	value(date_time) -> value<date_time>;
	#ifdef TOML_SMALL_FLOAT_TYPE
	value(TOML_SMALL_FLOAT_TYPE) -> value<double>;
	#endif
	#ifdef TOML_SMALL_INT_TYPE
	value(TOML_SMALL_INT_TYPE) -> value<int64_t>;
	#endif

	/// \brief	Prints the value out to a stream.
	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value<T>& rhs)
	{
		// this is the same behaviour as default_formatter, but it's so simple that there's
		// no need to spin up a new instance of it just for individual values.

		if constexpr (std::is_same_v<T, string>)
		{
			impl::print_to_stream('"', lhs);
			impl::print_to_stream_with_escapes(rhs.val_, lhs);
			impl::print_to_stream('"', lhs);
		}
		else
			impl::print_to_stream(rhs.val_, lhs);
				
		return lhs;
	}

	template <typename T>
	inline optional<T> node::value() const noexcept
	{
		static_assert(
			impl::is_value<T> || std::is_same_v<T, string_view>,
			"Value type must be one of the TOML value types (or string_view)"
		);

		switch (type())
		{
			case node_type::none:	[[fallthrough]];
			case node_type::table:	[[fallthrough]];
			case node_type::array:
				return {};

			case node_type::string:
			{
				if constexpr (std::is_same_v<T, string> || std::is_same_v<T, string_view>)
					return { T{ ref_cast<string>().get() } };
				else
					return {};
			}

			case node_type::integer:
			{
				if constexpr (std::is_same_v<T, int64_t>)
					return ref_cast<int64_t>().get();
				else if constexpr (std::is_same_v<T, double>)
					return static_cast<double>(ref_cast<int64_t>().get());
				else
					return {};
			}

			case node_type::floating_point:
			{
				if constexpr (std::is_same_v<T, double>)
					return ref_cast<double>().get();
				else if constexpr (std::is_same_v<T, int64_t>)
					return static_cast<int64_t>(ref_cast<double>().get());
				else
					return {};
			}

			case node_type::boolean:
			{
				if constexpr (std::is_same_v<T, bool>)
					return ref_cast<bool>().get();
				else
					return {};
			}

			case node_type::date:
			{
				if constexpr (std::is_same_v<T, date>)
					return ref_cast<date>().get();
				else
					return {};
			}

			case node_type::time:
			{
				if constexpr (std::is_same_v<T, time>)
					return ref_cast<time>().get();
				else
					return {};
			}

			case node_type::date_time:
			{
				if constexpr (std::is_same_v<T, date_time>)
					return ref_cast<date_time>().get();
				else
					return {};
			}

			TOML_NO_DEFAULT_CASE;
		}

		TOML_UNREACHABLE;
	}

	template <typename T>
	inline auto node::value_or(T&& default_value) const noexcept
	{
		static_assert(
			impl::is_value_or_promotable<impl::remove_cvref_t<T>>,
			"Default value type must be (or be promotable to) one of the TOML value types"
		);

		using value_type = impl::promoted<impl::remove_cvref_t<T>>;
		using return_type = std::conditional_t<
			std::is_same_v<value_type, string>,
			std::conditional_t<std::is_same_v<impl::remove_cvref_t<T>, string>, string, string_view>,
			value_type
		>;

		if (auto val = this->value<return_type>())
			return *val;
		return return_type{ std::forward<T>(default_value) };
	}
}


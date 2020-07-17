//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_node.h"
#include "toml_print_to_stream.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_FLOAT_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	template <typename Char, typename T>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const value<T>&);

	/// \brief	A TOML value.
	/// 		
	/// \tparam	T	The value's data type. Can be one of:
	/// 			- toml::string
	/// 			- toml::date
	/// 			- toml::time
	/// 			- toml::date_time
	/// 			- int64_t
	/// 			- double
	/// 			- bool
	template <typename T>
	class TOML_API value final : public node
	{
		static_assert(
			impl::is_native<T>,
			"A toml::value<> must model one of the native TOML value types:"
			TOML_NATIVE_VALUE_TYPE_LIST
		);

		private:
			friend class TOML_PARSER_TYPENAME;

			template <typename U, typename V>
			[[nodiscard]]
			TOML_ALWAYS_INLINE
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

			/// \brief	Prints the value out to a stream as formatted TOML.
			template <typename Char, typename U>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const value<U>& rhs);

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
			[[nodiscard]]
			friend bool operator == (const value& lhs, value_arg rhs) noexcept
			{
				if constexpr (std::is_same_v<value_type, double>)
				{
					using namespace ::toml::impl;
					static constexpr auto pack = [](auto l, auto r) constexpr noexcept
					{
						return (static_cast<uint64_t>(unbox_enum(l)) << 32)
							| static_cast<uint64_t>(unbox_enum(r));
					};
					
					switch (pack(impl::fpclassify(lhs.val_), impl::fpclassify(rhs)))
					{
						case pack(fp_class::pos_inf, fp_class::neg_inf):	[[fallthrough]];
						case pack(fp_class::pos_inf, fp_class::nan):		[[fallthrough]];
						case pack(fp_class::neg_inf, fp_class::pos_inf):	[[fallthrough]];
						case pack(fp_class::neg_inf, fp_class::nan):		[[fallthrough]];
						case pack(fp_class::nan, fp_class::pos_inf):		[[fallthrough]];
						case pack(fp_class::nan, fp_class::neg_inf):
							return false;

						case pack(fp_class::pos_inf, fp_class::pos_inf):	[[fallthrough]];
						case pack(fp_class::neg_inf, fp_class::neg_inf):	[[fallthrough]];
						case pack(fp_class::nan, fp_class::nan):
							return true;

						case pack(fp_class::ok, fp_class::ok):
							return lhs.val_ == rhs;

						TOML_NO_DEFAULT_CASE;
					}

					TOML_UNREACHABLE;
				}
				else
					return lhs.val_ == rhs;

			}
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
			[[nodiscard]]
			friend bool operator == (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs == rhs.val_; //calls asymmetrical value-equality operator defined above
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
			[[nodiscard]]
			friend bool operator != (const value& lhs, const value<U>& rhs) noexcept
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
			[[nodiscard]]
			friend bool operator < (const value& lhs, const value<U>& rhs) noexcept
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
			[[nodiscard]]
			friend bool operator <= (const value& lhs, const value<U>& rhs) noexcept
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
			[[nodiscard]]
			friend bool operator > (const value& lhs, const value<U>& rhs) noexcept
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
			[[nodiscard]]
			friend bool operator >= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ >= rhs.val_;
				else
					return impl::node_type_of<T> >= impl::node_type_of<U>;
			}
	};

	#if !TOML_ALL_INLINE
		TOML_PUSH_WARNINGS
		TOML_DISABLE_VTABLE_WARNINGS
		extern template class TOML_API value<string>;
		extern template class TOML_API value<int64_t>;
		extern template class TOML_API value<double>;
		extern template class TOML_API value<bool>;
		extern template class TOML_API value<date>;
		extern template class TOML_API value<time>;
		extern template class TOML_API value<date_time>;
		TOML_POP_WARNINGS
	#endif
	
	template <typename T>
	value(T) -> value<impl::native_type_of<impl::remove_cvref_t<T>>>;

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS
	TOML_DISABLE_SWITCH_WARNINGS

	template <typename T>
	[[nodiscard]]
	inline decltype(auto) node::get_value_exact() const noexcept
	{
		using namespace ::toml::impl;

		static_assert(node_type_of<T> != node_type::none);
		static_assert(node_type_of<T> != node_type::table);
		static_assert(node_type_of<T> != node_type::array);
		static_assert(is_native<T> || can_represent_native<T>);
		TOML_ASSERT(this->type() == node_type_of<T>);

		if constexpr (node_type_of<T> == node_type::string)
		{
			const auto& str = *ref_cast<string>();
			if constexpr (std::is_same_v<T, string>)
				return str;
			else if constexpr (std::is_same_v<T, string_view>)
				return T{ str };
			else if constexpr (std::is_same_v<T, const string_char*>)
				return str.c_str();

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
				#if TOML_WINDOWS_COMPAT
				return widen(str);
				#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
				#endif
			}

			// char8_t -> char (safe)
			else if constexpr (is_one_of<T, std::string, std::string_view>)
				return T(reinterpret_cast<const char*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char*>)
				return reinterpret_cast<const char*>(str.c_str());

			#ifdef __cpp_lib_char8_t
			// char -> char8_t (potentially unsafe - the feature is 'experimental'!)
			else if constexpr (is_one_of<T, std::u8string, std::u8string_view>)
				return T(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char8_t*>)
				return reinterpret_cast<const char8_t*>(str.c_str());
			else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
			#endif
		}
		else
			return static_cast<T>(*ref_cast<native_type_of<T>>());
	}


	template <typename T>
	inline optional<T> node::value_exact() const noexcept
	{
		using namespace ::toml::impl;

		static_assert(
			!is_wide_string<T> || TOML_WINDOWS_COMPAT,
			"Retrieving values as wide-character strings with node::value_exact() is only "
			"supported on Windows with TOML_WINDOWS_COMPAT enabled."
		);
		static_assert(
			is_native<T> || can_represent_native<T>,
			"The return type of node::value_exact() must be one of the following:"
			"\n|"
			"\n| A native TOML value type"
			TOML_NATIVE_VALUE_TYPE_LIST
			"\n|"
			"\n| A non-view type capable of losslessly representing a native TOML value type"
			#if TOML_CHAR_8_STRINGS
			"\n| - std::string"
			#elif defined __cpp_lib_char8_t
			"\n| - std::u8string"
			#endif
			#if TOML_WINDOWS_COMPAT
			"\n| - std::wstring"
			#endif
			"\n| - any signed integer type >= 64 bits"
			"\n| - any floating-point type >= 64 bits of precision"
			"\n|"
			"\n| An immutable view type not requiring additional temporary storage"
			"\n| - std::string_view"
			"\n| - const char*"
			#ifdef __cpp_lib_char8_t
			"\n| - std::u8string_view"
			"\n| - const char8_t*"
			#endif
		);

		if (type() == node_type_of<T>)
			return { this->get_value_exact<T>() };
		else
			return {};
	}

	template <typename T>
	inline optional<T> node::value() const noexcept
	{
		using namespace ::toml::impl;

		static_assert(
			!is_wide_string<T> || TOML_WINDOWS_COMPAT,
			"Retrieving values as wide-character strings with node::value() is only "
			"supported on Windows with TOML_WINDOWS_COMPAT enabled."
		);
		static_assert(
			is_native<T> || can_represent_native<T> || can_partially_represent_native<T>,
			"The return type of node::value() must be one of the following:"
			"\n|"
			"\n| A native TOML value type"
			TOML_NATIVE_VALUE_TYPE_LIST
			"\n|"
			"\n| A non-view type capable of losslessly representing a native TOML value type"
			#if TOML_CHAR_8_STRINGS
			"\n| - std::string"
			#elif defined __cpp_lib_char8_t
			"\n| - std::u8string"
			#endif
			#if TOML_WINDOWS_COMPAT
			"\n| - std::wstring"
			#endif
			"\n| - any signed integer type >= 64 bits"
			"\n| - any floating-point type >= 64 bits of precision"
			"\n|"
			"\n| A non-view type capable of (reasonably) representing a native TOML value type"
			"\n| - any other integer type"
			"\n| - any floating-point type >= 32 bits of precision"
			"\n|"
			"\n| An immutable view type not requiring additional temporary storage"
			"\n| - std::string_view"
			"\n| - const char*"
			#ifdef __cpp_lib_char8_t
			"\n| - std::u8string_view"
			"\n| - const char8_t*"
			#endif
		);

		// when asking for strings, dates, times and date_times there's no 'fuzzy' conversion
		// semantics to be mindful of so the exact retrieval is enough.
		if constexpr (is_natively_one_of<T, string, time, date, date_time>)
		{
			if (type() == node_type_of<T>)
				return { this->get_value_exact<T>() };
			else
				return {};
		}

		// everything else requires a bit of logicking.
		else
		{
			switch (type())
			{
				// int -> *
				case node_type::integer:
				{
					// int -> int
					if constexpr (is_natively_one_of<T, int64_t>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return static_cast<T>(*ref_cast<int64_t>());
						else
						{
							using traits = value_traits<T>;
							const int64_t val = *ref_cast<int64_t>();
							if constexpr (!traits::is_signed)
							{
								if constexpr ((sizeof(T) * CHAR_BIT) <= 53) // 53 bits < int64_max < 54 bits
								{
									using common_t = decltype(int64_t{} + T{});
									if (val < int64_t{} || static_cast<common_t>(val) > static_cast<common_t>(traits::max))
										return {};
								}
								else
								{
									if (val < int64_t{})
										return {};
								}
							}
							else
							{
								if (val < traits::min || val > traits::max)
									return {};
							}
							return { static_cast<T>(val) };
						}
					}

					// int -> float
					else if constexpr (is_natively_one_of<T, double>)
					{
						const int64_t val = *ref_cast<int64_t>();
						if constexpr (std::numeric_limits<T>::digits < 64)
						{
							constexpr auto largest_whole_float = (int64_t{ 1 } << std::numeric_limits<T>::digits);
							if (val < -largest_whole_float || val > largest_whole_float)
								return {};
						}
						return static_cast<T>(val);
					}

					// int -> bool
					else if constexpr (is_natively_one_of<T, bool>)
						return static_cast<bool>(*ref_cast<int64_t>());

					// int -> anything else
					else
						return {};
				}

				// float -> *
				case node_type::floating_point:
				{
					// float -> float
					if constexpr (is_natively_one_of<T, double>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return { static_cast<T>(*ref_cast<double>()) };
						else
						{
							const double val = *ref_cast<double>();
							if (val < (std::numeric_limits<T>::lowest)() || val > (std::numeric_limits<T>::max)())
								return {};
							return { static_cast<T>(val) };
						}
					}

					// float -> anything else
					else
						return {};
				}

				// bool -> *
				case node_type::boolean:
				{
					// bool -> bool
					if constexpr (is_natively_one_of<T, bool>)
						return { *ref_cast<bool>() };

					// bool -> int
					else if constexpr (is_natively_one_of<T, int64_t>)
						return { static_cast<T>(*ref_cast<bool>()) };

					// bool -> anything else
					else
						return {};
				}
			}

			// non-values, or 'exact' types covered above
			return {};
		}
	}

	TOML_POP_WARNINGS

	template <typename T>
	inline auto node::value_or(T&& default_value) const noexcept
	{
		using namespace ::toml::impl;

		static_assert(
			!is_wide_string<T> || TOML_WINDOWS_COMPAT,
			"Retrieving values as wide-character strings with node::value_or() is only "
			"supported on Windows with TOML_WINDOWS_COMPAT enabled."
		);


		if constexpr (is_wide_string<T>)
		{
			#if TOML_WINDOWS_COMPAT

			if (type() == node_type::string)
				return widen(*ref_cast<string>());
			return std::wstring{ std::forward<T>(default_value) };

			#else

			static_assert(dependent_false<T>, "Evaluated unreachable branch!");

			#endif
		}
		else
		{
			using value_type = std::conditional_t<
				std::is_pointer_v<std::decay_t<T>>,
				std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
				std::decay_t<T>
			>;
			using traits = value_traits<value_type>;

			static_assert(
				traits::is_native || traits::can_represent_native || traits::can_partially_represent_native,
				"The default return value type of node::value_or() must be one of the following:"
				"\n|"
				"\n| A native TOML value type"
				TOML_NATIVE_VALUE_TYPE_LIST
				"\n| "
				"\n| A non-view type capable of losslessly representing a native TOML value type"
				#if TOML_CHAR_8_STRINGS
				"\n| - std::string"
				#elif defined __cpp_lib_char8_t
				"\n| - std::u8string"
				#endif
				#if TOML_WINDOWS_COMPAT
				"\n| - std::wstring"
				#endif
				"\n| - any signed integer type >= 64 bits"
				"\n| - any floating-point type >= 64 bits of precision"
				"\n|"
				"\n| A non-view type capable of (reasonably) representing a native TOML value type"
				"\n| - any other integer type"
				"\n| - any floating-point type >= 32 bits of precision"
				"\n|"
				"\n| A compatible view type"
				"\n| - std::string_view"
				"\n| - const char*"
				"\n| - const char[]        (deduced as const char*)"
				"\n| - char*               (deduced as const char*)"
				#ifdef __cpp_lib_char8_t
				"\n| - std::u8string_view"
				"\n| - const char8_t*"
				"\n| - const char8_t[]     (deduced as const char8_t*)"
				"\n| - char8_t*            (deduced as const char8_t*)"
				#endif
				#if TOML_WINDOWS_COMPAT
				"\n| - std::wstring_view   (promoted to std::wstring)"
				"\n| - const wchar_t*      (promoted to std::wstring)"
				"\n| - const wchar_t[]     (promoted to std::wstring)"
				"\n| - wchar_t*            (promoted to std::wstring)"
				#endif
			);

			if constexpr (traits::is_native)
			{
				if (type() == node_type_of<value_type>)
					return *ref_cast<typename traits::native_type>();
				return std::forward<T>(default_value);
			}
			else
			{
				if (auto val = this->value<value_type>())
					return *val;
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return std::forward<T>(default_value);
			}
		}
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_FLOAT_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

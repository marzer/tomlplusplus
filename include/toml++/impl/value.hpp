//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "date_time.hpp"
#include "node.hpp"
#include "print_to_stream.hpp"
#include "std_utility.hpp"
#include "header_start.hpp"

/// \cond
// clang-format off

#if TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_SA_VALUE_MESSAGE_WSTRING			TOML_SA_LIST_SEP "std::wstring"
#else
	#define TOML_SA_VALUE_MESSAGE_WSTRING
#endif
#if TOML_HAS_CHAR8
	#define TOML_SA_VALUE_MESSAGE_U8STRING			TOML_SA_LIST_SEP "std::u8string"
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW		TOML_SA_LIST_SEP "std::u8string_view"
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8		TOML_SA_LIST_SEP "const char8_t*"
#else
	#define TOML_SA_VALUE_MESSAGE_U8STRING
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#endif
#if TOML_ENABLE_CHRONO
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS															\
		TOML_SA_LIST_SEP "std::chrono::year_month_day"														\
		TOML_SA_LIST_SEP "any std::chrono::hh_mm_ss or std::chrono::local_time of nanoseconds or finer"
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY																\
		TOML_SA_LIST_SEP "any other std::chrono::hh_mm_ss or std::chrono::local_time"
#else
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY
#endif

#define TOML_SA_VALUE_MESSAGE_LOSSLESS																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_U8STRING																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS																	\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_LOSSY																			\
	TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"		\
	TOML_SA_LIST_BEG "any other integral type"																\
	TOML_SA_LIST_SEP "any floating-point type"																\
	TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY																		\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_VIEW																			\
	TOML_SA_LIST_NXT "A compatible view type"																\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_STRING_LIKE																	\
	TOML_SA_LIST_NXT "A user-defined 'string-like' type (see toml::is_string_like)"							\
	TOML_SA_LIST_BEG "with const char* data() const or const char* c_str() const"							\
	TOML_SA_LIST_SEP "and size_t length() const"															\
	TOML_SA_LIST_SEP "and constructible from std::string_view, (const char*, size_t) or const std::string&"	\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_EXACT_FUNC_MESSAGE(type_arg)															\
	"The " type_arg " must be one of:"																		\
	TOML_SA_VALUE_MESSAGE_LOSSLESS																			\
	TOML_SA_VALUE_MESSAGE_VIEW																				\
	TOML_SA_VALUE_MESSAGE_STRING_LIKE

#define TOML_SA_VALUE_FUNC_MESSAGE(type_arg)																\
	"The " type_arg " must be one of:"																		\
	TOML_SA_VALUE_MESSAGE_LOSSLESS																			\
	TOML_SA_VALUE_MESSAGE_LOSSY																				\
	TOML_SA_VALUE_MESSAGE_VIEW																				\
	TOML_SA_VALUE_MESSAGE_STRING_LIKE

// clang-format on
/// \endcond

/// \cond
TOML_IMPL_NAMESPACE_START
{
	template <typename T, typename...>
	struct native_value_maker
	{
		template <typename... Args>
		TOML_NODISCARD
		static T make(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
		{
			// user-defined string-likes (see toml::is_string_like) provide their character data via data()/c_str()
			// + length() when they have no conversion to std::string, or one via const char* (it stops at a nul)
			if constexpr (std::is_same_v<T, std::string> && sizeof...(Args) == 1u && (is_string_like<Args> && ...)
						  && (!std::is_constructible_v<std::string, Args&&...>
							  || ((std::is_convertible_v<Args&&, const char*>
								   && !std::is_convertible_v<Args&&, std::string>)
								  && ...)))
				return std::string{ string_like_to_view(args)... };
			else
				return T(static_cast<Args&&>(args)...);
		}
	};

	template <typename T>
	struct native_value_maker<T, T>
	{
		template <typename U>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static U&& make(U&& val) noexcept
		{
			return static_cast<U&&>(val);
		}
	};

#if TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	struct string_maker
	{
		template <typename T>
		TOML_NODISCARD
		static std::string make(T&& arg)
		{
			using arg_type = std::decay_t<T>;
#if TOML_HAS_CHAR8
			if constexpr (is_one_of<arg_type, char8_t*, const char8_t*>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg)));
			}
			if constexpr (is_one_of<arg_type, std::u8string, std::u8string_view>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg.data())),
								   arg.length());
			}
#endif

#if TOML_ENABLE_WINDOWS_COMPAT
			if constexpr (is_wide_string<arg_type>)
			{
				return narrow(static_cast<T&&>(arg));
			}
#endif
		}
	};

#if TOML_HAS_CHAR8
	template <>
	struct native_value_maker<std::string, char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string_view> : string_maker
	{};
#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT
	template <>
	struct native_value_maker<std::string, wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring_view> : string_maker
	{};
#endif // TOML_ENABLE_WINDOWS_COMPAT

#endif // TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_integer_cast(int64_t val) noexcept
	{
		static_assert(node_type_of<T> == node_type::integer);
		static_assert(!is_cvref<T>);

		using traits = value_traits<T>;
		if constexpr (!traits::is_signed)
		{
			if constexpr ((sizeof(T) * CHAR_BIT) < 63) // 63 bits == int64_max
			{
				if (val < int64_t{} || val > static_cast<int64_t>(traits::max))
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

	enum class TOML_CLOSED_ENUM numeric_order : int
	{
		less,
		equal,
		greater,
		unordered
	};

	// the first value past the int64_t range; its negative is the lowest int64_t
	inline constexpr double two_to_the_63 = 9223372036854775808.0;

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_value_from_integer([[maybe_unused]] int64_t val) noexcept
	{
		// int -> int
		if constexpr (is_natively_one_of<T, int64_t>)
		{
			if constexpr (is_native<T> || can_represent_native<T>)
				return static_cast<T>(val);
			else
				return node_integer_cast<T>(val);
		}

		// int -> float
		else if constexpr (is_natively_one_of<T, double>)
		{
			if constexpr (value_traits<T>::digits < 64)
			{
				constexpr auto largest_whole_float = (int64_t{ 1 } << value_traits<T>::digits);
				if (val < -largest_whole_float || val > largest_whole_float)
					return {};
			}
			return static_cast<T>(val);
		}

		// int -> bool
		else if constexpr (is_natively_one_of<T, bool>)
			return static_cast<bool>(val);

		// int -> anything else
		else
			return {};
	}

	// by reference, as fpclassify() takes it
	template <typename T>
	TOML_PURE_GETTER
	inline optional<T> node_value_from_float([[maybe_unused]] const double& val) noexcept
	{
		// float -> float
		if constexpr (is_natively_one_of<T, double>)
		{
			if constexpr (is_native<T> || can_represent_native<T>)
				return { static_cast<T>(val) };
			else
			{
				if (fpclassify(val) == fp_class::ok
					&& (val < static_cast<double>(float_limits<T>::lowest())
						|| val > static_cast<double>(float_limits<T>::highest())))
					return {};
				return { static_cast<T>(val) };
			}
		}

		// float -> int
		else if constexpr (is_natively_one_of<T, int64_t>)
		{
			if (fpclassify(val) == fp_class::ok && val >= -two_to_the_63 && val < two_to_the_63
				&& static_cast<double>(static_cast<int64_t>(val)) == val)
				return node_integer_cast<T>(static_cast<int64_t>(val));
			return {};
		}

		// float -> anything else
		else
			return {};
	}

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_value_from_boolean([[maybe_unused]] bool val) noexcept
	{
		// bool -> bool
		if constexpr (is_natively_one_of<T, bool>)
			return { val };

		// bool -> int
		else if constexpr (is_natively_one_of<T, int64_t>)
			return { static_cast<T>(val) };

		// bool -> anything else
		else
			return {};
	}

	// compares as numbers: converting either operand to the other's type can change its value
	template <typename T>
	TOML_CONST_GETTER
	inline numeric_order compare_integer_with_float(int64_t lhs, T rhs) noexcept
	{
		static_assert(std::is_floating_point_v<T>);

		// a float16 can't hold 2^63
		using wide			 = std::common_type_t<T, double>;
		constexpr auto limit = static_cast<wide>(two_to_the_63);
		const auto widened	 = static_cast<wide>(rhs);
		if (widened >= limit)
			return numeric_order::less;
		if (widened < -limit)
			return numeric_order::greater;
		if (!(widened >= -limit))
			return numeric_order::unordered; // nan

		const auto whole = static_cast<int64_t>(widened);
		if (lhs != whole)
			return lhs < whole ? numeric_order::less : numeric_order::greater;

		const wide fraction = widened - static_cast<wide>(whole);
		if (fraction > wide{})
			return numeric_order::less;
		if (fraction < wide{})
			return numeric_order::greater;
		return numeric_order::equal;
	}

#if TOML_ENABLE_CHRONO

	// a local time can't hold a date-time that has an offset
	template <typename T>
	TOML_PURE_GETTER
	inline optional<T> to_chrono_local_time(const date_time& dt) noexcept
	{
		if (dt.offset)
			return {};
		return dt.template to_chrono<typename T::duration>();
	}

#endif

	template <typename...>
	struct value_variadic_ctor_allowed : std::true_type
	{};

	template <typename T, typename... Args>
	struct value_variadic_ctor_allowed<value<T>, value<T>, Args...> : std::false_type
	{};
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	A TOML value.
	///
	/// \tparam	ValueType	The value's native TOML data type. Can be one of:
	/// 						- std::string
	/// 						- toml::date
	/// 						- toml::time
	/// 						- toml::date_time
	/// 						- int64_t
	/// 						- double
	/// 						- bool
	template <typename ValueType>
	class value : public node
	{
		static_assert(impl::is_native<ValueType> && !impl::is_cvref<ValueType>,
					  "A toml::value<> must model one of the native TOML value types:" TOML_SA_NATIVE_VALUE_TYPE_LIST);

	  private:
		/// \cond

		friend class TOML_PARSER_TYPENAME;

		ValueType val_;
		value_flags flags_ = {};

		template <typename T, typename U>
		TOML_CONST_INLINE_GETTER
		static auto as_value([[maybe_unused]] U* ptr) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return ptr;
			else
				return nullptr;
		}

		/// \endcond

	  public:
		/// \brief	The value's underlying data type.
		using value_type = ValueType;

		/// \brief	A type alias for 'value arguments'.
		/// \details This differs according to the value's type argument:
		/// 		 - ints, floats, booleans: `value_type`
		/// 		 - strings: `string_view`
		/// 		 - everything else: `const value_type&`
		using value_arg = POXY_IMPLEMENTATION_DETAIL(
			std::conditional_t<
				std::is_same_v<value_type, std::string>,
				std::string_view,
				std::conditional_t<impl::is_one_of<value_type, double, int64_t, bool>, value_type, const value_type&>>);

		/// \brief	Constructs a toml value.
		///
		/// \tparam	Args	Constructor argument types.
		/// \param 	args	Arguments to forward to the internal value's constructor.
		TOML_HIDDEN_CONSTRAINT(
			(impl::value_variadic_ctor_allowed<value<ValueType>, impl::remove_cvref<Args>...>::value),
			typename... Args)
		TOML_NODISCARD_CTOR
		explicit value(Args&&... args)
#if !TOML_DISABLE_NOEXCEPT_NOEXCEPT
			noexcept(noexcept(value_type(
				impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))))
#endif
			: val_(impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		/// \brief	Constructs a toml value with the given flags.
		///
		/// \tparam	T		The initial value's type.
		/// \param 	val		The initial value, forwarded to the internal value's constructor.
		/// \param 	flags	The value's flags. toml::preserve_source_value_flags leaves them empty.
		TOML_HIDDEN_CONSTRAINT((impl::value_variadic_ctor_allowed<value<ValueType>, impl::remove_cvref<T>>::value),
							   typename T)
		TOML_NODISCARD_CTOR
		explicit value(T&& val, value_flags flags)
#if !TOML_DISABLE_NOEXCEPT_NOEXCEPT
			noexcept(noexcept(
				value_type(impl::native_value_maker<value_type, std::decay_t<T>>::make(static_cast<T&&>(val)))))
#endif
			: val_(impl::native_value_maker<value_type, std::decay_t<T>>::make(static_cast<T&&>(val))),
			  flags_{ flags == preserve_source_value_flags ? value_flags{} : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		/// \brief	Copy constructor.
		TOML_NODISCARD_CTOR
		value(const value& other) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		/// \brief	Copy constructor with flags override.
		TOML_NODISCARD_CTOR
		value(const value& other, value_flags flags) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		// NOLINTBEGIN(bugprone-use-after-move)
		/// \brief	Move constructor.
		TOML_NODISCARD_CTOR
		value(value&& other) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ std::exchange(other.flags_, value_flags{}) }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		/// \brief	Move constructor with flags override.
		TOML_NODISCARD_CTOR
		value(value&& other, value_flags flags) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
			other.flags_ = {};
		}

		/// \brief	Copy-assignment operator.
		value& operator=(const value& rhs) noexcept
		{
			node::operator=(rhs);
			val_   = rhs.val_;
			flags_ = rhs.flags_;
			return *this;
		}

		/// \brief	Move-assignment operator.
		value& operator=(value&& rhs) noexcept
		{
			if (&rhs != this)
			{
				node::operator=(std::move(rhs));
				val_   = std::move(rhs.val_);
				flags_ = std::exchange(rhs.flags_, value_flags{});
			}
			return *this;
		}
		// NOLINTEND(bugprone-use-after-move)

#if TOML_LIFETIME_HOOKS
		~value() noexcept
		{
			TOML_VALUE_DESTROYED;
		}
#endif

		/// \name Type checks
		/// @{

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
		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype) const noexcept final
		{
			return ntype == node_type::none || ntype == impl::node_type_of<value_type>;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final
		{
			if (!is_homogeneous(ntype))
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final
		{
			if (!is_homogeneous(ntype))
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		/// \cond
		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of value::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_void_v<type>)
				return true;
			else
				return impl::node_type_of<type> == impl::node_type_of<value_type>;
		}
		/// \endcond

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		/// \brief Returns `true`.
		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return true;
		}

		/// \brief Returns `true` if the #value_type is std::string.
		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return std::is_same_v<value_type, std::string>;
		}

		/// \brief Returns `true` if the #value_type is int64_t.
		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return std::is_same_v<value_type, int64_t>;
		}

		/// \brief Returns `true` if the #value_type is `double`.
		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return std::is_same_v<value_type, double>;
		}

		/// \brief Returns `true` if the #value_type is int64_t or `double`.
		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return impl::is_one_of<value_type, int64_t, double>;
		}

		/// \brief Returns `true` if the #value_type is `bool`.
		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return std::is_same_v<value_type, bool>;
		}

		/// \brief Returns `true` if the #value_type is toml::date.
		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return std::is_same_v<value_type, date>;
		}

		/// \brief Returns `true` if the #value_type is toml::time.
		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return std::is_same_v<value_type, time>;
		}

		/// \brief Returns `true` if the #value_type is toml::date_time.
		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return std::is_same_v<value_type, date_time>;
		}

		/// @}

		/// \name Type casts
		/// @{

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns a pointer to the value if it is a value<std::string>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<std::string>* as_string() noexcept final
		{
			return as_value<std::string>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<int64_t>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<int64_t>* as_integer() noexcept final
		{
			return as_value<int64_t>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<double>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<double>* as_floating_point() noexcept final
		{
			return as_value<double>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<bool>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<bool>* as_boolean() noexcept final
		{
			return as_value<bool>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<date>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<date>* as_date() noexcept final
		{
			return as_value<date>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<time>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<time>* as_time() noexcept final
		{
			return as_value<time>(this);
		}

		/// \brief Returns a pointer to the value if it is a value<date_time>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		value<date_time>* as_date_time() noexcept final
		{
			return as_value<date_time>(this);
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<std::string>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<std::string>* as_string() const noexcept final
		{
			return as_value<std::string>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<int64_t>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<int64_t>* as_integer() const noexcept final
		{
			return as_value<int64_t>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<double>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<double>* as_floating_point() const noexcept final
		{
			return as_value<double>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<bool>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<bool>* as_boolean() const noexcept final
		{
			return as_value<bool>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<date>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<date>* as_date() const noexcept final
		{
			return as_value<date>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<time>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<time>* as_time() const noexcept final
		{
			return as_value<time>(this);
		}

		/// \brief Returns a const-qualified pointer to the value if it is a value<date_time>, otherwise `nullptr`.
		TOML_CONST_INLINE_GETTER
		const value<date_time>* as_date_time() const noexcept final
		{
			return as_value<date_time>(this);
		}

		/// @}

		/// \name Value retrieval
		/// @{

		/// \brief	Returns a reference to the underlying value.
		TOML_PURE_INLINE_GETTER
		value_type& get() & noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (rvalue overload).
		TOML_PURE_INLINE_GETTER
		value_type&& get() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		/// \brief	Returns a reference to the underlying value (const overload).
		TOML_PURE_INLINE_GETTER
		const value_type& get() const& noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (const rvalue overload).
		TOML_PURE_INLINE_GETTER
		const value_type&& get() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		/// \brief	Returns a reference to the underlying value.
		TOML_PURE_INLINE_GETTER
		value_type& operator*() & noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (rvalue overload).
		TOML_PURE_INLINE_GETTER
		value_type&& operator*() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		/// \brief	Returns a reference to the underlying value (const overload).
		TOML_PURE_INLINE_GETTER
		const value_type& operator*() const& noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (const rvalue overload).
		TOML_PURE_INLINE_GETTER
		const value_type&& operator*() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		/// \brief	Returns a reference to the underlying value.
		TOML_PURE_INLINE_GETTER
		explicit operator value_type&() & noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (rvalue overload).
		TOML_PURE_INLINE_GETTER
		explicit operator value_type&&() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		/// \brief	Returns a reference to the underlying value (const overload).
		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&() const& noexcept
		{
			return val_;
		}

		/// \brief	Returns a reference to the underlying value (const rvalue overload).
		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&&() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		/// \brief	Returns a pointer to the underlying value.
		///
		/// \availability This operator is only available when #value_type is a class/struct.
		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		value_type* operator->() noexcept
		{
			return &val_;
		}

		/// \brief	Returns a pointer to the underlying value (const overload).
		///
		/// \availability This operator is only available when #value_type is a class/struct.
		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		const value_type* operator->() const noexcept
		{
			return &val_;
		}

		/// @}

		/// \name Metadata
		/// @{

		/// \brief	Returns the metadata flags associated with this value.
		TOML_PURE_INLINE_GETTER
		value_flags flags() const noexcept
		{
			return flags_;
		}

		/// \brief	Sets the metadata flags associated with this value.
		/// \returns A reference to the value object.
		value& flags(value_flags new_flags) noexcept
		{
			flags_ = new_flags;
			return *this;
		}

		/// @}

		/// \brief	Value-assignment operator.
		value& operator=(value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, std::string>)
				val_.assign(rhs);
			else
				val_ = rhs;
			return *this;
		}

		/// \brief	Value-assignment operator (string rvalue overload).
		TOML_CONSTRAINED_TEMPLATE((std::is_same_v<T, std::string>), typename T = value_type)
		value& operator=(std::string&& rhs) noexcept
		{
			val_ = std::move(rhs);
			return *this;
		}

		/// \name Equality and Comparison
		/// @{

		/// \brief	Value equality operator.
		///
		/// \detail For floating-point values, a NaN equals any other NaN, so a table holding one still equals
		/// 		itself. Otherwise floats compare as `double` does, so `-0.0` equals `0.0`.
		///
		/// 		An integer value compares with a floating-point operand as numbers, in this operator and in
		/// 		the relational ones, so `toml::value<int64_t>{ 1 } == 1.5` is false.
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, double>)
			{
				const auto lhs_nan = impl::fpclassify(lhs.val_) == impl::fp_class::nan;
				const auto rhs_nan = impl::fpclassify(rhs) == impl::fp_class::nan;
				if (lhs_nan != rhs_nan)
					return false;
				if (lhs_nan)
					return true;
			}
			return lhs.val_ == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, );

		/// \brief	Value less-than operator.
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ < rhs;
		}

		/// \brief	Value less-than operator.
		TOML_PURE_GETTER
		friend bool operator<(value_arg lhs, const value& rhs) noexcept
		{
			return lhs < rhs.val_;
		}

		/// \brief	Value less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ <= rhs;
		}

		/// \brief	Value less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend bool operator<=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs <= rhs.val_;
		}

		/// \brief	Value greater-than operator.
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ > rhs;
		}

		/// \brief	Value greater-than operator.
		TOML_PURE_GETTER
		friend bool operator>(value_arg lhs, const value& rhs) noexcept
		{
			return lhs > rhs.val_;
		}

		/// \brief	Value greater-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ >= rhs;
		}

		/// \brief	Value greater-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend bool operator>=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs >= rhs.val_;
		}

		/// \cond

		// an integer against a floating-point operand, which value_arg would truncate

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, T rhs) noexcept
		{
			return impl::compare_integer_with_float(lhs.val_, rhs) == impl::numeric_order::equal;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&,
									   T,
									   TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T>
																  && std::is_same_v<V, int64_t>),
																 typename T,
																 typename V = value_type));

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, T rhs) noexcept
		{
			return impl::compare_integer_with_float(lhs.val_, rhs) == impl::numeric_order::less;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<(T lhs, const value& rhs) noexcept
		{
			return impl::compare_integer_with_float(rhs.val_, lhs) == impl::numeric_order::greater;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, T rhs) noexcept
		{
			const auto order = impl::compare_integer_with_float(lhs.val_, rhs);
			return order == impl::numeric_order::less || order == impl::numeric_order::equal;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<=(T lhs, const value& rhs) noexcept
		{
			const auto order = impl::compare_integer_with_float(rhs.val_, lhs);
			return order == impl::numeric_order::greater || order == impl::numeric_order::equal;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, T rhs) noexcept
		{
			return rhs < lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>(T lhs, const value& rhs) noexcept
		{
			return rhs < lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, T rhs) noexcept
		{
			return rhs <= lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>=(T lhs, const value& rhs) noexcept
		{
			return rhs <= lhs;
		}

		/// \endcond

		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS value.
		/// \param 	rhs	The RHS value.
		///
		/// \returns	True if the values were of the same type and contained the same value.
		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs == rhs.val_; // calls asymmetrical value-equality operator defined above
			else
				return false;
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS value.
		/// \param 	rhs	The RHS value.
		///
		/// \returns	True if the values were not of the same type, or did not contain the same value.
		template <typename T>
		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const value& lhs, const value<T>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// \brief	Less-than operator.
		///
		/// \param 	lhs	The LHS toml::value.
		/// \param 	rhs	The RHS toml::value.
		///
		/// \returns	\conditional_return{Same value types}
		///				`lhs.get() < rhs.get()`
		/// 			\conditional_return{Different value types}
		///				`lhs.type() < rhs.type()`
		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ < rhs.val_;
			else
				return impl::node_type_of<value_type> < impl::node_type_of<T>;
		}

		/// \brief	Less-than-or-equal-to operator.
		///
		/// \param 	lhs	The LHS toml::value.
		/// \param 	rhs	The RHS toml::value.
		///
		/// \returns	\conditional_return{Same value types}
		///				`lhs.get() <= rhs.get()`
		/// 			\conditional_return{Different value types}
		///				`lhs.type() <= rhs.type()`
		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ <= rhs.val_;
			else
				return impl::node_type_of<value_type> <= impl::node_type_of<T>;
		}

		/// \brief	Greater-than operator.
		///
		/// \param 	lhs	The LHS toml::value.
		/// \param 	rhs	The RHS toml::value.
		///
		/// \returns	\conditional_return{Same value types}
		///				`lhs.get() > rhs.get()`
		/// 			\conditional_return{Different value types}
		///				`lhs.type() > rhs.type()`
		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ > rhs.val_;
			else
				return impl::node_type_of<value_type> > impl::node_type_of<T>;
		}

		/// \brief	Greater-than-or-equal-to operator.
		///
		/// \param 	lhs	The LHS toml::value.
		/// \param 	rhs	The RHS toml::value.
		///
		/// \returns	\conditional_return{Same value types}
		///				`lhs.get() >= rhs.get()`
		/// 			\conditional_return{Different value types}
		///				`lhs.type() >= rhs.type()`
		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ >= rhs.val_;
			else
				return impl::node_type_of<value_type> >= impl::node_type_of<T>;
		}

		/// @}

#if TOML_ENABLE_FORMATTERS

		/// \brief	Prints the value out to a stream as formatted TOML.
		///
		/// \availability This operator is only available when #TOML_ENABLE_FORMATTERS is enabled.
		friend std::ostream& operator<<(std::ostream& lhs, const value& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};

	/// \cond

	template <typename T>
	value(T) -> value<impl::native_type_of<impl::remove_cvref<T>>>;
	template <typename T>
	value(T, value_flags) -> value<impl::native_type_of<impl::remove_cvref<T>>>;

	template <typename T>
	TOML_NODISCARD
	inline decltype(auto) node::get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(node_type_of<T> != node_type::none);
		static_assert(node_type_of<T> != node_type::table);
		static_assert(node_type_of<T> != node_type::array);
		static_assert(is_native<T> || can_represent_native<T>);
		static_assert(!is_cvref<T>);
		TOML_ASSERT(this->type() == node_type_of<T>);

		if constexpr (node_type_of<T> == node_type::string)
		{
			const auto& str = *ref_cast<std::string>(*this);
			if constexpr (std::is_same_v<T, std::string>)
				return str;
			else if constexpr (std::is_same_v<T, std::string_view>)
				return T{ str };
			else if constexpr (std::is_same_v<T, const char*>)
				return str.c_str();

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return widen(str);
#else
				static_assert(always_false<T>, "Evaluated unreachable branch");
#endif
			}

			// char -> user-defined string-like type (see toml::is_string_like)
			else if constexpr (string_like_is_target<T>)
			{
				// construction preference order mirrors string_like_target_is_nothrow
				// (parenthesized init, matching the std::is_constructible checks)
				if constexpr (std::is_constructible_v<T, std::string_view>)
					return T(std::string_view{ str });
				else if constexpr (std::is_constructible_v<T, const char*, size_t>)
					return T(str.data(), str.length());
				else
					return T(str);
			}

#if TOML_HAS_CHAR8

			// char -> char8_t (potentially unsafe - the feature is 'experimental'!)
			else if constexpr (is_one_of<T, std::u8string, std::u8string_view>)
				return T(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char8_t*>)
				return reinterpret_cast<const char8_t*>(str.c_str());

#endif

			else
				static_assert(always_false<T>, "Evaluated unreachable branch");
		}
#if TOML_ENABLE_CHRONO
		else if constexpr (std::is_same_v<T, std::chrono::year_month_day>)
			return ref_cast<date>(*this)->to_chrono();
#endif
		else
			return static_cast<T>(*ref_cast<native_type_of<T>>(*this));
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_exact() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

		static_assert((is_native<T> || can_represent_native<T>) && !is_cvref<T>,
					  TOML_SA_VALUE_EXACT_FUNC_MESSAGE("return type of node::value_exact()"));

		// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
		if constexpr ((is_native<T> || can_represent_native<T>) && !is_cvref<T>)
		{
			if (type() != node_type_of<T>)
				return {};

#if TOML_ENABLE_CHRONO
			if constexpr (is_chrono_local_time<T>::value)
				return to_chrono_local_time<T>(*ref_cast<date_time>(*this));
			else if constexpr (is_chrono_hh_mm_ss<T>::value)
				return ref_cast<time>(*this)->template to_chrono<typename is_chrono_hh_mm_ss<T>::duration_type>();
			else
				return { this->get_value_exact<T>() };
#else
			return { this->get_value_exact<T>() };
#endif
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");
		static_assert((is_native<T> || can_represent_native<T> || can_partially_represent_native<T>) && !is_cvref<T>,
					  TOML_SA_VALUE_FUNC_MESSAGE("return type of node::value()"));

		// when asking for strings, dates, times and date_times there's no 'fuzzy' conversion
		// semantics to be mindful of so the exact retrieval is enough.
		if constexpr (is_natively_one_of<T, std::string, time, date, date_time>)
		{
			if (type() != node_type_of<T>)
				return {};

#if TOML_ENABLE_CHRONO
			if constexpr (is_chrono_local_time<T>::value)
				return to_chrono_local_time<T>(*ref_cast<date_time>(*this));
			else if constexpr (is_chrono_hh_mm_ss<T>::value)
				return ref_cast<time>(*this)->template to_chrono<typename is_chrono_hh_mm_ss<T>::duration_type>();
			else
				return { this->get_value_exact<T>() };
#else
			return { this->get_value_exact<T>() };
#endif
		}

		// everything else requires a bit of logicking.
		else
		{
			switch (type())
			{
				// int -> *
				case node_type::integer: return node_value_from_integer<T>(*ref_cast<int64_t>(*this));

				// float -> *
				case node_type::floating_point: return node_value_from_float<T>(*ref_cast<double>(*this));

				// bool -> *
				case node_type::boolean: return node_value_from_boolean<T>(*ref_cast<bool>(*this));

				// non-values, or 'exact' types covered above
				default: return {};
			}
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline auto node::value_or(T && default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_or() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

		if constexpr (is_wide_string<T>)
		{
#if TOML_ENABLE_WINDOWS_COMPAT

			if (type() == node_type::string)
				return widen(*ref_cast<std::string>(*this));
			return std::wstring{ static_cast<T&&>(default_value) };

#else

			static_assert(always_false<T>, "Evaluated unreachable branch");

#endif
		}
		else
		{
			using value_type =
				std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
								   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
								   std::decay_t<T>>;
			using traits = value_traits<value_type>;

			// clang-format off

			static_assert(
				traits::is_native || traits::can_represent_native || traits::can_partially_represent_native,
				"The default value type of node::value_or() must be one of:"
				TOML_SA_VALUE_MESSAGE_LOSSLESS
				TOML_SA_VALUE_MESSAGE_LOSSY

				TOML_SA_LIST_NXT "A compatible view type"
				TOML_SA_LIST_BEG "std::string_view"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "std::u8string_view"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring_view"
				#endif
				TOML_SA_LIST_SEP "const char*"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "const char8_t*"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "const wchar_t*"
				#endif
				TOML_SA_LIST_END

				TOML_SA_VALUE_MESSAGE_STRING_LIKE
			);

			// clang-format on

			// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
			if constexpr (traits::is_native || traits::can_represent_native || traits::can_partially_represent_native)
			{
				if constexpr (traits::is_native)
				{
					if (type() == node_type_of<value_type>)
						return *ref_cast<typename traits::native_type>(*this);
				}
				if (auto val = this->value<value_type>())
					return *std::move(val);
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}
	}

	/// \endcond
}
TOML_NAMESPACE_END;

#include "header_end.hpp"

#pragma once
#include "toml_node.h"
#include "toml_print_to_stream.h"

namespace toml
{
	/// \brief	A TOML value.
	/// 		
	/// \extends ::toml::node
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
	class value final : public node
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

			/// \brief	Constructs a toml value.
			///
			/// \tparam	U	Constructor argument types.
			/// \param 	args	Arguments to forward to the internal value's constructor.
			template <typename... U>
			TOML_NODISCARD_CTOR
			explicit value(U&&... args) TOML_MAY_THROW_UNLESS(std::is_nothrow_constructible_v<T, U &&...>)
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

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value& rhs) TOML_MAY_THROW
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


			using value_arg_t = std::conditional_t<
				std::is_same_v<T, string>,
				string_view,
				std::conditional_t<impl::is_one_of<T, double, int64_t, bool>, T, const T&>
			>;

			/// \brief	Value equality operator.
			[[nodiscard]] friend bool operator == (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ == rhs; }
			/// \brief	Value equality operator.
			[[nodiscard]] friend bool operator == (value_arg_t lhs, const value& rhs) noexcept { return lhs == rhs.val_; }
			/// \brief	Value inequality operator.
			[[nodiscard]] friend bool operator != (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ != rhs; }
			/// \brief	Value inequality operator.
			[[nodiscard]] friend bool operator != (value_arg_t lhs, const value& rhs) noexcept { return lhs != rhs.val_; }

			/// \brief	Value less-than operator.
			[[nodiscard]] friend bool operator <  (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ < rhs; }
			/// \brief	Value less-than operator.
			[[nodiscard]] friend bool operator <  (value_arg_t lhs, const value& rhs) noexcept { return lhs < rhs.val_; }
			/// \brief	Value less-than-or-equal-to operator.
			[[nodiscard]] friend bool operator <= (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ <= rhs; }
			/// \brief	Value less-than-or-equal-to operator.
			[[nodiscard]] friend bool operator <= (value_arg_t lhs, const value& rhs) noexcept { return lhs <= rhs.val_; }

			/// \brief	Value greater-than operator.
			[[nodiscard]] friend bool operator >  (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ > rhs; }
			/// \brief	Value greater-than operator.
			[[nodiscard]] friend bool operator >  (value_arg_t lhs, const value& rhs) noexcept { return lhs > rhs.val_; }
			/// \brief	Value greater-than-or-equal-to operator.
			[[nodiscard]] friend bool operator >= (const value& lhs, value_arg_t rhs) noexcept { return lhs.val_ >= rhs; }
			/// \brief	Value greater-than-or-equal-to operator.
			[[nodiscard]] friend bool operator >= (value_arg_t lhs, const value& rhs) noexcept { return lhs >= rhs.val_; }

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
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ != rhs.val_;
				else
					return true;
			}

			/// \brief	Less-than operator.
			///
			/// \param 	lhs	The LHS toml::value.
			/// \param 	rhs	The RHS toml::value.
			///
			/// \returns	Values of the same data type: `lhs.get() < rhs.get()` <br>
			/// 			Values of different types: `lhs.type() < rhs.type()`
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
			/// \returns	Values of the same data type: `lhs.get() <= rhs.get()` <br>
			/// 			Values of different types: `lhs.type() <= rhs.type()`
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
			/// \returns	Values of the same data type: `lhs.get() > rhs.get()` <br>
			/// 			Values of different types: `lhs.type() > rhs.type()`
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
			/// \returns	Values of the same data type: `lhs.get() >= rhs.get()` <br>
			/// 			Values of different types: `lhs.type() >= rhs.type()`
			template <typename U>
			[[nodiscard]] friend bool operator >= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ >= rhs.val_;
				else
					return impl::node_type_of<T> >= impl::node_type_of<U>;
			}
	};

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
	#ifdef TOML_SMALL_FLOAT_TYPE
	value(TOML_SMALL_FLOAT_TYPE) -> value<double>;
	#endif
	#ifdef TOML_SMALL_INT_TYPE
	value(TOML_SMALL_INT_TYPE) -> value<int64_t>;
	#endif
}

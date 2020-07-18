//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_common.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_MISC_WARNINGS

#define TOML_NATIVE_VALUE_TYPE_LIST							\
	"\n| - std::string"										\
	"\n| - int64_t"											\
	"\n| - double"											\
	"\n| - bool"											\
	"\n| - toml::date"										\
	"\n| - toml::time"										\
	"\n| - toml::date_time"


#define TOML_NODE_TYPE_LIST									\
	"\n| - toml::table"										\
	"\n| - toml::array"										\
	"\n| - toml::value<std::string>"						\
	"\n| - toml::value<int64_t>"							\
	"\n| - toml::value<double>"								\
	"\n| - toml::value<bool>"								\
	"\n| - toml::value<toml::date>"							\
	"\n| - toml::value<toml::time>"							\
	"\n| - toml::value<toml::date_time>"

#define TOML_UNWRAPPED_NODE_TYPE_LIST						\
	"\n|"													\
	"\n| A native TOML value type"							\
	TOML_NATIVE_VALUE_TYPE_LIST								\
	"\n|"													\
	"\n| A TOML node type"									\
	TOML_NODE_TYPE_LIST

namespace toml
{
	TOML_ABI_NAMESPACE_VERSION

	/// \brief	A TOML node.
	///
	/// \detail A parsed TOML document forms a tree made up of tables, arrays and values.
	/// 		This type is the base of each of those, providing a lot of the polymorphic plumbing.
	class TOML_INTERFACE TOML_API node
	{
		private:
			friend class TOML_PARSER_TYPENAME;
			source_region source_{};

		protected:
			
			node(node&& other) noexcept;
			node& operator= (node&& rhs) noexcept;

			template <typename T>
			[[nodiscard]]
			TOML_ALWAYS_INLINE
			impl::wrap_node<T>& ref_cast() & noexcept
			{
				return *reinterpret_cast<impl::wrap_node<T>*>(this);
			}

			template <typename T>
			[[nodiscard]]
			TOML_ALWAYS_INLINE
			impl::wrap_node<T>&& ref_cast() && noexcept
			{
				return std::move(*reinterpret_cast<impl::wrap_node<T>*>(this));
			}

			template <typename T>
			[[nodiscard]]
			TOML_ALWAYS_INLINE
			const impl::wrap_node<T>& ref_cast() const & noexcept
			{
				return *reinterpret_cast<const impl::wrap_node<T>*>(this);
			}

			template <typename N, typename T>
			using ref_cast_type = decltype(std::declval<N>().template ref_cast<T>());

			node() noexcept = default;
			node(const node&) = delete;
			node& operator= (const node&) = delete;

		public:

			virtual ~node() noexcept = default;

			/// \brief	Returns the node's type identifier.
			[[nodiscard]] virtual node_type type() const noexcept = 0;

			/// \brief	Returns true if this node is a table.
			[[nodiscard]] virtual bool is_table() const noexcept = 0;
			/// \brief	Returns true if this node is an array.
			[[nodiscard]] virtual bool is_array() const noexcept = 0;
			/// \brief	Returns true if this node is a value.
			[[nodiscard]] virtual bool is_value() const noexcept = 0;

			/// \brief	Returns true if this node is a string value.
			[[nodiscard]] virtual bool is_string() const noexcept;
			/// \brief	Returns true if this node is an integer value.
			[[nodiscard]] virtual bool is_integer() const noexcept;
			/// \brief	Returns true if this node is an floating-point value.
			[[nodiscard]] virtual bool is_floating_point() const noexcept;
			/// \brief	Returns true if this node is an integer or floating-point value.
			[[nodiscard]] virtual bool is_number() const noexcept;
			/// \brief	Returns true if this node is a boolean value.
			[[nodiscard]] virtual bool is_boolean() const noexcept;
			/// \brief	Returns true if this node is a local date value.
			[[nodiscard]] virtual bool is_date() const noexcept;
			/// \brief	Returns true if this node is a local time value.
			[[nodiscard]] virtual bool is_time() const noexcept;
			/// \brief	Returns true if this node is a date-time value.
			[[nodiscard]] virtual bool is_date_time() const noexcept;
			/// \brief	Returns true if this node is an array containing only tables.
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept;

			/// \brief	Checks if a node is a specific type.
			///
			/// \tparam	T	A TOML node or value type.
			///
			/// \returns	Returns true if this node is an instance of the specified type.
			template <typename T>
			[[nodiscard]]
			bool is() const noexcept
			{
				using type = impl::unwrap_node<T>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"The template type argument of node::is() must be one of the following:"
					TOML_UNWRAPPED_NODE_TYPE_LIST
				);

					 if constexpr (std::is_same_v<type, table>) return is_table();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, std::string>) return is_string();
				else if constexpr (std::is_same_v<type, int64_t>) return is_integer();
				else if constexpr (std::is_same_v<type, double>) return is_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return is_boolean();
				else if constexpr (std::is_same_v<type, date>) return is_date();
				else if constexpr (std::is_same_v<type, time>) return is_time();
				else if constexpr (std::is_same_v<type, date_time>) return is_date_time();
			}

			/// \brief	Returns a pointer to the node as a toml::table, if it is one.
			[[nodiscard]] virtual table* as_table() noexcept;
			/// \brief	Returns a pointer to the node as a toml::array, if it is one.
			[[nodiscard]] virtual array* as_array() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<string>, if it is one.
			[[nodiscard]] virtual toml::value<std::string>* as_string() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<int64_t>, if it is one.
			[[nodiscard]] virtual toml::value<int64_t>* as_integer() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<double>, if it is one.
			[[nodiscard]] virtual toml::value<double>* as_floating_point() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<bool>, if it is one.
			[[nodiscard]] virtual toml::value<bool>* as_boolean() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<date>, if it is one.
			[[nodiscard]] virtual toml::value<date>* as_date() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<time>, if it is one.
			[[nodiscard]] virtual toml::value<time>* as_time() noexcept;
			/// \brief	Returns a pointer to the node as a toml::value<date_time>, if it is one.
			[[nodiscard]] virtual toml::value<date_time>* as_date_time() noexcept;

			[[nodiscard]] virtual const table* as_table() const noexcept;
			[[nodiscard]] virtual const array* as_array() const noexcept;
			[[nodiscard]] virtual const toml::value<std::string>* as_string() const noexcept;
			[[nodiscard]] virtual const toml::value<int64_t>* as_integer() const noexcept;
			[[nodiscard]] virtual const toml::value<double>* as_floating_point() const noexcept;
			[[nodiscard]] virtual const toml::value<bool>* as_boolean() const noexcept;
			[[nodiscard]] virtual const toml::value<date>* as_date() const noexcept;
			[[nodiscard]] virtual const toml::value<time>* as_time() const noexcept;
			[[nodiscard]] virtual const toml::value<date_time>* as_date_time() const noexcept;


		private:

			template <typename T>
			[[nodiscard]]
			decltype(auto) get_value_exact() const noexcept;

		public:

			/// \brief	Gets the value contained by this node.
			/// 
			/// \detail This function has 'exact' retrieval semantics; the only return value types allowed are the
			/// 		TOML native value types, or types that can losslessly represent a native value type (e.g.
			/// 		std::wstring on Windows).
			/// 
			/// \tparam	T	One of the native TOML value types, or a type capable of losslessly representing one.
			/// 
			/// \returns	The underlying value if the node was a value of the
			/// 			matching type (or losslessly convertible to it), or an empty optional.
			/// 
			/// \see node::value()
			template <typename T>
			[[nodiscard]]
			optional<T> value_exact() const noexcept;

			/// \brief	Gets the value contained by this node.
			/// 
			/// \detail This function has 'permissive' retrieval semantics; some value types are allowed
			/// 		to convert to others (e.g. retrieving a boolean as an integer), and the specified return value
			/// 		type can be any type where a reasonable conversion from a native TOML value exists
			/// 		(e.g. std::wstring on Windows). If the source value cannot be represented by
			/// 		the destination type, an empty optional is returned.
			/// 
			/// \cpp
			/// auto tbl = toml::parse(R"(
			/// 	int	= -10
			/// 	flt	= 25.0
			/// 	pi	= 3.14159
			/// 	bool = false
			/// 	huge = 9223372036854775807
			/// 	str	= "foo"
			/// )"sv);
			/// 
			/// const auto print_value_with_typename =
			///		[&](std::string_view key, std::string_view type_name, auto* dummy)
			///		{
			///			std::cout << "'" << key << "' as " << type_name << ": ";
			///			using type = std::remove_pointer_t<decltype(dummy)>;
			///			if (std::optional<type> val = tbl.get(key)->value<type>())
			///				std::cout << *val << "\n";
			///			else
			///				std::cout << "No conversion path or out-of-range\n";
			///		};
			/// 
			/// #define print_value(key, T)	print_value_with_typename(key, #T, (T*)nullptr)
			/// 
			/// print_value("int", bool);
			/// print_value("int", int);
			/// print_value("int", unsigned int);
			/// print_value("int", long long);
			/// print_value("int", float);
			/// print_value("int", double);
			/// std::cout << "\n";
			/// 
			/// print_value("flt", bool);
			/// print_value("flt", int);
			/// print_value("flt", unsigned int);
			/// print_value("flt", long long);
			/// print_value("flt", float);
			/// print_value("flt", double);
			/// std::cout << "\n";
			/// 
			/// print_value("pi", bool);
			/// print_value("pi", int);
			/// print_value("pi", unsigned int);
			/// print_value("pi", long long);
			/// print_value("pi", float);
			/// print_value("pi", double);
			/// std::cout << "\n";
			/// 
			/// print_value("bool", bool);
			/// print_value("bool", int);
			/// print_value("bool", unsigned int);
			/// print_value("bool", long long);
			/// print_value("bool", float);
			/// print_value("bool", double);
			/// std::cout << "\n";
			/// 
			/// print_value("huge", bool);
			/// print_value("huge", int);
			/// print_value("huge", unsigned int);
			/// print_value("huge", long long);
			/// print_value("huge", float);
			/// print_value("huge", double);
			/// std::cout << "\n";
			/// 
			/// print_value("str", std::string);
			/// print_value("str", std::string_view);
			/// print_value("str", const char*);
			/// std::cout << "\n";
			/// \ecpp
			/// 
			/// \out
			/// 'int' as bool: true
			/// 'int' as int: -10
			/// 'int' as unsigned int: No conversion path or out-of-range
			/// 'int' as long long: -10
			/// 'int' as float: -10
			/// 'int' as double: -10
			/// 
			/// 'flt' as bool: No conversion path or out-of-range
			/// 'flt' as int: 25
			/// 'flt' as unsigned int: 25
			/// 'flt' as long long: 25
			/// 'flt' as float: 25
			/// 'flt' as double: 25
			/// 
			/// 'pi' as bool: No conversion path or out-of-range
			/// 'pi' as int: No conversion path or out-of-range
			/// 'pi' as unsigned int: No conversion path or out-of-range
			/// 'pi' as long long: No conversion path or out-of-range
			/// 'pi' as float: 3.14159
			/// 'pi' as double: 3.14159
			/// 
			/// 'bool' as bool: false
			/// 'bool' as int: 0
			/// 'bool' as unsigned int: 0
			/// 'bool' as long long: 0
			/// 'bool' as float: No conversion path or out-of-range
			/// 'bool' as double: No conversion path or out-of-range
			/// 
			/// 'huge' as bool: true
			/// 'huge' as int: No conversion path or out-of-range
			/// 'huge' as unsigned int: No conversion path or out-of-range
			/// 'huge' as long long: 9223372036854775807
			/// 'huge' as float: No conversion path or out-of-range
			/// 'huge' as double: No conversion path or out-of-range
			/// 
			/// 'str' as std::string: foo
			/// 'str' as std::string_view: foo
			/// 'str' as const char*: foo
			/// \eout
			///
			/// \tparam	T	One of the native TOML value types, or a type capable of converting to one.
			///
			/// \returns	The underlying value if the node was a value of the matching type (or convertible to it)
			/// 			and within the range of the output type, or an empty optional.
			/// 
			/// \attention If you want strict value retrieval semantics that do not allow for any type conversions,
			/// 		   use node::value_exact() instead.
			/// 
			/// \see node::value_exact()
			template <typename T>
			[[nodiscard]]
			optional<T> value() const noexcept;

			/// \brief	Gets the raw value contained by this node, or a default.
			///
			/// \tparam	T				Default value type. Must be one of the native TOML value types,
			/// 						or convertible to it.
			/// \param 	default_value	The default value to return if the node wasn't a value, wasn't the
			/// 						correct type, or no conversion was possible.
			///
			/// \returns	The underlying value if the node was a value of the matching type (or convertible to it)
			/// 			and within the range of the output type, or the provided default.
			/// 
			/// \attention This function has the same permissive retrieval semantics as node::value(). If you want strict
			/// 		   value retrieval semantics that do not allow for any type conversions, use node::value_exact()
			/// 		   instead.
			/// 
			/// \see
			/// 	- node::value()
			///		- node::value_exact()
			template <typename T>
			[[nodiscard]]
			auto value_or(T&& default_value) const noexcept;

			/// \brief	Gets a pointer to the node as a more specific node type.
			///
			/// \details \cpp
			/// 
			/// toml::value<int64_t>* int_value = node->as<int64_t>();
			/// toml::table* tbl = node->as<toml::table>();
			/// if (int_value)
			///		std::cout << "Node is a value<int64_t>" << std::endl;
			/// else if (tbl)
			///		std::cout << "Node is a table" << std::endl;
			///	
			///	// fully-qualified value node types also work (useful for template code):
			///	toml::value<int64_t>* int_value2 = node->as<toml::value<int64_t>>();
			/// if (int_value2)
			///		std::cout << "Node is a value<int64_t>" << std::endl;
			///		
			/// \ecpp
			/// 
			/// \tparam	T	The node type or TOML value type to cast to.
			///
			/// \returns	A pointer to the node as the given type, or nullptr if it was a different type.
			template <typename T>
			[[nodiscard]]
			impl::wrap_node<T>* as() noexcept
			{
				using type = impl::unwrap_node<T>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"The template type argument of node::as() must be one of the following:"
					TOML_UNWRAPPED_NODE_TYPE_LIST
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, std::string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			/// \brief	Gets a pointer to the node as a more specific node type (const overload).
			template <typename T>
			[[nodiscard]]
			const impl::wrap_node<T>* as() const noexcept
			{
				using type = impl::unwrap_node<T>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"The template type argument of node::as() must be one of the following:"
					TOML_UNWRAPPED_NODE_TYPE_LIST
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, std::string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			/// \brief	Returns the source region responsible for generating this node during parsing.
			[[nodiscard]] const source_region& source() const noexcept;

		private:

			template <typename Func, typename N, typename T>
			static constexpr bool can_visit = std::is_invocable_v<Func, ref_cast_type<N, T>>;

			template <typename Func, typename N>
			static constexpr bool can_visit_any =
				can_visit<Func, N, table>
				|| can_visit<Func, N, array>
				|| can_visit<Func, N, std::string>
				|| can_visit<Func, N, int64_t>
				|| can_visit<Func, N, double>
				|| can_visit<Func, N, bool>
				|| can_visit<Func, N, date>
				|| can_visit<Func, N, time>
				|| can_visit<Func, N, date_time>;

			template <typename Func, typename N>
			static constexpr bool can_visit_all =
				can_visit<Func, N, table>
				&& can_visit<Func, N, array>
				&& can_visit<Func, N, std::string>
				&& can_visit<Func, N, int64_t>
				&& can_visit<Func, N, double>
				&& can_visit<Func, N, bool>
				&& can_visit<Func, N, date>
				&& can_visit<Func, N, time>
				&& can_visit<Func, N, date_time>;

			template <typename Func, typename N, typename T>
			static constexpr bool visit_is_nothrow_one =
				!can_visit<Func, N, T>
				|| std::is_nothrow_invocable_v<Func, ref_cast_type<N, T>>;

			template <typename Func, typename N>
			static constexpr bool visit_is_nothrow =
				visit_is_nothrow_one<Func, N, table>
				&& visit_is_nothrow_one<Func, N, array>
				&& visit_is_nothrow_one<Func, N, std::string>
				&& visit_is_nothrow_one<Func, N, int64_t>
				&& visit_is_nothrow_one<Func, N, double>
				&& visit_is_nothrow_one<Func, N, bool>
				&& visit_is_nothrow_one<Func, N, date>
				&& visit_is_nothrow_one<Func, N, time>
				&& visit_is_nothrow_one<Func, N, date_time>;

			template <typename Func, typename N, typename T, bool = can_visit<Func, N, T>>
			struct visit_return_type final
			{
				using type = decltype(std::declval<Func>()(std::declval<ref_cast_type<N, T>>()));
			};
			template <typename Func, typename N, typename T>
			struct visit_return_type<Func, N, T, false> final
			{
				using type = void;
			};

			template <typename A, typename B>
			using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

			//# these functions are static helpers to preserve const and ref categories
			//# (otherwise I'd have to implement them thrice)
			//# ((propagation in C++: a modern horror story))

			template <typename N, typename Func>
			static decltype(auto) do_visit(N&& n, Func&& visitor)
				noexcept(visit_is_nothrow<Func&&, N&&>)
			{
				static_assert(
					can_visit_any<Func&&, N&&>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (n.type())
				{
					case node_type::table:
						if constexpr (can_visit<Func&&, N&&, table>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<table>());
						break;

					case node_type::array:
						if constexpr (can_visit<Func&&, N&&, array>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<array>());
						break;

					case node_type::string:
						if constexpr (can_visit<Func&&, N&&, std::string>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<std::string>());
						break;

					case node_type::integer:
						if constexpr (can_visit<Func&&, N&&, int64_t>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<int64_t>());
						break;

					case node_type::floating_point:
						if constexpr (can_visit<Func&&, N&&, double>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<double>());
						break;

					case node_type::boolean:
						if constexpr (can_visit<Func&&, N&&, bool>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<bool>());
						break;

					case node_type::date:
						if constexpr (can_visit<Func&&, N&&, date>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<date>());
						break;

					case node_type::time:
						if constexpr (can_visit<Func&&, N&&, time>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<time>());
						break;

					case node_type::date_time:
						if constexpr (can_visit<Func&&, N&&, date_time>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<date_time>());
						break;

					case node_type::none: TOML_UNREACHABLE;
					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (can_visit_all<Func&&, N&&>)
					TOML_UNREACHABLE;
				else
				{
					using return_type = 
						nonvoid<typename visit_return_type<Func&&, N&&, table>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, array>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, std::string>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, int64_t>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, double>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, bool>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, date>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, time>::type,
								typename visit_return_type<Func&&, N&&, date_time>::type
					>>>>>>>>;

					if constexpr (!std::is_void_v<return_type>)
					{
						static_assert(
							std::is_default_constructible_v<return_type>,
							"Non-exhaustive visitors must return a default-constructible type, or void"
						);
						return return_type{};
					}
				}
			}


			template <typename T, typename N>
			[[nodiscard]]
			static decltype(auto) do_ref(N&& n) noexcept
			{
				using type = impl::unwrap_node<T>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"The template type argument of node::ref() must be one of the following:"
					TOML_UNWRAPPED_NODE_TYPE_LIST
				);
				TOML_ASSERT(
					n.template is<T>()
					&& "template type argument T provided to toml::node::ref() didn't match the node's actual type"
				);
				if constexpr (impl::is_native<type>)
					return std::forward<N>(n).template ref_cast<type>().get();
				else
					return std::forward<N>(n).template ref_cast<type>();
			}

		public:

			/// \brief	Invokes a visitor on the node based on the node's concrete type.
			/// 
			/// \details Visitation is useful when you expect
			/// 		 a node to be one of a set number of types and need
			/// 		 to handle these types differently. Using `visit()` allows
			/// 		 you to eliminate some of the casting/conversion boilerplate: \cpp
			///
			/// node.visit([](auto&& n)
			/// {
			///		if constexpr (toml::is_string<decltype(n)>)
			///			do_something_with_a_string(*n)); //n is a toml::value<std::string>
			///		else if constexpr (toml::is_integer<decltype(n)>)
			///			do_something_with_an_int(*n); //n is a toml::value<int64_t>
			///		else
			///			throw std::exception{ "Expected string or integer" };
			/// });
			///
			/// \ecpp
			///
			/// \tparam	Func	A callable type invocable with one or more of the
			/// 				toml++ node types.
			/// 				
			/// \param 	visitor	The visitor object.
			/// 
			/// \returns The return value of the visitor.
			/// 		 Can be void. Non-exhaustive visitors must return a default-constructible type.
			/// 
			/// \see https://en.wikipedia.org/wiki/Visitor_pattern
			template <typename Func>
			decltype(auto) visit(Func&& visitor) &
				noexcept(visit_is_nothrow<Func&&, node&>)
			{
				return do_visit(*this, std::forward<Func>(visitor));
			}

			/// \brief	Invokes a visitor on the node based on the node's concrete type (rvalue overload).
			template <typename Func>
			decltype(auto) visit(Func&& visitor) &&
				noexcept(visit_is_nothrow<Func&&, node&&>)
			{
				return do_visit(std::move(*this), std::forward<Func>(visitor));
			}

			/// \brief	Invokes a visitor on the node based on the node's concrete type (const lvalue overload).
			template <typename Func>
			decltype(auto) visit(Func&& visitor) const&
				noexcept(visit_is_nothrow<Func&&, const node&>)
			{
				return do_visit(*this, std::forward<Func>(visitor));
			}

			/// \brief	Gets a raw reference to a value node's underlying data.
			///
			/// \warning This function is dangerous if used carelessly and **WILL** break your code if the
			///			 chosen value type doesn't match the node's actual type. In debug builds an assertion
			///			 will fire when invalid accesses are attempted: \cpp
			///
			/// auto tbl = toml::parse(R"(
			///		min = 32
			///		max = 45
			/// )"sv);
			/// 
			/// int64_t& min_ref = tbl.get("min")->ref<int64_t>(); // matching type
			/// double& max_ref = tbl.get("max")->ref<double>();  // mismatched type, hits assert()
			///
			/// \ecpp
			/// 
			/// \tparam	T	One of the TOML value types.
			///
			/// \returns	A reference to the underlying data.
			template <typename T>
			[[nodiscard]]
			impl::unwrap_node<T>& ref() & noexcept
			{
				return do_ref<T>(*this);
			}

			/// \brief	Gets a raw reference to a value node's underlying data (rvalue overload).
			template <typename T>
			[[nodiscard]]
			impl::unwrap_node<T>&& ref() && noexcept
			{
				return do_ref<T>(std::move(*this));
			}

			/// \brief	Gets a raw reference to a value node's underlying data (const lvalue overload).
			template <typename T>
			[[nodiscard]]
			const impl::unwrap_node<T>& ref() const& noexcept
			{
				return do_ref<T>(*this);
			}
	};

	TOML_ABI_NAMESPACE_END // version
}

TOML_POP_WARNINGS // TOML_DISABLE_MISC_WARNINGS

//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_common.h"

namespace toml
{
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
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>& ref_cast() & noexcept
			{
				return *reinterpret_cast<impl::node_of<T>*>(this);
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>&& ref_cast() && noexcept
			{
				return std::move(*reinterpret_cast<impl::node_of<T>*>(this));
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const impl::node_of<T>& ref_cast() const & noexcept
			{
				return *reinterpret_cast<const impl::node_of<T>*>(this);
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
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is() const noexcept
			{
				using type = impl::unwrapped<impl::remove_cvref_t<T>>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return is_table();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, string>) return is_string();
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
			[[nodiscard]] virtual toml::value<string>* as_string() noexcept;
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
			[[nodiscard]] virtual const toml::value<string>* as_string() const noexcept;
			[[nodiscard]] virtual const toml::value<int64_t>* as_integer() const noexcept;
			[[nodiscard]] virtual const toml::value<double>* as_floating_point() const noexcept;
			[[nodiscard]] virtual const toml::value<bool>* as_boolean() const noexcept;
			[[nodiscard]] virtual const toml::value<date>* as_date() const noexcept;
			[[nodiscard]] virtual const toml::value<time>* as_time() const noexcept;
			[[nodiscard]] virtual const toml::value<date_time>* as_date_time() const noexcept;

			/// \brief	Gets the raw value contained by this node.
			/// 
			/// \detail The optional returned by this function will only contain a value if the node was an instance of
			/// 		toml::value with the same value type as the template argument. Additionally, some type are allowed to
			/// 		convert to each other, for instance asking for an integer when the value exists as a double,
			/// 		or requesting a string value as a string_view: \cpp
			/// auto tbl = toml::parse(R"(
			///		int_val = 10
			///		float_val = 25.6
			///		string_val = "kek"
			/// )"sv);
			/// 
			/// if (auto val = tbl.get("int_val"sv)->value<int64_t>())
			///		std::cout << "'int_val' as int64_t: "sv << *val << std::endl;
			/// if (auto val = tbl.get("int_val"sv)->value<double>())
			///		std::cout << "'int_val' as double: "sv << *val << std::endl;
			/// if (auto val = tbl.get("float_val"sv)->value<int64_t>())
			///		std::cout << "'float_val' as int64_t: "sv << *val << std::endl;
			/// if (auto val = tbl.get("float_val"sv)->value<double>())
			///		std::cout << "'float_val' as double: "sv << *val << std::endl;
			/// if (auto val = tbl.get("string_val"sv)->value<std::string>())
			///		std::cout << "'string_val' as std::string: "sv << *val << std::endl;
			/// if (auto val = tbl.get("string_val"sv)->value<std::string_view>())
			///		std::cout << "'string_val' as std::string_view: "sv << *val << std::endl;
			/// if (auto val = tbl.get("string_val"sv)->value<int64_t>())
			///		std::cout << "this line won't be printed."sv << std::endl;
			/// \ecpp
			/// 
			/// \out
			/// 'int_val' as int64_t: 10
			/// 'int_val' as double: 10
			/// 'float_val' as int64_t: 25
			/// 'float_val' as double: 25.6
			/// 'string_val' as std::string: kek
			/// 'string_val' as std::string_view: kek
			/// \eout
			///
			/// \tparam	T	One of the TOML value types. Can also be a string_view.
			///
			/// \returns	The underlying value if the node was a value of the matching type (or convertible to it), or an empty optional.
			template <typename T>
			[[nodiscard]] optional<T> value() const noexcept;

			/// \brief	Gets the raw value contained by this node, or a default.
			///
			/// \tparam	T	Default value type. Must be (or be promotable to) one of the TOML value types.
			/// \param 	default_value	The default value to return if the node wasn't a value, wasn't the
			/// 						correct type, or no conversion was possible.
			///
			/// \returns	The node's underlying value, or the default if the node wasn't a value, wasn't the
			/// 						correct type, or no conversion was possible.
			/// 
			/// \see node::value()
			template <typename T>
			[[nodiscard]] auto value_or(T&& default_value) const noexcept;

			/// \brief	Gets a pointer to the node as a more specific node type.
			///
			/// \details \cpp
			/// 
			/// auto int_value = node->as<int64_t>();
			/// auto tbl = node->as<toml::table>();
			/// if (int_value)
			///		std::cout << "Node is a value<int64_t>" << std::endl;
			/// else if (tbl)
			///		std::cout << "Node is a table" << std::endl;
			///	
			///	// fully-qualified value node types also work:
			///	auto int_value2 = node->as<toml::value<int64_t>>();
			/// if (int_value2)
			///		std::cout << "Node is a value<int64_t>" << std::endl;
			///		
			/// \ecpp
			/// 
			/// \tparam	T	The node type or TOML value type to cast to.
			///
			/// \returns	A pointer to the node as the given type, or nullptr if it was a different type.
			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>* as() noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			/// \brief	Gets a pointer to the node as a more specific node type (const overload).
			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const impl::node_of<T>* as() const noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
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

			template <typename FUNC, typename N, typename T>
			static constexpr bool can_visit = std::is_invocable_v<FUNC, ref_cast_type<N, T>>;

			template <typename FUNC, typename N>
			static constexpr bool can_visit_any =
				can_visit<FUNC, N, table>
				|| can_visit<FUNC, N, array>
				|| can_visit<FUNC, N, string>
				|| can_visit<FUNC, N, int64_t>
				|| can_visit<FUNC, N, double>
				|| can_visit<FUNC, N, bool>
				|| can_visit<FUNC, N, date>
				|| can_visit<FUNC, N, time>
				|| can_visit<FUNC, N, date_time>;

			template <typename FUNC, typename N>
			static constexpr bool can_visit_all =
				can_visit<FUNC, N, table>
				&& can_visit<FUNC, N, array>
				&& can_visit<FUNC, N, string>
				&& can_visit<FUNC, N, int64_t>
				&& can_visit<FUNC, N, double>
				&& can_visit<FUNC, N, bool>
				&& can_visit<FUNC, N, date>
				&& can_visit<FUNC, N, time>
				&& can_visit<FUNC, N, date_time>;

			template <typename FUNC, typename N, typename T>
			static constexpr bool visit_is_nothrow_one =
				!can_visit<FUNC, N, T>
				|| std::is_nothrow_invocable_v<FUNC, ref_cast_type<N, T>>;

			template <typename FUNC, typename N>
			static constexpr bool visit_is_nothrow =
				visit_is_nothrow_one<FUNC, N, table>
				&& visit_is_nothrow_one<FUNC, N, array>
				&& visit_is_nothrow_one<FUNC, N, string>
				&& visit_is_nothrow_one<FUNC, N, int64_t>
				&& visit_is_nothrow_one<FUNC, N, double>
				&& visit_is_nothrow_one<FUNC, N, bool>
				&& visit_is_nothrow_one<FUNC, N, date>
				&& visit_is_nothrow_one<FUNC, N, time>
				&& visit_is_nothrow_one<FUNC, N, date_time>;

			template <typename FUNC, typename N, typename T, bool = can_visit<FUNC, N, T>>
			struct visit_return_type final
			{
				using type = decltype(std::declval<FUNC>()(std::declval<ref_cast_type<N, T>>()));
			};
			template <typename FUNC, typename N, typename T>
			struct visit_return_type<FUNC, N, T, false> final
			{
				using type = void;
			};

			template <typename A, typename B>
			using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

			//# these functions are static helpers to preserve const and ref categories
			//# (otherwise I'd have to implement them thrice)
			//# ((propagation in C++: a modern horror story))

			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N&& n, FUNC&& visitor)
				noexcept(visit_is_nothrow<FUNC&&, N&&>)
			{
				static_assert(
					can_visit_any<FUNC&&, N&&>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (n.type())
				{
					case node_type::table:
						if constexpr (can_visit<FUNC&&, N&&, table>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<table>());
						break;

					case node_type::array:
						if constexpr (can_visit<FUNC&&, N&&, array>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<array>());
						break;

					case node_type::string:
						if constexpr (can_visit<FUNC&&, N&&, string>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<string>());
						break;

					case node_type::integer:
						if constexpr (can_visit<FUNC&&, N&&, int64_t>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<int64_t>());
						break;

					case node_type::floating_point:
						if constexpr (can_visit<FUNC&&, N&&, double>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<double>());
						break;

					case node_type::boolean:
						if constexpr (can_visit<FUNC&&, N&&, bool>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<bool>());
						break;

					case node_type::date:
						if constexpr (can_visit<FUNC&&, N&&, date>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<date>());
						break;

					case node_type::time:
						if constexpr (can_visit<FUNC&&, N&&, time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<time>());
						break;

					case node_type::date_time:
						if constexpr (can_visit<FUNC&&, N&&, date_time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<date_time>());
						break;

					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (can_visit_all<FUNC&&, N&&>)
					TOML_UNREACHABLE;
				else
				{
					using return_type = 
						nonvoid<typename visit_return_type<FUNC&&, N&&, table>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, array>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, string>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, int64_t>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, double>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, bool>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, date>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, time>::type,
								typename visit_return_type<FUNC&&, N&&, date_time>::type
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
			[[nodiscard]] static decltype(auto) do_ref(N&& n) noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);
				TOML_ASSERT(
					n.template is<T>()
					&& "template type argument T provided to toml::node::ref() didn't match the node's actual type"
				);
				if constexpr (impl::is_value<type>)
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
			///			do_something_with_a_string(*n)); //n is a toml::value<toml::string>
			///		else if constexpr (toml::is_integer<decltype(n)>)
			///			do_something_with_an_int(*n); //n is a toml::value<int64_t>
			///		else
			///			throw std::exception{ "Expected string or integer" };
			/// });
			///
			/// \ecpp
			///
			/// \tparam	FUNC	A callable type invocable with one or more of the
			/// 				toml++ node types.
			/// 				
			/// \param 	visitor	The visitor object.
			/// 
			/// \returns The return value of the visitor.
			/// 		 Can be void. Non-exhaustive visitors must return a default-constructible type.
			/// 
			/// \see https://en.wikipedia.org/wiki/Visitor_pattern
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) & noexcept(visit_is_nothrow<FUNC&&, node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) && noexcept(visit_is_nothrow<FUNC&&, node&&>)
			{
				return do_visit(std::move(*this), std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const& noexcept(visit_is_nothrow<FUNC&&, const node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			/// \brief	Gets a raw reference to a value node's underlying data.
			///
			/// \warning This function is dangerous if used carelessly and **WILL** break your code if the
			///			 chosen value type doesn't match the node's actual type. In debug builds an assertion
			///			 will fire when invalid accesses are attempted: \cpp
			///
			/// auto tbl = toml::parse(R"(
			/// 
			///	min = 32
			///	max = 45
			///	
			/// )"sv);
			/// 
			/// auto& min_ref = tbl.get("min")->ref<int64_t>(); // this is OK
			/// auto& max_ref = tbl.get("max")->ref<double>();  // hits assertion because the type is wrong
			///
			/// \ecpp
			/// 
			/// \tparam	T	One of the TOML value types.
			///
			/// \returns	A reference to the underlying data.
			template <typename T>
			[[nodiscard]] impl::unwrapped<T>& ref() & noexcept
			{
				return do_ref<T>(*this);
			}

			template <typename T>
			[[nodiscard]] impl::unwrapped<T>&& ref() && noexcept
			{
				return do_ref<T>(std::move(*this));
			}

			template <typename T>
			[[nodiscard]] const impl::unwrapped<T>& ref() const& noexcept
			{
				return do_ref<T>(*this);
			}
	};
}


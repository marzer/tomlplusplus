#pragma once
#include "toml_common.h"

namespace toml
{
	/// \brief	A TOML node.
	///
	/// \detail A parsed TOML document forms a tree made up of tables, arrays and values.
	/// 		This type is the base of each of those, providing a lot of the polymorphic plumbing.
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			source_region source_{};

		protected:
			
			node(node&& other) noexcept
				: source_{ std::move(other.source_) }
			{}

			node& operator= (node&& rhs) noexcept
			{
				source_ = std::move(rhs.source_);
				return *this;
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>& ref_cast() & noexcept { return *reinterpret_cast<node_of<T>*>(this); }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>&& ref_cast() && noexcept { return std::move(*reinterpret_cast<node_of<T>*>(this)); }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const node_of<T>& ref_cast() const & noexcept { return *reinterpret_cast<const node_of<T>*>(this); }

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
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			/// \brief	Returns true if this node is an integer value.
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			/// \brief	Returns true if this node is an floating-point value.
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			/// \brief	Returns true if this node is a boolean value.
			[[nodiscard]] virtual bool is_boolean() const noexcept { return false; }
			/// \brief	Returns true if this node is a local date value.
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			/// \brief	Returns true if this node is a local time value.
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			/// \brief	Returns true if this node is a date-time value.
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			/// \brief	Returns true if this node is an array containing only tables.
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }

			/// \brief	Checks if a node is a specific type.
			///
			/// \tparam	T	The 
			///
			/// \returns	Returns true if this node is an instance
			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is() const noexcept
			{
				using type = value_of<impl::remove_cvref_t<T>>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
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
			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::array, if it is one.
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<string>, if it is one.
			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<int64_t>, if it is one.
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<double>, if it is one.
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<bool>, if it is one.
			[[nodiscard]] virtual value<bool>* as_boolean() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<date>, if it is one.
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<time>, if it is one.
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			/// \brief	Returns a pointer to the node as a toml::value<date_time>, if it is one.
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }

			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_boolean() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }

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
			node_of<T>* as() noexcept
			{
				using type = value_of<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
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
			const node_of<T>* as() const noexcept
			{
				using type = value_of<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
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
			[[nodiscard]] const source_region& source() const noexcept
			{
				return source_;
			}

		private:

			template <typename FUNC, typename N, typename T, bool = std::is_invocable_v<FUNC, ref_cast_type<N, T>>>
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

			// this is done using a static helper to preserve const and ref categories
			// (otherwise I'd have to implement this function thrice)
			// ((propagation in C++: a modern horror story))
			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N&& node, FUNC&& visitor) TOML_MAY_THROW
			{
				static_assert(
					std::is_invocable_v<FUNC&&,    ref_cast_type<N&&, table>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, array>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, string>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, int64_t>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, double>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, bool>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, time>>
					|| std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date_time>>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (node.type())
				{
					case node_type::table:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, table>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<table>());
						break;
					case node_type::array:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, array>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<array>());
						break;
					case node_type::string:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, string>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<string>());
						break;
					case node_type::integer:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, int64_t>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<int64_t>());
						break;
					case node_type::floating_point:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, double>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<double>());
						break;
					case node_type::boolean:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, bool>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<bool>());
						break;
					case node_type::date:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<date>());
						break;
					case node_type::time:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, time>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<time>());
						break;
					case node_type::date_time:
						if constexpr (std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date_time>>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<date_time>());
						break;
					TOML_NO_DEFAULT_CASE;
				}

				static constexpr auto is_exhaustive =
					std::is_invocable_v<FUNC&&, ref_cast_type<N&&, table>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, array>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, string>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, int64_t>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, double>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, bool>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, time>>
					&& std::is_invocable_v<FUNC&&, ref_cast_type<N&&, date_time>>;

				if constexpr (is_exhaustive)
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
			///			do_something_with_a_string(n.get());
			///		else if constexpr (toml::is_floating_point<decltype(n)>)
			///			do_something_with_a_float(n.get());
			///		else
			///			throw std::exception("Expected string or integer")
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
			/// 		 Must be default-constructible if non-void and your visitor is not exhaustive.
			/// 
			/// \see https://en.wikipedia.org/wiki/Visitor_pattern
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) & TOML_MAY_THROW
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			/// \brief	Invokes a visitor on the node based on the node's concrete type (rvalue overload).
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) && TOML_MAY_THROW
			{
				return do_visit(std::move(*this), std::forward<FUNC>(visitor));
			}

			/// \brief	Invokes a visitor on the node based on the node's concrete type (const overload).
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const& TOML_MAY_THROW
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}
	};
}

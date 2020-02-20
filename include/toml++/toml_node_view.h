#pragma once
#include "toml_table.h"
#include "toml_array.h"
#include "toml_value.h"

namespace toml
{
	/// \brief	A view of a node.
	/// 
	/// \detail A node_view is like a std::optional<toml::node> with lots of toml-specific stuff built-in.
	/// 		It _may_ represent a node, and allows you to do many of the same operations that you'd do
	/// 		on nodes directly, as well as easily traversing the node tree by creating
	/// 		subviews (via node_view::operator[]). \cpp
	/// 
	/// auto tbl = toml::parse(R"(
	/// 
	///		title = "my hardware store"
	///		
	///		[[products]]
	///		name = "Hammer"
	///		sku = 738594937
	///		keywords = [ "hammer", "construction", "build" ]
	///
	///		[[products]]
	///		name = "Nail"
	///		sku = 284758393
	///		color = "gray"
	///		
	/// )"sv);
	/// 
	/// std::cout << tbl["title"] << std::endl;
	/// std::cout << tbl["products"][0]["name"] << std::endl;
	/// std::cout << tbl["products"][0]["keywords"] << std::endl;
	/// std::cout << tbl["products"][0]["keywords"][2] << std::endl;
	/// 
	/// tbl["products"][0]["keywords"].as_array()->push_back("heavy");
	/// std::cout << tbl["products"][0]["keywords"] << std::endl;
	/// std::cout << "has third product: "sv << !!tbl["products"][2] << std::endl;
	/// std::cout << tbl["products"][2] << std::endl; // no-op
	/// 
	/// \ecpp
	/// 
	/// \out
	/// "my hardware store"
	/// "Hammer"
	/// [ "hammer", "construction", "build" ]
	/// [ "hammer", "construction", "build", "heavy" ]
	/// has third product: false
	/// \eout
	template <typename T>
	class node_view final
	{
		public:
			using viewed_type = T;

		private:
			friend class toml::table;

			viewed_type* node_;

			TOML_NODISCARD_CTOR
			node_view(viewed_type* node) noexcept
				: node_{ node }
			{}

		public:

			/// \brief	Returns true if the view references a node.
			[[nodiscard]] explicit operator bool() const noexcept { return node_ != nullptr; }
			/// \brief	Returns the node that's being referenced by the view.
			[[nodiscard]] viewed_type* get() noexcept { return node_; }
			/// \brief	Returns the node that's being referenced by the view (const overload).
			[[nodiscard]] const viewed_type* get() const noexcept { return node_; }


			/// \brief	Returns the type identifier for the viewed node.
			[[nodiscard]] node_type type() const noexcept { return node_ ? node_->type() : node_type::none; }

			/// \brief	Returns true if the viewed node is a toml::table.
			[[nodiscard]] bool is_table() const noexcept { return type() == node_type::table; }
			/// \brief	Returns true if the viewed node is a toml::array.
			[[nodiscard]] bool is_array() const noexcept { return type() == node_type::array; }
			/// \brief	Returns true if the viewed node is a toml::value<>.
			[[nodiscard]] bool is_value() const noexcept { return type() > node_type::array; }
			/// \brief	Returns true if the viewed node is a toml::value<string>.
			[[nodiscard]] bool is_string() const noexcept { return type() == node_type::string; }
			/// \brief	Returns true if the viewed node is a toml::value<int64_t>.
			[[nodiscard]] bool is_integer() const noexcept { return type() == node_type::integer; }
			/// \brief	Returns true if the viewed node is a toml::value<double>.
			[[nodiscard]] bool is_floating_point() const noexcept { return type() == node_type::floating_point; }
			/// \brief	Returns true if the viewed node is a toml::value<bool>.
			[[nodiscard]] bool is_boolean() const noexcept { return type() == node_type::boolean; }
			/// \brief	Returns true if the viewed node is a toml::value<date>.
			[[nodiscard]] bool is_date() const noexcept { return type() == node_type::date; }
			/// \brief	Returns true if the viewed node is a toml::value<time>.
			[[nodiscard]] bool is_time() const noexcept { return type() == node_type::time; }
			/// \brief	Returns true if the viewed node is a toml::value<date_time>.
			[[nodiscard]] bool is_date_time() const noexcept { return type() == node_type::date_time; }

			/// \brief	Returns true if the viewed node is a toml::array that contains only tables.
			[[nodiscard]] bool is_array_of_tables() const noexcept
			{
				return node_ ? node_->is_array_of_tables() : false;
			}

			/// \brief	Checks if this view references a node of a specific type.
			///
			/// \tparam	U	A TOML node or value type.
			///
			/// \returns	Returns true if the viewed node is an instance of the specified type.
			/// 
			/// \see toml::node::is()
			template <typename U>
			[[nodiscard]]
			bool is() const noexcept
			{
				return node_ ? node_->template is<U>() : false;
			}

			/// \brief	Gets a pointer to the viewed node as a more specific node type.
			///
			/// \tparam	U	The node type or TOML value type to cast to.
			///
			/// \returns	A pointer to the node as the given type, or nullptr if it was a different type.
			/// 
			/// \see toml::node::as()
			template <typename U>
			[[nodiscard]]
			auto as() noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			/// \brief	Gets a pointer to the viewed node as a more specific node type (const overload).
			template <typename U>
			[[nodiscard]]
			const impl::node_of<U>* as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			/// \brief	Returns a pointer to the viewed node as a toml::table, if it is one.
			[[nodiscard]] auto as_table() noexcept { return as<table>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::array, if it is one.
			[[nodiscard]] auto as_array() noexcept { return as<array>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<string>, if it is one.
			[[nodiscard]] auto as_string() noexcept { return as<string>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<int64_t>, if it is one.
			[[nodiscard]] auto as_integer() noexcept { return as<int64_t>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<double>, if it is one.
			[[nodiscard]] auto as_floating_point() noexcept { return as<double>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<bool>, if it is one.
			[[nodiscard]] auto as_boolean() noexcept { return as<bool>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<date>, if it is one.
			[[nodiscard]] auto as_date() noexcept { return as<date>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<time>, if it is one.
			[[nodiscard]] auto as_time() noexcept { return as<time>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<date_time>, if it is one.
			[[nodiscard]] auto as_date_time() noexcept { return as<date_time>(); }

			[[nodiscard]] const table* as_table() const noexcept { return as<table>(); }
			[[nodiscard]] const array* as_array() const noexcept { return as<array>(); }
			[[nodiscard]] const value<string>* as_string() const noexcept { return as<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept { return as<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept { return as<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept { return as<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept { return as<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept { return as<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept { return as<date_time>(); }

		private:

			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N* node, FUNC&& visitor)
				TOML_MAY_THROW_UNLESS(noexcept(std::declval<N*>()->visit(std::declval<FUNC&&>())))
			{
				using return_type = decltype(node->visit(std::forward<FUNC>(visitor)));
				if (node)
					return node->visit(std::forward<FUNC>(visitor));
				if constexpr (!std::is_void_v<return_type>)
					return return_type{};
			}

			template <typename FUNC, typename N>
			static constexpr bool visit_is_nothrow =
				noexcept(do_visit(std::declval<N*>(), std::declval<FUNC&&>()));

		public:

			/// \brief	Invokes a visitor on the viewed node based on its concrete type.
			/// 
			/// \remarks Has no effect if the view does not reference a node.
			/// 
			/// \see node::visit()
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor)
				TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, viewed_type>)
			{
				return do_visit(node_, std::forward<FUNC>(visitor));
			}

			/// \brief	Invokes a visitor on the viewed node based on its concrete type (const overload).
			/// 
			/// \remarks Has no effect if the view does not reference a node.
			/// 
			///  \see node::visit()
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const
				TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, const viewed_type>)
			{
				return do_visit(node_, std::forward<FUNC>(visitor));
			}

			/// \brief	Returns true if the viewed node is a table with the same contents as RHS.
			[[nodiscard]] friend bool operator == (const node_view& lhs, const table& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto tbl = lhs.as<table>();
				return tbl && *tbl == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const table&, )

			/// \brief	Returns true if the viewed node is an array with the same contents as RHS.
			[[nodiscard]] friend bool operator == (const node_view& lhs, const array& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const array&, )

			/// \brief	Returns true if the viewed node is a value with the same value as RHS.
			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const value<U>& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto val = lhs.as<U>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const value<U>&, template <typename U>)

			/// \brief	Returns true if the viewed node is a value with the same value as RHS.
			template <typename U, typename = std::enable_if_t<impl::is_value_or_promotable<U>>>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const U& rhs) noexcept
			{
				const auto val = lhs.as<impl::promoted<U>>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(
				const node_view&,
				const U&,
				template <typename U, typename = std::enable_if_t<impl::is_value_or_promotable<U>>>
			)

			/// \brief	Returns true if the viewed node is an array with the same contents as the RHS initializer list.
			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const std::initializer_list<U>& rhs) noexcept
			{
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::initializer_list<U>&, template <typename U>)

			/// \brief	Returns true if the viewed node is an array with the same contents as the RHS vector.
			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const std::vector<U>& rhs) noexcept
			{
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::vector<U>&, template <typename U>)

			/// \brief	Returns a view of the selected subnode.
			///
			/// \param 	key	The key of the node to retrieve
			///
			/// \returns	A view of the selected node if this node represented a table and it contained a
			/// 			value at the given key, or an empty view.
			[[nodiscard]] node_view<viewed_type> operator[] (string_view key) noexcept
			{
				if (auto tbl = this->as_table())
					return { tbl->get(key) };
				return { nullptr };
			}

			/// \brief	Returns a view of the selected subnode.
			///
			/// \param 	index The index of the node to retrieve
			///
			/// \returns	A view of the selected node if this node represented an array and it contained a
			/// 			value at the given index, or an empty view.
			[[nodiscard]] node_view<viewed_type> operator[] (size_t index) noexcept
			{
				if (auto tbl = this->as_array())
					return { tbl->get(index) };
				return { nullptr };
			}

			/// \brief	Returns a view of the selected subnode (const overload).
			[[nodiscard]] node_view<const viewed_type> operator[] (string_view key) const noexcept
			{
				if (auto tbl = this->as_table())
					return { tbl->get(key) };
				return { nullptr };
			}

			/// \brief	Returns a view of the selected subnode (const overload).
			[[nodiscard]] node_view<const viewed_type> operator[] (size_t index) const noexcept
			{
				if (auto tbl = this->as_array())
					return { tbl->get(index) };
				return { nullptr };
			}


			/// \brief	Prints the viewed node out to a stream.
			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& os, const node_view& nv) TOML_MAY_THROW
			{
				nv.visit([&](const auto& node) TOML_MAY_THROW
				{
					os << node;
				});
				return os;
			}
	};

	inline node_view<node> table::operator[] (string_view key) noexcept
	{
		return { this->get(key) };
	}

	inline node_view<const node> table::operator[] (string_view key) const noexcept
	{
		return { this->get(key) };
	}
}

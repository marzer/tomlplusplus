//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_table.h"
#include "toml_array.h"
#include "toml_value.h"

namespace toml
{
	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const node_view<T>&);

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
	/// std::cout << "has product[2]: "sv << !!tbl["products"][2] << std::endl;
	/// std::cout << "product[2]: "sv << tbl["products"][2] << std::endl;
		/// \ecpp
	/// 
	/// \out
	/// "my hardware store"
	/// "Hammer"
	/// [ "hammer", "construction", "build" ]
	/// "build"
	/// [ "hammer", "construction", "build", "heavy" ]
	/// has product[2]: false
	/// product[2]:
	/// \eout
	template <typename T>
	class TOML_API node_view final
	{
		public:
			using viewed_type = T;

		private:
			friend class toml::table;
			template <typename U> friend class toml::node_view;

			mutable viewed_type* node_;

			TOML_NODISCARD_CTOR
			node_view(viewed_type* node) noexcept
				: node_{ node }
			{}

			template <typename FUNC>
			static constexpr bool visit_is_nothrow
				= noexcept(std::declval<viewed_type*>()->visit(std::declval<FUNC&&>()));

		public:

			/// \brief	Returns true if the view references a node.
			[[nodiscard]] explicit operator bool() const noexcept { return node_ != nullptr; }
			/// \brief	Returns the node that's being referenced by the view.
			[[nodiscard]] viewed_type* get() const noexcept { return node_; }

			/// \brief	Returns the type identifier for the viewed node.
			[[nodiscard]] node_type type() const noexcept { return node_ ? node_->type() : node_type::none; }

			/// \brief	Returns true if the viewed node is a toml::table.
			[[nodiscard]] bool is_table() const noexcept { return node_ && node_->is_table(); }
			/// \brief	Returns true if the viewed node is a toml::array.
			[[nodiscard]] bool is_array() const noexcept { return node_ && node_->is_array(); }
			/// \brief	Returns true if the viewed node is a toml::value<>.
			[[nodiscard]] bool is_value() const noexcept { return node_ && node_->is_value(); }
			/// \brief	Returns true if the viewed node is a toml::value<string>.
			[[nodiscard]] bool is_string() const noexcept { return node_ && node_->is_string(); }
			/// \brief	Returns true if the viewed node is a toml::value<int64_t>.
			[[nodiscard]] bool is_integer() const noexcept { return node_ && node_->is_integer(); }
			/// \brief	Returns true if the viewed node is a toml::value<double>.
			[[nodiscard]] bool is_floating_point() const noexcept { return node_ && node_->is_floating_point(); }
			/// \brief	Returns true if the viewed node is a toml::value<int64_t> or toml::value<double>.
			[[nodiscard]] bool is_number() const noexcept { return node_ && node_->is_number(); }
			/// \brief	Returns true if the viewed node is a toml::value<bool>.
			[[nodiscard]] bool is_boolean() const noexcept { return node_ && node_->is_boolean(); }
			/// \brief	Returns true if the viewed node is a toml::value<date>.
			[[nodiscard]] bool is_date() const noexcept { return node_ && node_->is_date(); }
			/// \brief	Returns true if the viewed node is a toml::value<time>.
			[[nodiscard]] bool is_time() const noexcept { return node_ && node_->is_time(); }
			/// \brief	Returns true if the viewed node is a toml::value<date_time>.
			[[nodiscard]] bool is_date_time() const noexcept { return node_ && node_->is_date_time(); }
			/// \brief	Returns true if the viewed node is a toml::array that contains only tables.
			[[nodiscard]] bool is_array_of_tables() const noexcept { return node_ && node_->is_array_of_tables(); }

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
			auto as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			/// \brief	Returns a pointer to the viewed node as a toml::table, if it is one.
			[[nodiscard]] auto as_table() const noexcept { return as<table>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::array, if it is one.
			[[nodiscard]] auto as_array() const noexcept { return as<array>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<string>, if it is one.
			[[nodiscard]] auto as_string() const noexcept { return as<string>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<int64_t>, if it is one.
			[[nodiscard]] auto as_integer() const noexcept { return as<int64_t>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<double>, if it is one.
			[[nodiscard]] auto as_floating_point() const noexcept { return as<double>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<bool>, if it is one.
			[[nodiscard]] auto as_boolean() const noexcept { return as<bool>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<date>, if it is one.
			[[nodiscard]] auto as_date() const noexcept { return as<date>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<time>, if it is one.
			[[nodiscard]] auto as_time() const noexcept { return as<time>(); }
			/// \brief	Returns a pointer to the viewed node as a toml::value<date_time>, if it is one.
			[[nodiscard]] auto as_date_time() const noexcept { return as<date_time>(); }

			/// \brief	Gets the raw value contained by the referenced node.
			/// 
			/// \tparam	U	One of the TOML value types. Can also be a string_view.
			///
			/// \returns	The underlying value if the node was a value of the matching type (or convertible to it), or an empty optional.
			/// 
			/// \see node::value()
			template <typename U>
			[[nodiscard]] optional<U> value() const noexcept
			{
				if (node_)
					return node_->template value<U>();
				return {};
			}

			/// \brief	Gets the raw value contained by the referenced node, or a default.
			///
			/// \tparam	U	Default value type. Must be (or be promotable to) one of the TOML value types.
			/// \param 	default_value	The default value to return if the view did not reference a node,
			/// 						or if the node wasn't a value, wasn't the correct type, or no conversion was possible.
			///
			/// \returns	The node's underlying value, or the default if the node wasn't a value, wasn't the
			/// 			correct type, or no conversion was possible.
			/// 
			/// \see node::value_or()
			template <typename U>
			[[nodiscard]] auto value_or(U&& default_value) const noexcept
			{
				using return_type = decltype(node_->value_or(std::forward<U>(default_value)));
				if (node_)
					return node_->value_or(std::forward<U>(default_value));
				return return_type{ std::forward<U>(default_value) };
			}

			/// \brief	Invokes a visitor on the viewed node based on its concrete type.
			/// 
			/// \remarks Has no effect if the view does not reference a node.
			/// 
			/// \see node::visit()
			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const
				noexcept(visit_is_nothrow<FUNC&&>)
			{
				using return_type = decltype(node_->visit(std::forward<FUNC>(visitor)));
				if (node_)
					return node_->visit(std::forward<FUNC>(visitor));
				if constexpr (!std::is_void_v<return_type>)
					return return_type{};
			}

			/// \brief	Gets a raw reference to the viewed node's underlying data.
			///
			/// \warning This function is dangerous if used carelessly and **WILL** break your code if the 
			/// 		 node_view didn't reference a node, or the chosen value type doesn't match the node's
			/// 		 actual type. In debug builds an assertion will fire when invalid accesses are attempted: \cpp
			///
			/// auto tbl = toml::parse(R"(
			///	
			///	min = 32
			///	max = 45
			/// 
			/// )"sv);
			/// 
			/// auto& min_ref = tbl["min"].ref<int64_t>(); // this is OK
			/// auto& max_ref = tbl["max"].ref<double>(); // hits assertion because the type is wrong
			/// auto& min_ref = tbl["foo"].ref<int64_t>(); // hits assertion because "foo" didn't exist
			///
			/// \ecpp
			/// 
			/// \tparam	U	One of the TOML value types.
			///
			/// \returns	A reference to the underlying data.
			template <typename U>
			[[nodiscard]] decltype(auto) ref() const noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);
				TOML_ASSERT(
					node_
					&& "toml::node_view::ref() called on a node_view that did not reference a node"
				);
				return node_->template ref<type>();
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
			[[nodiscard]] friend bool operator == (const node_view& lhs, const toml::value<U>& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto val = lhs.as<U>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const toml::value<U>&, template <typename U>)

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
			[[nodiscard]] node_view operator[] (string_view key) const noexcept
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
			[[nodiscard]] node_view operator[] (size_t index) const noexcept
			{
				if (auto arr = this->as_array())
					return { arr->get(index) };
				return { nullptr };
			}

			template <typename CHAR, typename U>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const node_view<U>&);
	};

	#if !TOML_ALL_INLINE
	extern template class TOML_API node_view<node>;
	extern template class TOML_API node_view<const node>;
	#endif

	/// \brief	Prints the viewed node out to a stream.
	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& os, const node_view<T>& nv)
	{
		if (nv.node_)
		{
			nv.node_->visit([&os](const auto& n)
			{
				os << n;
			});
		}
		return os;
	}
}


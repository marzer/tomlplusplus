//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_map.h"
#include "std_initializer_list.h"
#include "array.h"
#include "make_node.h"
#include "node_view.h"
#include "header_start.h"

/// \cond
TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	struct table_proxy_pair
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const std::string& first;
		value_type& second;
	};

	template <bool IsConst>
	class table_iterator
	{
	  private:
		template <bool>
		friend class table_iterator;
		friend class TOML_NAMESPACE::table;

		using proxy_type		   = table_proxy_pair<IsConst>;
		using raw_mutable_iterator = std::map<std::string, std::unique_ptr<node>, std::less<>>::iterator;
		using raw_const_iterator   = std::map<std::string, std::unique_ptr<node>, std::less<>>::const_iterator;
		using raw_iterator		   = std::conditional_t<IsConst, raw_const_iterator, raw_mutable_iterator>;

		mutable raw_iterator raw_;
		mutable std::aligned_storage_t<sizeof(proxy_type), alignof(proxy_type)> proxy;
		mutable bool proxy_instantiated = false;

		TOML_NODISCARD
		proxy_type* get_proxy() const noexcept
		{
			if (!proxy_instantiated)
			{
				auto p			   = ::new (static_cast<void*>(&proxy)) proxy_type{ raw_->first, *raw_->second.get() };
				proxy_instantiated = true;
				return p;
			}
			else
				return TOML_LAUNDER(reinterpret_cast<proxy_type*>(&proxy));
		}

		TOML_NODISCARD_CTOR
		table_iterator(raw_mutable_iterator raw) noexcept //
			: raw_{ raw }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		table_iterator(raw_const_iterator raw) noexcept //
			: raw_{ raw }
		{}

	  public:
		TOML_NODISCARD_CTOR
		table_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator& other) noexcept //
			: raw_{ other.raw_ }
		{}

		table_iterator& operator=(const table_iterator& rhs) noexcept
		{
			raw_			   = rhs.raw_;
			proxy_instantiated = false;
			return *this;
		}

		using value_type		= table_proxy_pair<IsConst>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= typename std::iterator_traits<raw_iterator>::difference_type;
		using iterator_category = typename std::iterator_traits<raw_iterator>::iterator_category;

		table_iterator& operator++() noexcept // ++pre
		{
			++raw_;
			proxy_instantiated = false;
			return *this;
		}

		table_iterator operator++(int) noexcept // post++
		{
			table_iterator out{ raw_ };
			++raw_;
			proxy_instantiated = false;
			return out;
		}

		table_iterator& operator--() noexcept // --pre
		{
			--raw_;
			proxy_instantiated = false;
			return *this;
		}

		table_iterator operator--(int) noexcept // post--
		{
			table_iterator out{ raw_ };
			--raw_;
			proxy_instantiated = false;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.raw_ == rhs.raw_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.raw_ != rhs.raw_;
		}

		TOML_DISABLE_WARNINGS;

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_NODISCARD
		operator table_iterator<true>() const noexcept
		{
			return table_iterator<true>{ raw_ };
		}

		TOML_ENABLE_WARNINGS;
	};

	struct table_init_pair
	{
		mutable std::string key;
		mutable std::unique_ptr<node> value;

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(std::string&& k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ std::move(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(std::string_view k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ k },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(const char* k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ k },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

#if TOML_ENABLE_WINDOWS_COMPAT

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(std::wstring&& k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ narrow(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(std::wstring_view k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ narrow(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

		template <typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(const wchar_t* k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ narrow(std::wstring_view{ k }) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}

#endif
	};
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief A BidirectionalIterator for iterating over key-value pairs in a toml::table.
	using table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<false>);

	/// \brief A BidirectionalIterator for iterating over const key-value pairs in a toml::table.
	using const_table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<true>);

	/// \brief	A TOML table.
	///
	/// \remarks The interface of this type is modeled after std::map, with some
	/// 		additional considerations made for the heterogeneous nature of a
	/// 		TOML table, and for the removal of some cruft (the public interface of
	/// 		std::map is, simply, _a hot mess_).
	class table : public node
	{
	  private:
		/// \cond

		friend class TOML_PARSER_TYPENAME;

		std::map<std::string, std::unique_ptr<node>, std::less<>> map_;
		bool inline_ = false;

		TOML_NODISCARD_CTOR
		TOML_API
		table(const impl::table_init_pair*, const impl::table_init_pair*);

		/// \endcond

	  public:
		/// \brief A BidirectionalIterator for iterating over key-value pairs in a toml::table.
		using iterator = table_iterator;

		/// \brief A BidirectionalIterator for iterating over const key-value pairs in a toml::table.
		using const_iterator = const_table_iterator;

#if TOML_LIFETIME_HOOKS

		TOML_NODISCARD_CTOR
		table() noexcept
		{
			TOML_TABLE_CREATED;
		}

		~table() noexcept
		{
			TOML_TABLE_DESTROYED;
		}
#else

		/// \brief	Default constructor.
		TOML_NODISCARD_CTOR
		table() noexcept = default;

#endif

		/// \brief	Copy constructor.
		TOML_NODISCARD_CTOR
		TOML_API
		table(const table&);

		/// \brief	Move constructor.
		TOML_NODISCARD_CTOR
		TOML_API
		table(table&& other) noexcept;

		/// \brief	Constructs a table with one or more initial key-value pairs.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "foo", 1 },
		///		{ "bar", 2.0 },
		///		{ "kek", "three" }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { foo = 1, bar = 2.0, kek = "three" }
		/// \eout
		///
		/// \param 	kvps	A list of key-value pairs used to initialize the table.
		TOML_NODISCARD_CTOR
		explicit table(std::initializer_list<impl::table_init_pair> kvps) //
			: table{ kvps.begin(), kvps.end() }
		{}

		/// \brief	Copy-assignment operator.
		TOML_API
		table& operator=(const table&);

		/// \brief	Move-assignment operator.
		TOML_API
		table& operator=(table&& rhs) noexcept;

		/// \name Type checks
		/// @{

		/// \brief Returns #toml::node_type::table.
		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::table;
		}

		TOML_PURE_GETTER
		TOML_API
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_PURE_GETTER
		TOML_API
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_PURE_GETTER
		TOML_API
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		/// \cond
		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::unwrap_node<ElemType>;

			static_assert(
				std::is_void_v<
					type> || ((impl::is_native<type> || impl::is_one_of<type, table, array>)&&!impl::is_cvref<type>),
				"The template type argument of table::is_homogeneous() must be void or one "
				"of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}
		/// \endcond

		/// \brief Returns `true`.
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return true;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		/// \brief Returns `false`.
		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		/// @}

		/// \name Type casts
		/// @{

		/// \brief Returns a pointer to the table.
		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return this;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		/// \brief Returns a const-qualified pointer to the table.
		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return this;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		/// \brief Returns `nullptr`.
		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		/// @}

		/// \name Metadata
		/// @{

		/// \brief	Returns true if this table is an inline table.
		///
		/// \remarks Runtime-constructed tables (i.e. those not created during
		/// 		 parsing) are not inline by default.
		TOML_PURE_INLINE_GETTER
		bool is_inline() const noexcept
		{
			return inline_;
		}

		/// \brief	Sets whether this table is a TOML inline table.
		///
		/// \detail \godbolt{an9xdj}
		///
		/// \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 },
		///		{ "d", toml::table{ { "e", 4 } } }
		///	};
		/// std::cout << "is inline? "sv << tbl.is_inline() << "\n";
		/// std::cout << tbl << "\n\n";
		///
		/// tbl.is_inline(!tbl.is_inline());
		/// std::cout << "is inline? "sv << tbl.is_inline() << "\n";
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// is inline? false
		/// a = 1
		/// b = 2
		/// c = 3
		///
		/// [d]
		/// e = 4
		///
		///
		/// is inline? true
		/// { a = 1, b = 2, c = 3, d = { e = 4 } }
		/// \eout
		///
		/// \remarks A table being 'inline' is only relevent during printing;
		/// 		 it has no effect on the general functionality of the table
		/// 		 object.
		///
		/// \param 	val	The new value for 'inline'.
		void is_inline(bool val) noexcept
		{
			inline_ = val;
		}

		/// @}

		/// \name Value retrieval
		/// @{

		/// \brief	Gets the node at a specific key.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 42, },
		///		{ "b", "is the meaning of life, apparently." }
		///	};
		///	std::cout << R"(node ["a"] exists: )"sv << !!arr.get("a") << "\n";
		///	std::cout << R"(node ["b"] exists: )"sv << !!arr.get("b") << "\n";
		///	std::cout << R"(node ["c"] exists: )"sv << !!arr.get("c") << "\n";
		/// if (auto val = arr.get("a"))
		///		std::cout << R"(node ["a"] was an )"sv << val->type() << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// node ["a"] exists: true
		/// node ["b"] exists: true
		/// node ["c"] exists: false
		/// node ["a"] was an integer
		/// \eout
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A pointer to the node at the specified key, or nullptr.
		TOML_PURE_GETTER
		TOML_API
		node* get(std::string_view key) noexcept;

		/// \brief	Gets the node at a specific key (const overload).
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A pointer to the node at the specified key, or nullptr.
		TOML_PURE_INLINE_GETTER
		const node* get(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).get(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets the node at a specific key.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A pointer to the node at the specified key, or nullptr.
		TOML_NODISCARD
		node* get(std::wstring_view key)
		{
			return get(impl::narrow(key));
		}

		/// \brief	Gets the node at a specific key (const overload).
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A pointer to the node at the specified key, or nullptr.
		TOML_NODISCARD
		const node* get(std::wstring_view key) const
		{
			return const_cast<table&>(*this).get(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets the node at a specific key if it is a particular type.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 42, },
		///		{ "b", "is the meaning of life, apparently." }
		///	};
		/// if (auto val = arr.get_as<int64_t>("a"))
		///		std::cout << R"(node ["a"] was an integer with value )"sv << **val << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// node ["a"] was an integer with value 42
		/// \eout
		///
		/// \tparam	ValueType	One of the TOML node or value types.
		/// \param 	key			The node's key.
		///
		/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
		template <typename ValueType>
		TOML_PURE_GETTER
		impl::wrap_node<ValueType>* get_as(std::string_view key) noexcept
		{
			const auto n = this->get(key);
			return n ? n->template as<ValueType>() : nullptr;
		}

		/// \brief	Gets the node at a specific key if it is a particular type (const overload).
		///
		/// \tparam	ValueType	One of the TOML node or value types.
		/// \param 	key			The node's key.
		///
		/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
		template <typename ValueType>
		TOML_PURE_GETTER
		const impl::wrap_node<ValueType>* get_as(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).template get_as<ValueType>(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets the node at a specific key if it is a particular type.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \tparam	ValueType	One of the TOML node or value types.
		/// \param 	key			The node's key.
		///
		/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
		template <typename ValueType>
		TOML_NODISCARD
		impl::wrap_node<ValueType>* get_as(std::wstring_view key)
		{
			return get_as<ValueType>(impl::narrow(key));
		}

		/// \brief	Gets the node at a specific key if it is a particular type (const overload).
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \tparam	ValueType	One of the TOML node or value types.
		/// \param 	key			The node's key.
		///
		/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
		template <typename ValueType>
		TOML_NODISCARD
		const impl::wrap_node<ValueType>* get_as(std::wstring_view key) const
		{
			return const_cast<table&>(*this).template get_as<ValueType>(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets a reference to the element at a specific key, throwing `std::out_of_range` if none existed.
		TOML_NODISCARD
		TOML_API
		node& at(std::string_view key);

		/// \brief	Gets a reference to the element at a specific key, throwing `std::out_of_range` if none existed.
		TOML_NODISCARD
		const node& at(std::string_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets a reference to the element at a specific key, throwing `std::out_of_range` if none existed.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		node& at(std::wstring_view key)
		{
			return at(impl::narrow(key));
		}

		/// \brief	Gets a reference to the element at a specific key, throwing `std::out_of_range` if none existed.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		const node& at(std::wstring_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// @}

		/// \name Table operations
		/// @{

		/// \brief	Returns an iterator to the first key-value pair.
		TOML_NODISCARD
		iterator begin() noexcept
		{
			return { map_.begin() };
		}

		/// \brief	Returns an iterator to the first key-value pair.
		TOML_NODISCARD
		const_iterator begin() const noexcept
		{
			return { map_.cbegin() };
		}

		/// \brief	Returns an iterator to the first key-value pair.
		TOML_NODISCARD
		const_iterator cbegin() const noexcept
		{
			return { map_.cbegin() };
		}

		/// \brief	Returns an iterator to one-past-the-last key-value pair.
		TOML_NODISCARD
		iterator end() noexcept
		{
			return { map_.end() };
		}

		/// \brief	Returns an iterator to one-past-the-last key-value pair.
		TOML_NODISCARD
		const_iterator end() const noexcept
		{
			return { map_.cend() };
		}

		/// \brief	Returns an iterator to one-past-the-last key-value pair.
		TOML_NODISCARD
		const_iterator cend() const noexcept
		{
			return { map_.cend() };
		}

		/// \brief	Returns true if the table is empty.
		TOML_NODISCARD
		bool empty() const noexcept
		{
			return map_.empty();
		}

		/// \brief	Returns the number of key-value pairs in the table.
		TOML_NODISCARD
		size_t size() const noexcept
		{
			return map_.size();
		}

		/// \brief	Removes all key-value pairs from the table.
		void clear() noexcept
		{
			map_.clear();
		}

		/// \brief	Inserts a new value at a specific key if one did not already exist.
		///
		/// \detail \godbolt{bMnW5r}
		///
		/// \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// for (auto k : { "a", "d" })
		/// {
		///		auto result = tbl.insert(k, 42);
		///		std::cout << "inserted with key '"sv << k << "': "sv << result.second << "\n";
		/// }
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// a = 1
		/// b = 2
		/// c = 3
		///
		/// inserted with key 'a': false
		/// inserted with key 'd': true
		/// a = 1
		/// b = 2
		/// c = 3
		/// d = 42
		/// \eout
		///
		/// \tparam KeyType		std::string (or a type convertible to it).
		/// \tparam ValueType	toml::node, toml::node_view, toml::table, toml::array, or a native TOML value type
		/// 					(or a type promotable to one).
		/// \param 	key			The key at which to insert the new value.
		/// \param 	val			The new value to insert.
		/// \param	flags		Value flags to apply to new values.
		///
		/// \returns	\conditional_return{Valid input}
		/// 			<ul>
		/// 				<li>An iterator to the insertion position (or the position of the value that prevented insertion)
		/// 				<li>A boolean indicating if the insertion was successful.
		/// 			</ul>
		///				\conditional_return{Input is an empty toml::node_view}
		/// 			`{ end(), false }`
		///
		/// \attention The return value will always be `{ end(), false }` if the input value was an
		/// 		   empty toml::node_view, because no insertion can take place. This is the only circumstance
		/// 		   in which this can occur.
		TOML_CONSTRAINED_TEMPLATE((std::is_convertible_v<KeyType&&, std::string_view> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert(KeyType&& key,
										 ValueType&& val,
										 value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert(impl::narrow(static_cast<KeyType&&>(key)), static_cast<ValueType&&>(val), flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				auto ipos = map_.lower_bound(key);
				if (ipos == map_.end() || ipos->first != key)
				{
					ipos = map_.emplace_hint(ipos,
											 static_cast<KeyType&&>(key),
											 impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		/// \brief	Inserts a series of key-value pairs into the table.
		///
		/// \detail \godbolt{bzYcce}
		///
		/// \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// auto kvps = std::array<std::pair<std::string, int>, 2>{{
		///		{ "d", 42 },
		///		{ "a", 43 } // won't be inserted, 'a' already exists
		///	}};
		///	tbl.insert(kvps.begin(), kvps.end());
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// a = 1
		/// b = 2
		/// c = 3
		///
		/// a = 1
		/// b = 2
		/// c = 3
		/// d = 42
		/// \eout
		///
		/// \tparam Iter	An InputIterator to a collection of key-value pairs.
		/// \param 	first	An iterator to the first value in the input collection.
		/// \param 	last	An iterator to one-past-the-last value in the input collection.
		/// \param	flags		Value flags to apply to new values.
		///
		/// \remarks This function is morally equivalent to calling `insert(key, value)` for each
		/// 		 key-value pair covered by the iterator range, so any values with keys already found in the
		/// 		 table will not be replaced.
		TOML_CONSTRAINED_TEMPLATE((!std::is_convertible_v<Iter, std::string_view> && !impl::is_wide_string<Iter>),
								  typename Iter)
		void insert(Iter first, Iter last, value_flags flags = preserve_source_value_flags)
		{
			if (first == last)
				return;
			for (auto it = first; it != last; it++)
			{
				if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
					insert(std::move((*it).first), std::move((*it).second), flags);
				else
					insert((*it).first, (*it).second, flags);
			}
		}

		/// \brief	Inserts or assigns a value at a specific key.
		///
		/// \detail \godbolt{ddK563}
		///
		/// \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// for (auto k : { "a", "d" })
		/// {
		///		auto result = tbl.insert_or_assign(k, 42);
		///		std::cout << "value at key '"sv << k
		///			<< "' was "sv << (result.second ? "inserted"sv : "assigned"sv) << "\n";
		/// }
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// a = 1
		/// b = 2
		/// c = 3
		///
		/// value at key 'a' was assigned
		/// value at key 'd' was inserted
		/// a = 42
		/// b = 2
		/// c = 3
		/// d = 42
		/// \eout
		///
		/// \tparam KeyType		std::string (or a type convertible to it).
		/// \tparam ValueType	toml::node, toml::node_view, toml::table, toml::array, or a native TOML value type
		/// 					(or a type promotable to one).
		/// \param 	key			The key at which to insert or assign the value.
		/// \param 	val			The value to insert/assign.
		/// \param	flags		Value flags to apply to new values.
		///
		/// \returns	\conditional_return{Valid input}
		/// 			<ul>
		/// 				<li>An iterator to the value's position
		/// 				<li>`true` if the value was inserted, `false` if it was assigned.
		/// 			</ul>
		/// 			\conditional_return{Input is an empty toml::node_view}
		/// 			 `{ end(), false }`
		///
		/// \attention The return value will always be `{ end(), false }` if the input value was
		/// 		   an empty toml::node_view, because no insertion or assignment can take place.
		/// 		   This is the only circumstance in which this can occur.
		template <typename KeyType, typename ValueType>
		std::pair<iterator, bool> insert_or_assign(KeyType&& key,
												   ValueType&& val,
												   value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert_or_assign(impl::narrow(static_cast<KeyType&&>(key)),
										static_cast<ValueType&&>(val),
										flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				auto ipos = map_.lower_bound(key);
				if (ipos == map_.end() || ipos->first != key)
				{
					ipos = map_.emplace_hint(ipos,
											 static_cast<KeyType&&>(key),
											 impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				else
				{
					(*ipos).second.reset(impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, false };
				}
			}
		}

		/// \brief	Emplaces a new value at a specific key if one did not already exist.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// for (auto k : { "a", "d" })
		/// {
		///		// add a string using std::string's substring constructor
		///		auto result = tbl.emplace<std::string>(k, "this is not a drill"sv, 14, 5);
		///		std::cout << "emplaced with key '"sv << k << "': "sv << result.second << "\n";
		/// }
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { a = 1, b = 2, c = 3 }
		/// emplaced with key 'a': false
		/// emplaced with key 'd': true
		/// { a = 1, b = 2, c = 3, d = "drill" }
		/// \eout
		///
		/// \tparam ValueType	toml::table, toml::array, or any native TOML value type.
		/// \tparam KeyType		std::string (or a type convertible to it).
		/// \tparam ValueArgs	Value constructor argument types.
		/// \param 	key			The key at which to emplace the new value.
		/// \param 	args		Arguments to forward to the value's constructor.
		///
		/// \returns A std::pair containing: <br>
		/// 		- An iterator to the emplacement position (or the position of the value that prevented emplacement)
		/// 		- A boolean indicating if the emplacement was successful.
		///
		/// \remark There is no difference between insert() and emplace() for trivial value types (floats, ints, bools).
		template <typename ValueType, typename KeyType, typename... ValueArgs>
		std::pair<iterator, bool> emplace(KeyType&& key, ValueArgs&&... args)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Emplacement using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return emplace<ValueType>(impl::narrow(static_cast<KeyType&&>(key)), static_cast<ValueArgs&&>(args)...);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				using type = impl::unwrap_node<ValueType>;
				static_assert((impl::is_native<type> || impl::is_one_of<type, table, array>)&&!impl::is_cvref<type>,
							  "The emplacement type argument of table::emplace() must be one "
							  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

				auto ipos = map_.lower_bound(key);
				if (ipos == map_.end() || ipos->first != key)
				{
					ipos = map_.emplace_hint(ipos,
											 static_cast<KeyType&&>(key),
											 new impl::wrap_node<type>{ static_cast<ValueArgs&&>(args)... });
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		/// \brief	Removes the specified key-value pair from the table.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// tbl.erase(tbl.begin() + 1);
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { a = 1, b = 2, c = 3 }
		/// { a = 1, c = 3 }
		/// \eout
		///
		/// \param 	pos		Iterator to the key-value pair being erased.
		///
		/// \returns Iterator to the first key-value pair immediately following the removed key-value pair.
		iterator erase(iterator pos) noexcept
		{
			return { map_.erase(pos.raw_) };
		}

		/// \brief	Removes the specified key-value pair from the table (const iterator overload).
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// tbl.erase(tbl.cbegin() + 1);
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { a = 1, b = 2, c = 3 }
		/// { a = 1, c = 3 }
		/// \eout
		///
		/// \param 	pos		Iterator to the key-value pair being erased.
		///
		/// \returns Iterator to the first key-value pair immediately following the removed key-value pair.
		iterator erase(const_iterator pos) noexcept
		{
			return { map_.erase(pos.raw_) };
		}

		/// \brief	Removes the key-value pairs in the range [first, last) from the table.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", "bad" },
		///		{ "c", "karma" },
		///		{ "d", 2 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// tbl.erase(tbl.cbegin() + 1, tbl.cbegin() + 3);
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { a = 1, b = "bad", c = "karma", d = 2 }
		/// { a = 1, d = 2 }
		/// \eout
		///
		/// \param 	first	Iterator to the first key-value pair being erased.
		/// \param 	last	Iterator to the one-past-the-last key-value pair being erased.
		///
		/// \returns Iterator to the first key-value pair immediately following the last removed key-value pair.
		iterator erase(const_iterator first, const_iterator last) noexcept
		{
			return { map_.erase(first.raw_, last.raw_) };
		}

		/// \brief	Removes the value with the given key from the table.
		///
		/// \detail \cpp
		/// auto tbl = toml::table{
		///		{ "a", 1 },
		///		{ "b", 2 },
		///		{ "c", 3 }
		///	};
		/// std::cout << tbl << "\n";
		///
		/// std::cout << tbl.erase("b") << "\n";
		/// std::cout << tbl.erase("not an existing key") << "\n";
		/// std::cout << tbl << "\n";
		///
		/// \ecpp
		///
		/// \out
		/// { a = 1, b = 2, c = 3 }
		/// true
		/// false
		/// { a = 1, c = 3 }
		/// \eout
		///
		/// \param 	key		Key to erase.
		///
		/// \returns True if any values with matching keys were found and erased.
		bool erase(std::string_view key) noexcept
		{
			if (auto it = map_.find(key); it != map_.end())
			{
				map_.erase(it);
				return true;
			}
			return false;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Removes the value with the given key from the table.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key		Key to erase.
		///
		/// \returns True if any values with matching keys were found and erased.
		bool erase(std::wstring_view key)
		{
			return erase(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets an iterator to the node at a specific key.
		///
		/// \param 	key	The node's key.
		///
		/// \returns	An iterator to the node at the specified key, or end().
		TOML_NODISCARD
		iterator find(std::string_view key) noexcept
		{
			return { map_.find(key) };
		}

		/// \brief	Gets an iterator to the node at a specific key (const overload)
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A const iterator to the node at the specified key, or cend().
		TOML_NODISCARD
		const_iterator find(std::string_view key) const noexcept
		{
			return { map_.find(key) };
		}

		/// \brief	Returns true if the table contains a node at the given key.
		TOML_NODISCARD
		bool contains(std::string_view key) const noexcept
		{
			return get(key) != nullptr;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets an iterator to the node at a specific key.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key	The node's key.
		///
		/// \returns	An iterator to the node at the specified key, or end().
		TOML_NODISCARD
		iterator find(std::wstring_view key)
		{
			return find(impl::narrow(key));
		}

		/// \brief	Gets an iterator to the node at a specific key (const overload).
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key	The node's key.
		///
		/// \returns	A const iterator to the node at the specified key, or cend().
		TOML_NODISCARD
		const_iterator find(std::wstring_view key) const
		{
			return find(impl::narrow(key));
		}

		/// \brief	Returns true if the table contains a node at the given key.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		bool contains(std::wstring_view key) const
		{
			return contains(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// @}

		/// \name Node views
		/// @{

		/// \brief	Gets a node_view for the selected key-value pair.
		///
		/// \param 	key The key used for the lookup.
		///
		/// \returns	A view of the value at the given key if one existed, or an empty node view.
		///
		/// \remarks std::map::operator[]'s behaviour of default-constructing a value at a key if it
		/// 		 didn't exist is a crazy bug factory so I've deliberately chosen not to emulate it.
		/// 		 <strong>This is not an error.</strong>
		///
		/// \see toml::node_view
		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		/// \brief	Gets a node_view for the selected key-value pair (const overload).
		///
		/// \param 	key The key used for the lookup.
		///
		/// \returns	A view of the value at the given key if one existed, or an empty node view.
		///
		/// \remarks std::map::operator[]'s behaviour of default-constructing a value at a key if it
		/// 		 didn't exist is a crazy bug factory so I've deliberately chosen not to emulate it.
		/// 		 <strong>This is not an error.</strong>
		///
		/// \see toml::node_view
		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Gets a node_view for the selected key-value pair.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key The key used for the lookup.
		///
		/// \returns	A view of the value at the given key if one existed, or an empty node view.
		///
		/// \remarks std::map::operator[]'s behaviour of default-constructing a value at a key if it
		/// 		 didn't exist is a crazy bug factory so I've deliberately chosen not to emulate it.
		/// 		 <strong>This is not an error.</strong>
		///
		/// \see toml::node_view
		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		/// \brief	Gets a node_view for the selected key-value pair (const overload).
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \param 	key The key used for the lookup.
		///
		/// \returns	A view of the value at the given key if one existed, or an empty node view.
		///
		/// \remarks std::map::operator[]'s behaviour of default-constructing a value at a key if it
		/// 		 didn't exist is a crazy bug factory so I've deliberately chosen not to emulate it.
		/// 		 <strong>This is not an error.</strong>
		///
		/// \see toml::node_view
		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// @}

	  private:
		/// \cond

		TOML_NODISCARD
		TOML_API
		static bool equal(const table&, const table&) noexcept;

		/// \endcond

	  public:
		/// \name Equality
		/// @{

		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS table.
		/// \param 	rhs	The RHS table.
		///
		/// \returns	True if the tables contained the same keys and map.
		TOML_NODISCARD
		friend bool operator==(const table& lhs, const table& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS table.
		/// \param 	rhs	The RHS table.
		///
		/// \returns	True if the tables did not contain the same keys and map.
		TOML_NODISCARD
		friend bool operator!=(const table& lhs, const table& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		/// @}

#if TOML_ENABLE_FORMATTERS

		/// \brief	Prints the table out to a stream as formatted TOML.
		///
		/// \availability This operator is only available when #TOML_ENABLE_FORMATTERS is enabled.
		friend std::ostream& operator<<(std::ostream& lhs, const table& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};

	//#  template <typename T>
	//#  inline std::vector<T> node::select_exact() const noexcept
	//# {
	//# 	using namespace impl;
	//#
	//# 	static_assert(
	//# 		!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
	//# 		"Retrieving values as wide-character strings with node::select_exact() is only "
	//# 		"supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled."
	//# 	);
	//#
	//# 	static_assert(
	//# 		(is_native<T> || can_represent_native<T>) && !is_cvref<T>,
	//# 		TOML_SA_VALUE_EXACT_FUNC_MESSAGE("return type of node::select_exact()")
	//# 	);
	//# }

	//#  template <typename T>
	//#  inline std::vector<T> node::select() const noexcept
	//# {
	//# 	using namespace impl;
	//#
	//# 	static_assert(
	//# 		!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
	//# 		"Retrieving values as wide-character strings with node::select() is only "
	//# 		"supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled."
	//# 	);
	//# 	static_assert(
	//# 		(is_native<T> || can_represent_native<T> || can_partially_represent_native<T>) && !is_cvref<T>,
	//# 		TOML_SA_VALUE_FUNC_MESSAGE("return type of node::select()")
	//# 	);
	//# }
}
TOML_NAMESPACE_END;

#include "header_end.h"

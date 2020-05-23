//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_array.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_VTABLE_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml::impl
{
	template <bool IsConst>
	struct table_proxy_pair final
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const string& first;
		value_type& second;
	};

	template <bool IsConst>
	class table_iterator final
	{
		private:
			friend class ::toml::table;

			using proxy_type = table_proxy_pair<IsConst>;
			using raw_mutable_iterator = string_map<std::unique_ptr<node>>::iterator;
			using raw_const_iterator = string_map<std::unique_ptr<node>>::const_iterator;
			using raw_iterator = std::conditional_t<IsConst, raw_const_iterator, raw_mutable_iterator>;
			
			mutable raw_iterator raw_;
			mutable std::aligned_storage_t<sizeof(proxy_type), alignof(proxy_type)> proxy;
			mutable bool proxy_instantiated = false;

			[[nodiscard]]
			proxy_type* get_proxy() const noexcept
			{
				if (!proxy_instantiated)
				{
					auto p = new (&proxy) proxy_type{ raw_->first, *raw_->second.get() };
					proxy_instantiated = true;
					return p;
				}
				else
					return TOML_LAUNDER(reinterpret_cast<proxy_type*>(&proxy));
			}

			table_iterator(raw_mutable_iterator raw) noexcept
				: raw_{ raw }
			{}

			template <bool C = IsConst, typename = std::enable_if_t<C>>
			TOML_NODISCARD_CTOR
			table_iterator(raw_const_iterator raw) noexcept
				: raw_{ raw }
			{}

		public:

			table_iterator() noexcept = default;

			table_iterator(const table_iterator& other) noexcept
				: raw_{ other.raw_ }
			{}

			table_iterator& operator = (const table_iterator& rhs) noexcept
			{
				raw_ = rhs.raw_;
				proxy_instantiated = false;
				return *this;
			}

			using value_type = table_proxy_pair<IsConst>;
			using reference = value_type&;
			using pointer = value_type*;

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

			[[nodiscard]]
			reference operator* () const noexcept
			{
				return *get_proxy();
			}

			[[nodiscard]]
			pointer operator -> () const noexcept
			{
				return get_proxy();
			}

			[[nodiscard]]
			friend constexpr bool operator == (const table_iterator& lhs, const table_iterator& rhs) noexcept
			{
				return lhs.raw_ == rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator != (const table_iterator& lhs, const table_iterator& rhs) noexcept
			{
				return lhs.raw_ != rhs.raw_;
			}

			template <bool C = IsConst, typename = std::enable_if_t<!C>>
			[[nodiscard]]
			operator table_iterator<true>() const noexcept
			{
				return table_iterator<true>{ raw_ };
			}
	};

	struct table_init_pair final
	{
		string key;
		std::unique_ptr<node> value;

		template <typename T>
		table_init_pair(string&& k, T && v) noexcept
			: key{ std::move(k) },
			value{ make_node(std::forward<T>(v)) }
		{}

		template <typename T>
		table_init_pair(string_view k, T&& v) noexcept
			: key{ k },
			value{ make_node(std::forward<T>(v)) }
		{}

		template <typename T>
		table_init_pair(const string_char* k, T&& v) noexcept
			: key{ k },
			value{ make_node(std::forward<T>(v)) }
		{}
	};
}

namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const table& lhs, const table& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const table& lhs, const table& rhs) noexcept;
	template <typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const table&);

	/// \brief A BidirectionalIterator for iterating over key-value pairs in a toml::table.
	using table_iterator = impl::table_iterator<false>;

	/// \brief A BidirectionalIterator for iterating over const key-value pairs in a toml::table.
	using const_table_iterator = impl::table_iterator<true>;

	/// \brief	A TOML table.
	/// 
	/// \remarks The interface of this type is modeled after std::map, with some
	/// 		additional considerations made for the heterogeneous nature of a
	/// 		TOML table, and for the removal of some cruft (the public interface of
	/// 		std::map is, simply, _a hot mess_).
	class TOML_API table final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

			table(impl::table_init_pair*, size_t) noexcept;

		public:

			/// \brief A BidirectionalIterator for iterating over key-value pairs in a toml::table.
			using iterator = table_iterator;
			/// \brief A BidirectionalIterator for iterating over const key-value pairs in a toml::table.
			using const_iterator = const_table_iterator;
			
			/// \brief	Default constructor.
			TOML_NODISCARD_CTOR
			table() noexcept;

			/// \brief	Constructs a table with one or more initial key-value pairs.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{ // double braces required :( - see remark
			///		{ "foo", 1 },
			///		{ "bar", 2.0 },
			///		{ "kek", "three" }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// { foo = 1, bar = 2.0, kek = "three" }
			/// \eout
			/// 
			/// \tparam	N	Number of key-value pairs used to initialize the table.
			/// \param 	arr	An array of key-value pairs used to initialize the table.
			/// 
			/// \remarks C++ std::initializer_lists represent their member elements as
			/// 		 const even if the list's value type is non-const. This is great for
			/// 		 compiler writers, I guess, but pretty annoying for me since
			/// 		 TOML key-value pairs are polymorphic (and thus move-only). This means
			/// 		 that for the human-friendly braced init list syntax to work I can't use
			/// 		 std::initializer_list and must instead invent an annoying proxy type,
			/// 		 which means an extra level of nesting.
			/// 		 <br><br>
			/// 		 See https://en.cppreference.com/w/cpp/utility/initializer_list
			/// 		 if you'd like to learn more about this.
			template <size_t N>
			TOML_NODISCARD_CTOR
			explicit table(impl::table_init_pair(&& arr)[N]) noexcept
				: table{ arr, N }
			{}

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			table(table&& other) noexcept;

			/// \brief	Move-assignment operator.
			table& operator= (table&& rhs) noexcept;

			table(const table&) = delete;
			table& operator= (const table&) = delete;

			/// \brief	Always returns `node_type::table` for table nodes.
			[[nodiscard]] node_type type() const noexcept override;
			/// \brief	Always returns `true` for table nodes.
			[[nodiscard]] bool is_table() const noexcept override;
			/// \brief	Always returns `false` for table nodes.
			[[nodiscard]] bool is_array() const noexcept override;
			/// \brief	Always returns `false` for table nodes.
			[[nodiscard]] bool is_value() const noexcept override;
			[[nodiscard]] table* as_table() noexcept override;
			[[nodiscard]] const table* as_table() const noexcept override;

			/// \brief	Returns true if this table is an inline table.
			/// 
			/// \remarks Runtime-constructed tables (i.e. those not created during
			/// 		 parsing) are not inline by default.
			[[nodiscard]] bool is_inline() const noexcept;

			/// \brief	Sets whether this table is a TOML inline table.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 },
			///		{ "d", toml::table{{ { "e", 4 } }} }
			///	}};
			/// std::cout << "is inline? "sv << tbl.is_inline() << std::endl;
			/// std::cout << tbl << std::endl << std::endl;
			/// 
			/// tbl.is_inline(!tbl.is_inline());
			/// std::cout << "is inline? "sv << tbl.is_inline() << std::endl;
			/// std::cout << tbl << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// is inline? false
			/// a = 1
			/// b = 2
			/// c = 3
			/// [d]
			/// e = 4
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
			void is_inline(bool val) noexcept;

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
			[[nodiscard]] node_view<node> operator[] (string_view key) noexcept;

			/// \brief	Gets a node_view for the selected key-value pair (const overload).
			[[nodiscard]] node_view<const node> operator[] (string_view key) const noexcept;

			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] iterator begin() noexcept;
			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] const_iterator begin() const noexcept;
			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] const_iterator cbegin() const noexcept;

			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] iterator end() noexcept;
			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] const_iterator end() const noexcept;
			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] const_iterator cend() const noexcept;

			/// \brief	Returns true if the table is empty.
			[[nodiscard]] bool empty() const noexcept;
			/// \brief	Returns the number of key-value pairs in the table.
			[[nodiscard]] size_t size() const noexcept;
			/// \brief	Removes all key-value pairs from the table.
			void clear() noexcept;

			/// \brief	Inserts a new value at a specific key if one did not already exist.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// for (auto k : { "a", "d" })
			/// {
			///		auto result = tbl.insert(k, 42);
			///		std::cout << "inserted with key '"sv << k << "': "sv << result.second << std::endl;
			/// }
			/// std::cout << tbl << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// { a = 1, b = 2, c = 3 }
			/// inserted with key 'a': false
			/// inserted with key 'd': true
			/// { a = 1, b = 2, c = 3, d = 42 }
			/// \eout
			/// 
			/// \tparam K		toml::string (or a type convertible to it).
			/// \tparam V		One of the TOML value types (or a type promotable to one).
			/// \param 	key		The key at which to insert the new value.
			/// \param 	val		The new value to insert.
			/// 
			/// \returns A std::pair containing:
			/// 		- An iterator to the insertion position (or the position of the value that prevented insertion)  
			/// 		- A boolean indicating if the insertion was successful.
			template <typename K, typename V, typename = std::enable_if_t<
				std::is_convertible_v<K&&, string_view>
			>>
			std::pair<iterator, bool> insert(K&& key, V&& val) noexcept
			{
				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(ipos, std::forward<K>(key), impl::make_node(std::forward<V>(val)));
					return { ipos, true };
				}
				return { ipos, false };
			}

			/// \brief	Inserts a series of key-value pairs into the table.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// auto kvps = std::array<std::pair<toml::string, int>>{{
			///		{ "d", 42 },
			///		{ "a", 43 }
			///	}};
			///	tbl.insert(kvps.begin(), kvps.end());
			/// std::cout << tbl << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// { a = 1, b = 2, c = 3 }
			/// { a = 1, b = 2, c = 3, d = 42 }	//"a" already existed
			/// \eout
			/// 
			/// \tparam Iter	An InputIterator to a collection of key-value pairs.
			/// \param 	first	An iterator to the first value in the input collection.
			/// \param 	last	An iterator to one-past-the-last value in the input collection.
			/// 
			/// \remarks This function is morally equivalent to calling `insert(key, value)` for each
			/// 		 key-value pair covered by the iterator range, so any values with keys already found in the
			/// 		 table will not be replaced.
			template <typename Iter, typename = std::enable_if_t<
				!std::is_convertible_v<Iter&&, string_view>
			>>
			void insert(Iter first, Iter last) noexcept
			{
				if (first == last)
					return;
				for (auto it = first; it != last; it++)
				{
					if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
						insert(std::move((*it).first), std::move((*it).second));
					else
						insert((*it).first, (*it).second);
				}
			}

			/// \brief	Inserts or assigns a value at a specific key.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// for (auto k : { "a", "d" })
			/// {
			///		auto result = tbl.insert_or_assign(k, 42);
			///		std::cout << "value at key '"sv << k
			///			<< "' was "sv << (result.second ? "inserted"sv : "assigned"sv) << std::endl;
			/// }
			/// std::cout << tbl << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// { a = 1, b = 2, c = 3 }
			/// value at key 'a' was assigned
			/// value at key 'd' was inserted
			/// { a = 42, b = 2, c = 3, d = 42 }
			/// \eout
			/// 
			/// \tparam K		toml::string (or a type convertible to it).
			/// \tparam V		One of the TOML value types (or a type promotable to one).
			/// \param 	key		The key at which to insert or assign the value.
			/// \param 	val		The value to insert/assign.
			/// 
			/// \returns A std::pair containing:
			/// 		- An iterator to the value's position
			/// 		- A boolean containing `true` if the value was inserted, `false` if it was assigned.
			template <typename K, typename V>
			std::pair<iterator, bool> insert_or_assign(K&& key, V&& val) noexcept
			{
				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(ipos, std::forward<K>(key), impl::make_node(std::forward<V>(val)));
					return { ipos, true };
				}
				else
				{
					(*ipos).second.reset(impl::make_node(std::forward<V>(val)));
					return { ipos, false };
				}
			}

			/// \brief	Emplaces a new value at a specific key if one did not already exist.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// for (auto k : { "a", "d" })
			/// {
			///		// add a string using std::string's substring constructor
			///		auto result = tbl.emplace<std::string>(k, "this is not a drill"sv, 14, 5);
			///		std::cout << "emplaced with key '"sv << k << "': "sv << result.second << std::endl;
			/// }
			/// std::cout << tbl << std::endl;
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
			/// \tparam	U		One of the TOML node or value types.
			/// \tparam K		toml::string (or a type convertible to it).
			/// \tparam V		Value constructor argument types.
			/// \param 	key		The key at which to emplace the new value.
			/// \param 	args	Arguments to forward to the value's constructor.
			/// 
			/// \returns A std::pair containing:
			/// 		- An iterator to the emplacement position (or the position of the value that prevented emplacement)
			/// 		- A boolean indicating if the emplacement was successful.  
			/// 
			/// \remark There is no difference between insert() and emplace() for trivial value types (floats, ints, bools).
			template <typename U, typename K, typename... V>
			std::pair<iterator, bool> emplace(K&& key, V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(
						ipos,
						std::forward<K>(key),
						new impl::node_of<type>{ std::forward<V>(args)... }
					);
					return { ipos, true };
				}
				return { ipos, false };
			}

			/// \brief	Removes the specified key-value pair from the table.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// tbl.erase(tbl.begin() + 1);
			/// std::cout << tbl << std::endl;
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
			iterator erase(iterator pos) noexcept;

			/// \brief	Removes the specified key-value pair from the table (const iterator overload).
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// tbl.erase(tbl.cbegin() + 1);
			/// std::cout << tbl << std::endl;
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
			iterator erase(const_iterator pos) noexcept;

			/// \brief	Removes the key-value pairs in the range [first, last) from the table.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", "bad" },
			///		{ "c", "karma" },
			///		{ "d", 2 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// tbl.erase(tbl.cbegin() + 1, tbl.cbegin() + 3);
			/// std::cout << tbl << std::endl;
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
			iterator erase(const_iterator first, const_iterator last) noexcept;

			/// \brief	Removes the value with the given key from the table.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 1 },
			///		{ "b", 2 },
			///		{ "c", 3 }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// std::cout << tbl.erase("b") << std::endl;
			/// std::cout << tbl.erase("not an existing key") << std::endl;
			/// std::cout << tbl << std::endl;
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
			bool erase(string_view key) noexcept;

		private:

			template <typename Map, typename Key>
			[[nodiscard]] static auto do_get(Map& vals, const Key& key) noexcept
			{
				using return_type = std::conditional_t<
					std::is_const_v<Map>,
					const node*,
					node*
				>;

				if (auto it = vals.find(key); it != vals.end())
					return return_type{ it->second.get() };
				return return_type{};
			}

			template <typename T, typename Map, typename Key>
			[[nodiscard]] static auto do_get_as(Map& vals, const Key& key) noexcept
			{
				const auto node = do_get(vals, key);
				return node ? node->template as<T>() : nullptr;
			}

			template <typename Map, typename Key>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static bool do_contains(Map& vals, const Key& key) noexcept
			{
				#if TOML_CPP >= 20
					return vals.contains(key);
				#else
					return do_get(vals, key) != nullptr;
				#endif
			}

		public:

			/// \brief	Gets the node at a specific key.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 42, },
			///		{ "b", "is the meaning of life, apparently." }
			///	}};
			///	std::cout << R"(node ["a"] exists: )"sv << !!arr.get("a") << std::endl;
			///	std::cout << R"(node ["b"] exists: )"sv << !!arr.get("b") << std::endl;
			///	std::cout << R"(node ["c"] exists: )"sv << !!arr.get("c") << std::endl;
			/// if (auto val = arr.get("a"))
			///		std::cout << R"(node ["a"] was an )"sv << val->type() << std::endl;
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
			[[nodiscard]] node* get(string_view key) noexcept;

			/// \brief	Gets the node at a specific key (const overload).
			///
			/// \param 	key	The node's key.
			///
			/// \returns	A pointer to the node at the specified key, or nullptr.
			[[nodiscard]] const node* get(string_view key) const noexcept;

			[[nodiscard]] iterator find(string_view key) noexcept;
			[[nodiscard]] const_iterator find(string_view key) const noexcept;

			/// \brief	Gets the node at a specific key if it is a particular type.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 42, },
			///		{ "b", "is the meaning of life, apparently." }
			///	}};
			/// if (auto val = arr.get_as<int64_t>("a"))
			///		std::cout << R"(node ["a"] was an integer with value )"sv << **val << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// node ["a"] was an integer with value 42
			/// \eout
			/// 
			/// \tparam	T	The node's type.
			/// \param 	key	The node's key.
			///
			/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
			template <typename T>
			[[nodiscard]] impl::node_of<T>* get_as(string_view key) noexcept
			{
				return do_get_as<T>(values, key);
			}

			/// \brief	Gets the node at a specific key if it is a particular type (const overload).
			///
			/// \tparam	T	The node's type.
			/// \param 	key	The node's key.
			///
			/// \returns	A pointer to the node at the specified key if it was of the given type, or nullptr.
			template <typename T>
			[[nodiscard]] const impl::node_of<T>* get_as(string_view key) const noexcept
			{
				return do_get_as<T>(values, key);
			}

			/// \brief	Returns true if the table contains a node at the given key.
			[[nodiscard]] bool contains(string_view key) const noexcept;

			/// \brief	Equality operator.
			///
			/// \param 	lhs	The LHS table.
			/// \param 	rhs	The RHS table.
			///
			/// \returns	True if the tables contained the same keys and values.
			friend bool operator == (const table& lhs, const table& rhs) noexcept;

			/// \brief	Inequality operator.
			///
			/// \param 	lhs	The LHS table.
			/// \param 	rhs	The RHS table.
			///
			/// \returns	True if the tables did not contain the same keys and values.
			friend bool operator != (const table& lhs, const table& rhs) noexcept;

			template <typename Char>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const table&);
	};
}

TOML_POP_WARNINGS // TOML_DISABLE_VTABLE_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

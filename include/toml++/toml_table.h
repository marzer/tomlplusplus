#pragma once
#include "toml_array.h"

namespace toml::impl
{
	template <bool is_const>
	struct table_proxy_pair final
	{
		using value_type = std::conditional_t<is_const, const node, node>;

		const string& key;
		value_type& value;
	};

	template <bool is_const>
	class table_iterator final
	{
		private:
			friend class toml::table;

			using raw_iterator = std::conditional_t<
				is_const,
				string_map<std::unique_ptr<node>>::const_iterator,
				string_map<std::unique_ptr<node>>::iterator
			>;
				
			mutable raw_iterator raw_;

			table_iterator(const raw_iterator& raw) noexcept
				: raw_{ raw }
			{}

			table_iterator(raw_iterator&& raw) noexcept
				: raw_{ std::move(raw) }
			{}

		public:

			table_iterator() noexcept = default;

			using reference = table_proxy_pair<is_const>;
			using difference_type = ptrdiff_t;

			table_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				return *this;
			}

			table_iterator operator++(int) noexcept // post++
			{
				table_iterator out{ raw_ };
				++raw_;
				return out;
			}

			table_iterator& operator--() noexcept // --pre
			{
				--raw_;
				return *this;
			}

			table_iterator operator--(int) noexcept // post--
			{
				table_iterator out{ raw_ };
				--raw_;
				return out;
			}

			reference operator* () const noexcept
			{
				return { raw_->first, *raw_->second.get() };
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
	/// \brief	A TOML table.
	/// 
	/// \remarks The interface of this type is modeled after std::map, with some
	/// 		additional considerations made for the heterogeneous nature of a
	/// 		TOML table, and for the removal of some cruft (the public interface of
	/// 		std::map is, simply, _a hot mess_).
	/// 		
	/// \detail \cpp
	/// // example
	/// // code
	/// // here
	/// \ecpp
	class table final
		: public node
	{
		private:
			friend class impl::parser;
			friend class node_view<table>;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

		public:

			/// \brief A BidirectionalIterator for iterating over the key-value pairs in a table.
			using iterator = impl::table_iterator<false>;
			/// \brief A const BidirectionalIterator for iterating over the key-value pairs in a table.
			using const_iterator = impl::table_iterator<true>;
			
			/// \brief	Default constructor.
			TOML_NODISCARD_CTOR
			table() noexcept {}

			/// \brief	Constructs a table with one or more initial key-value pairs.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{ //double braces required :( - see Remarks
			///		{ "foo", 1 },
			///		{ "bar", 2.0 },
			///		{ "kek", "three" }
			///	}};
			/// std::cout << tbl << std::endl;
			/// 
			/// // output: 
			/// // { foo = 1, bar = 2.0, kek = "three" }
			/// \ecpp
			/// 
			/// \tparam	N	Number of key-value pairs used to initialize the table.
			/// \param 	arr	An array of key-value pairs used to initialize the table.
			/// 
			/// \remarks C++ std::initializer_lists represent their member elements as
			/// 		 const even if the list's value type is non-const. This is great for
			/// 		 compiler writers, I guess, but pretty annoying for me since
			/// 		 TOML key-value pairs are polymorphic. This means that for the
			/// 		 human-friendly braced init list syntax to work I can't use
			/// 		 std::initializer_list and must instead invent an annoying proxy type,
			/// 		 which means an extra level of nesting.
			/// 		 <br><br>
			/// 		 See https://en.cppreference.com/w/cpp/utility/initializer_list
			/// 		 if you'd like to be more annoyed about this.
			template <size_t N>
			TOML_NODISCARD_CTOR
			explicit table(impl::table_init_pair(&& arr)[N]) noexcept
			{
				for (auto&& kvp : arr)
				{
					values.insert_or_assign(
						std::move(kvp.key),
						std::move(kvp.value)
					);
				}
			}

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			table(table&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) },
				inline_ { other.inline_ }
			{}

			/// \brief	Move-assignment operator.
			table& operator= (table&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				inline_ = rhs.inline_;
				return *this;
			}

			/// \brief	Always returns `node_type::table` for table nodes.
			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }
			/// \brief	Always returns `true` for table nodes.
			[[nodiscard]] bool is_table() const noexcept override { return true; }
			/// \brief	Always returns `false` for table nodes.
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			/// \brief	Always returns `false` for table nodes.
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }


			/// \brief	Returns true if this table is an inline table.
			/// 
			/// \remarks Runtime-constructed tables (i.e. those not created during
			/// 		 parsing) are not inline by default.
			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

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
			/// // example output:
			/// // is inline? false
			/// // a = 1
			/// // b = 2
			/// // c = 3
			/// // [d]
			/// // e = 4
			/// //
			/// // is inline? true
			/// // { a = 1, b = 2, c = 3, d = { e = 4 } }
			/// \ecpp
			/// 
			/// \remarks A table being 'inline' is only relevent during printing;
			/// 		 it has no effect on the general functionality of the table
			/// 		 object.
			/// 
			/// \param 	val	The new value for 'inline'.
			void is_inline(bool val) noexcept { inline_ = val; }

			/// \brief	Gets a node_view for the selected key-value pair.
			///
			/// \param 	key The key used for the lookup.
			///
			/// \returns	A node_view.
			///
			/// \remarks std::map::operator[]'s behaviour of constructing an element at a key if it
			/// 		 didn't exist is terribad, so I've deliberately chosen not to emulate that
			/// 		 insane bug-factory. This Is Not An Error (tm).
			/// 
			/// \see toml::node_view
			[[nodiscard]] inline node_view<table> operator[] (string_view key) noexcept;

			/// \brief	Gets a node_view for the selected key-value pair (const overload).
			[[nodiscard]] inline node_view<const table> operator[] (string_view key) const noexcept;

			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			/// \brief	Returns an iterator to the first key-value pair.
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }

			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() }; }
			/// \brief	Returns an iterator to one-past-the-last key-value pair.
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }

			/// \brief	Returns true if the table is empty.
			[[nodiscard]] bool empty() const noexcept { return values.empty(); }
			/// \brief	Returns the number of key-value pairs in the table.
			[[nodiscard]] size_t size() const noexcept { return values.size(); }
			/// \brief	Removes all key-value pairs from the table.
			void clear() noexcept { values.clear(); }

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

			template <typename ITER, typename = std::enable_if_t<
				!std::is_convertible_v<ITER&&, string_view>
			>>
			void insert(ITER first, ITER last) noexcept
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
						new node_of<type>{ std::forward<V>(args)... }
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
			/// // output: 
			/// // { a = 1, b = 2, c = 3 }
			/// // { a = 1, c = 3 }
			/// \ecpp
			/// 
			/// \param 	pos		Iterator to the key-value pair being erased.
			/// 
			/// \returns Iterator to the first key-value pair immediately following the removed key-value pair.
			iterator erase(iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}

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
			/// // output: 
			/// // { a = 1, b = 2, c = 3 }
			/// // { a = 1, c = 3 }
			/// \ecpp
			/// 
			/// \param 	pos		Iterator to the key-value pair being erased.
			/// 
			/// \returns Iterator to the first key-value pair immediately following the removed key-value pair.
			iterator erase(const_iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}

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
			/// // output: 
			/// // { a = 1, b = "bad", c = "karma", d = 2 }
			/// // { a = 1, d = 2 }
			/// \ecpp
			/// 
			/// \param 	first	Iterator to the first key-value pair being erased.
			/// \param 	last	Iterator to the one-past-the-last key-value pair being erased.
			/// 
			/// \returns Iterator to the first key-value pair immediately following the last removed key-value pair.
			iterator erase(const_iterator first, const_iterator last) noexcept
			{
				return { values.erase(first.raw_, last.raw_) };
			}

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
			/// // output: 
			/// // { a = 1, b = 2, c = 3 }
			/// // true
			/// // false
			/// // { a = 1, c = 3 }
			/// \ecpp
			/// 
			/// \param 	key		Key to erase.
			/// 
			/// \returns True if any values with matching keys were found and erased.
			bool erase(string_view key) noexcept
			{
				if (auto it = values.find(key); it != values.end())
				{
					values.erase(it);
					return true;
				}
				return false;
			}

		private:

			template <typename MAP, typename KEY>
			[[nodiscard]] static auto do_get(MAP& vals, const KEY& key) noexcept
			{
				using return_type = std::conditional_t<
					std::is_const_v<MAP>,
					const node*,
					node*
				>;

				if (auto it = vals.find(key); it != vals.end())
					return return_type{ it->second.get() };
				return return_type{};
			}

			template <typename T, typename MAP, typename KEY>
			[[nodiscard]] static auto do_get_as(MAP& vals, const KEY& key) noexcept
			{
				const auto node = do_get(vals, key);
				return node ? node->template as<T>() : nullptr;
			}

			template <typename MAP, typename KEY>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static bool do_contains(MAP& vals, const KEY& key) noexcept
			{
				#if TOML_CPP >= 20
				return vals.contains(key);
				#else
				return do_get(vals, key) != nullptr;
				#endif
			}

		public:

			[[nodiscard]] node* get(string_view key) noexcept { return do_get(values, key); }
			[[nodiscard]] const node* get(string_view key) const noexcept { return do_get(values, key); }

			[[nodiscard]] iterator find(string_view key) noexcept { return { values.find(key) }; }
			[[nodiscard]] const_iterator find(string_view key) const noexcept { return { values.find(key) }; }

			/// \brief	Gets the node at a specific key if it is a particular type.
			///
			/// \detail \cpp
			/// auto tbl = toml::table{{
			///		{ "a", 42, },
			///		{ "b", "is the meaning of life, apparently." }
			///	}};
			/// if (auto val = arr.get_as<int64_t>("a"))
			///		std::cout << node [\"a\"] was an integer with value "sv << **val << std::endl;
			/// 
			/// // output: 
			/// // node ["a"] was an integer with value 42
			/// \ecpp
			/// 
			/// \tparam	T	The node's type.
			/// \param 	key	The node's key.
			///
			/// \returns	A pointer to the selected node if it was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] node_of<T>* get_as(string_view key) noexcept { return do_get_as<T>(values, key); }

			/// \brief	Gets the node at a specific key if it is a particular type (const overload).
			///
			/// \tparam	T	The node's type.
			/// \param 	key	The node's key.
			///
			/// \returns	A pointer to the selected node if it was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(string_view key) const noexcept { return do_get_as<T>(values, key); }

			/// \brief	Returns true if the table contains a node at the given key.
			[[nodiscard]] bool contains(string_view key) const noexcept { return do_contains(values, key); }

			/// \brief	Equality operator.
			///
			/// \param 	lhs	The LHS table.
			/// \param 	rhs	The RHS table.
			///
			/// \returns	True if the tables contained the same keys and values.
			[[nodiscard]] friend bool operator == (const table& lhs, const table& rhs) noexcept
			{
				if (&lhs == &rhs)
					return true;
				if (lhs.values.size() != rhs.values.size())
					return false;

				for (auto l = lhs.values.begin(), r = rhs.values.begin(), e = lhs.values.end(); l != e; l++, r++)
				{
					if (l->first != r->first)
						return false;

					const auto lhs_type = l->second->type();
					const node& rhs_ = *r->second;
					const auto rhs_type = rhs_.type();
					if (lhs_type != rhs_type)
						return false;

					const bool equal = l->second->visit([&](const auto& lhs_) noexcept
					{
						return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_);
					});
					if (!equal)
						return false;
				}
				return true;
			}

			/// \brief	Inequality operator.
			///
			/// \param 	lhs	The LHS table.
			/// \param 	rhs	The RHS table.
			///
			/// \returns	True if the tables did not contain the same keys and values.
			[[nodiscard]] friend bool operator != (const table& lhs, const table& rhs) noexcept
			{
				return !(lhs == rhs);
			}

			template <typename CHAR>
			friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const table&) TOML_MAY_THROW;
	};
}

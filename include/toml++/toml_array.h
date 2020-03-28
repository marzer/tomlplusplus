//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_value.h"

namespace toml::impl
{
	template <bool is_const>
	class array_iterator final
	{
		private:
			friend class toml::array;

			using raw_iterator = std::conditional_t<
				is_const,
				std::vector<std::unique_ptr<node>>::const_iterator,
				std::vector<std::unique_ptr<node>>::iterator
			>;
				
			mutable raw_iterator raw_;

			array_iterator(const raw_iterator& raw) noexcept
				: raw_{ raw }
			{}

			array_iterator(raw_iterator&& raw) noexcept
				: raw_{ std::move(raw) }
			{}

		public:

			using value_type = std::conditional_t<is_const, const node, node>;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = ptrdiff_t;

			array_iterator() noexcept = default;

			array_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				return *this;
			}

			array_iterator operator++(int) noexcept // post++
			{
				array_iterator out{ raw_ };
				++raw_;
				return out;
			}

			array_iterator& operator--() noexcept // --pre
			{
				--raw_;
				return *this;
			}

			array_iterator operator--(int) noexcept // post--
			{
				array_iterator out{ raw_ };
				--raw_;
				return out;
			}

			[[nodiscard]]
			reference operator * () const noexcept
			{
				return *raw_->get();
			}

			[[nodiscard]]
			pointer operator -> () const noexcept
			{
				return raw_->get();
			}

			array_iterator& operator += (ptrdiff_t rhs) noexcept
			{
				raw_ += rhs;
				return *this;
			}

			array_iterator& operator -= (ptrdiff_t rhs) noexcept
			{
				raw_ -= rhs;
				return *this;
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ + rhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (ptrdiff_t lhs, const array_iterator& rhs) noexcept
			{
				return { rhs.raw_ + lhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator - (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ - rhs };
			}

			[[nodiscard]]
			friend constexpr ptrdiff_t operator - (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ - rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator == (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ == rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator != (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ != rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator < (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ < rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator <= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ <= rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator > (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ > rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator >= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ >= rhs.raw_;
			}

			[[nodiscard]]
			reference operator[] (ptrdiff_t idx) const noexcept
			{
				return *(raw_ + idx)->get();
			}
	};

	template class array_iterator<true>;
	template class array_iterator<false>;

	template <typename T>
	[[nodiscard]] TOML_ALWAYS_INLINE
	auto make_node(T&& val) noexcept
	{
		using type = impl::unwrapped<remove_cvref_t<T>>;
		if constexpr (is_one_of<type, array, table>)
		{
			static_assert(
				std::is_rvalue_reference_v<decltype(val)>,
				"Tables and arrays may only be moved (not copied)."
			);
			return new type{ std::forward<T>(val) };
		}
		else
		{
			static_assert(
				is_value_or_promotable<type>,
				"Value initializers must be (or be promotable to) one of the TOML value types"
			);
			return new value{ std::forward<T>(val) };
		}
	}
}


namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const array& lhs, const array& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const array& lhs, const array& rhs) noexcept;
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&);

	/// \brief	A TOML array.
	///
	/// \detail The interface of this type is modeled after std::vector, with some
	/// 		additional considerations made for the heterogeneous nature of a
	/// 		TOML array. \cpp
	/// 
	/// auto tbl = toml::parse("arr = [1, 2, 3, 4, 'five']"sv);
	/// auto& arr = *tbl.get_as<toml::array>("arr");
	/// std::cout << arr << std::endl;
	/// 
	/// for (size_t i = 0; i < arr.size(); i++)
	/// {
	///		arr[i].visit([](auto&& el) noexcept
	///		{
	///			if constexpr (toml::is_number<decltype(el)>)
	///				(*el)++;
	///			else if constexpr (toml::is_string<decltype(el)>)
	///				el = "six"sv;
	///		});
	/// }
	/// std::cout << arr << std::endl;
	/// 
	/// arr.push_back(7);
	/// arr.push_back(8.0f);
	/// arr.push_back("nine"sv);
	/// arr.erase(arr.cbegin());
	/// std::cout << arr << std::endl;
	/// 
	/// arr.emplace_back<toml::array>(10, 11.0);
	/// std::cout << arr << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// [1, 2, 3, 4, "five"]
	/// [2, 3, 4, 5, "six"]
	/// [3, 4, 5, "six", 7, 8.0, "nine"]
	/// [3, 4, 5, "six", 7, 8.0, "nine", [10, 11.0]]
	/// \eout
	class TOML_API array final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;
			std::vector<std::unique_ptr<node>> values;

			void preinsertion_resize(size_t idx, size_t count) noexcept;

		public:

			using value_type = node;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using reference = node&;
			using const_reference = const node&;

			/// \brief A RandomAccessIterator for iterating over the nodes in an array.
			using iterator = impl::array_iterator<false>;
			/// \brief A const RandomAccessIterator for iterating over the nodes in an array.
			using const_iterator = impl::array_iterator<true>;

			/// \brief	Default constructor.
			TOML_NODISCARD_CTOR
			array() noexcept;

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			array(array&& other) noexcept;

			/// \brief	Constructs an array with one or more initial values.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2.0, "three"sv, toml::array{ 4, 5 } };
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, 2.0, "three", [4, 5]]
			/// \eout
			/// 	 
			/// \tparam	U		One of the TOML node or value types (or a type promotable to one).
			/// \tparam	V		One of the TOML node or value types (or a type promotable to one).
			/// \param 	val		The value used to initialize node 0.
			/// \param 	vals	The values used to initialize nodes 1...N.
			template <typename U, typename... V>
			TOML_NODISCARD_CTOR
			explicit array(U&& val, V&&... vals)
			{
				values.reserve(sizeof...(V) + 1_sz);
				values.emplace_back(impl::make_node(std::forward<U>(val)));
				if constexpr (sizeof...(V) > 0)
				{
					(
						values.emplace_back(impl::make_node(std::forward<V>(vals))),
						...
					);
				}
			}

			/// \brief	Move-assignment operator.
			array& operator= (array&& rhs) noexcept;

			array(const array&) = delete;
			array& operator= (const array&) = delete;

			/// \brief	Always returns node_type::array for array nodes.
			[[nodiscard]] node_type type() const noexcept override;
			/// \brief	Always returns `false` for array nodes.
			[[nodiscard]] bool is_table() const noexcept override;
			/// \brief	Always returns `true` for array nodes.
			[[nodiscard]] bool is_array() const noexcept override;
			/// \brief	Always returns `false` for array nodes.
			[[nodiscard]] bool is_value() const noexcept override;
			[[nodiscard]] array* as_array() noexcept override;
			[[nodiscard]] const array* as_array() const noexcept override;

			/// \brief	Checks if the array contains nodes of only one type.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << "homogenous: "sv << arr.is_homogeneous() << std::endl;
			/// std::cout << "all doubles: "sv << arr.is_homogeneous<double>() << std::endl;
			/// std::cout << "all arrays: "sv << arr.is_homogeneous<toml::array>() << std::endl;
			/// std::cout << "all integers: "sv << arr.is_homogeneous<int64_t>() << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// homogeneous: true
			/// all doubles: false
			/// all arrays: false
			/// all integers: true
			/// \eout
			/// 
			/// \tparam	T	A TOML node type. <br>
			/// 			<strong><em>Explicitly specified:</em></strong> "is every node a T?" <br>
			/// 			<strong><em>Left as `void`:</em></strong> "is every node the same type?"
			///
			/// \returns	True if the array was homogeneous.
			/// 
			/// \remarks	Always returns `false` for empty arrays.
			template <typename T = void>
			[[nodiscard]] bool is_homogeneous() const noexcept
			{
				if (values.empty())
					return false;

				if constexpr (std::is_same_v<T, void>)
				{
					const auto type = values[0]->type();
					for (size_t i = 1; i < values.size(); i++)
						if (values[i]->type() != type)
							return false;
				}
				else
				{
					for (auto& v : values)
						if (!v->is<T>())
							return false;
				}
				return true;
			}

			/// \brief	Returns true if this array contains only tables.
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is_array_of_tables() const noexcept override
			{
				return is_homogeneous<toml::table>();
			}

			/// \brief	Gets a reference to the node at a specific index.
			[[nodiscard]] node& operator[] (size_t index) noexcept;
			/// \brief	Gets a reference to the node at a specific index.
			[[nodiscard]] const node& operator[] (size_t index) const noexcept;

			/// \brief	Returns a reference to the first node in the array.
			[[nodiscard]] node& front() noexcept;
			/// \brief	Returns a reference to the first node in the array.
			[[nodiscard]] const node& front() const noexcept;
			/// \brief	Returns a reference to the last node in the array.
			[[nodiscard]] node& back() noexcept;
			/// \brief	Returns a reference to the last node in the array.
			[[nodiscard]] const node& back() const noexcept;

			/// \brief	Returns an iterator to the first node.
			[[nodiscard]] iterator begin() noexcept;
			/// \brief	Returns an iterator to the first node.
			[[nodiscard]] const_iterator begin() const noexcept;
			/// \brief	Returns an iterator to the first node.
			[[nodiscard]] const_iterator cbegin() const noexcept;

			/// \brief	Returns an iterator to one-past-the-last node.
			[[nodiscard]] iterator end() noexcept;
			/// \brief	Returns an iterator to one-past-the-last node.
			[[nodiscard]] const_iterator end() const noexcept;
			/// \brief	Returns an iterator to one-past-the-last node.
			[[nodiscard]] const_iterator cend() const noexcept;

			/// \brief	Returns true if the array is empty.
			[[nodiscard]] bool empty() const noexcept;
			/// \brief	Returns the number of nodes in the array.
			[[nodiscard]] size_t size() const noexcept;
			/// \brief	Reserves internal storage capacity up to a pre-determined number of nodes.
			void reserve(size_t new_capacity);
			/// \brief	Removes all nodes from the array.
			void clear() noexcept;

			/// \brief	Inserts a new node at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 3 };
			///	arr.insert(arr.cbegin() + 1, "two");
			///	arr.insert(arr.cend(), toml::array{ 4, 5 });
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, "two", 3, [4, 5]]
			/// \eout
			/// 
			/// \tparam	U		One of the TOML node or value types (or a type promotable to one).
			/// \param 	pos		The insertion position.
			/// \param 	val		The value being inserted.
			///
			/// \returns	An iterator to the inserted value.
			template <typename U>
			iterator insert(const_iterator pos, U&& val) noexcept
			{
				return { values.emplace(pos.raw_, impl::make_node(std::forward<U>(val))) };
			}

			/// \brief	Repeatedly inserts a value starting at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{
			///		"with an evil twinkle in its eye the goose said",
			///		"and immediately we knew peace was never an option."
			///	};
			///	arr.insert(arr.cbegin() + 1, 3, "honk");
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [
			/// 	"with an evil twinkle in its eye the goose said",
			/// 	"honk",
			/// 	"honk",
			/// 	"honk",
			/// 	"and immediately we knew peace was never an option."
			/// ]
			/// \eout
			/// 
			/// \tparam	U		One of the TOML value types (or a type promotable to one).
			/// \param 	pos		The insertion position.
			/// \param 	count	The number of times the value should be inserted.
			/// \param 	val		The value being inserted.
			///
			/// \returns	An iterator to the first inserted value (or a copy of `pos` if count was 0).
			template <typename U>
			iterator insert(const_iterator pos, size_t count, U&& val) noexcept
			{
				switch (count)
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, std::forward<U>(val));
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, count);
						size_t i = start_idx;
						for (size_t e = start_idx + count - 1_sz; i < e; i++)
							values[i].reset(impl::make_node(val));

						//# potentially move the initial value into the last element
						values[i].reset(impl::make_node(std::forward<U>(val)));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			/// \brief	Inserts a range of values into the array at a specific position.
			///
			/// \tparam	ITER	An iterator type. Must satisfy ForwardIterator.
			/// \param 	pos		The insertion position.
			/// \param 	first	Iterator to the first value being inserted.
			/// \param 	last	Iterator to the one-past-the-last value being inserted.
			///
			/// \returns	An iterator to the first inserted value (or a copy of `pos` if `first` == `last`).
			template <typename ITER>
			iterator insert(const_iterator pos, ITER first, ITER last) noexcept
			{
				const auto count = std::distance(first, last);
				switch (count)
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, *first);
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, count);
						size_t i = start_idx;
						for (auto it = first; it != last; it++)
							values[i].reset(impl::make_node(*it));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			/// \brief	Inserts a range of values into the array at a specific position.
			///
			/// \tparam	U		One of the TOML node or value types (or a type promotable to one).
			/// \param 	pos		The insertion position.
			/// \param 	ilist	An initializer list containing the values to be inserted.
			///
			/// \returns	An iterator to the first inserted value (or a copy of `pos` if `ilist` was empty).
			template <typename U>
			iterator insert(const_iterator pos, std::initializer_list<U> ilist) noexcept
			{
				switch (ilist.size())
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, *ilist.begin());
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, ilist.size());
						size_t i = start_idx;
						for (auto& val : ilist)
							values[i].reset(impl::make_node(val));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			/// \brief	Emplaces a new value at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	
			///	//add a string using std::string's substring constructor
			///	arr.emplace<std::string>(arr.cbegin() + 1, "this is not a drill"sv, 14, 5);
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, "drill" 2]
			/// \eout
			/// 
			/// \tparam	U		One of the TOML node or value types.
			/// \tparam	V		Value constructor argument types.
			/// \param 	pos		The insertion position.
			/// \param 	args	Arguments to forward to the value's constructor.
			///
			/// \returns	An iterator to the inserted value.
			/// 
			/// \remarks There is no difference between insert() and emplace()
			/// 		 for trivial value types (floats, ints, bools).
			template <typename U, typename... V>
			iterator emplace(const_iterator pos, V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return { values.emplace(pos.raw_, new impl::node_of<type>{ std::forward<V>(args)...} ) };
			}

			/// \brief	Removes the specified node from the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << arr << std::endl;
			/// 
			/// arr.erase(arr.cbegin() + 1);
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, 2, 3]
			/// [1, 3]
			/// \eout
			/// 
			/// \param 	pos		Iterator to the node being erased.
			/// 
			/// \returns Iterator to the first node immediately following the removed node.
			iterator erase(const_iterator pos) noexcept;

			/// \brief	Removes the nodes in the range [first, last) from the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, "bad", "karma" 2 };
			/// std::cout << arr << std::endl;
			/// 
			/// arr.erase(arr.cbegin() + 1, arr.cbegin() + 3);
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, "bad", "karma", 3]
			/// [1, 3]
			/// \eout
			/// 
			/// \param 	first	Iterator to the first node being erased.
			/// \param 	last	Iterator to the one-past-the-last node being erased.
			/// 
			/// \returns Iterator to the first node immediately following the last removed node.
			iterator erase(const_iterator first, const_iterator last) noexcept;

			/// \brief	Appends a new value to the end of the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	arr.push_back(3);
			///	arr.push_back(4.0);
			///	arr.push_back(toml::array{ 5, "six"sv });
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, 2, 3, 4.0, [5, "six"]]
			/// \eout
			/// 
			/// \tparam	U		One of the TOML value types (or a type promotable to one).
			/// \param 	val		The value being added.
			/// 
			/// \returns A reference to the newly-constructed value node.
			template <typename U>
			decltype(auto) push_back(U&& val) noexcept
			{
				auto nde = impl::make_node(std::forward<U>(val));
				values.emplace_back(nde);
				return *nde;
			}

			/// \brief	Emplaces a new value at the end of the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	arr.emplace_back<toml::array>(3, "four"sv);
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, 2, [3, "four"]]
			/// \eout
			/// 
			/// \tparam	U		One of the TOML value types.
			/// \tparam	V		Value constructor argument types.
			/// \param 	args	Arguments to forward to the value's constructor.
			/// 
			/// \returns A reference to the newly-constructed value node.
			///
			/// \remarks There is no difference between push_back and emplace_back
			/// 		 For trivial value types (floats, ints, bools).
			template <typename U, typename... V>
			decltype(auto) emplace_back(V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				auto nde = new impl::node_of<type>{ std::forward<V>(args)... };
				values.emplace_back(nde);
				return *nde;
			}

			/// \brief	Removes the last node from the array.
			void pop_back() noexcept;

			/// \brief	Gets the node at a specific index.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 99, "bottles of beer on the wall" };
			///	std::cout << "node [0] exists: "sv << !!arr.get(0) << std::endl;
			///	std::cout << "node [1] exists: "sv << !!arr.get(1) << std::endl;
			///	std::cout << "node [2] exists: "sv << !!arr.get(2) << std::endl;
			/// if (auto val = arr.get(0))
			///		std::cout << "node [0] was an "sv << val->type() << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// node [0] exists: true
			/// node [1] exists: true
			/// node [2] exists: false
			/// node [0] was an integer
			/// \eout
			/// 
			/// \param 	index	The node's index.
			///
			/// \returns	A pointer to the node at the specified index if one existed, or nullptr.
			[[nodiscard]] node* get(size_t index) noexcept;

			/// \brief	Gets the node at a specific index (const overload).
			///
			/// \param 	index	The node's index.
			///
			/// \returns	A pointer to the node at the specified index if one existed, or nullptr.
			[[nodiscard]] const node* get(size_t index) const noexcept;

			/// \brief	Gets the node at a specific index if it is a particular type.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 42, "is the meaning of life, apparently."sv };
			/// if (auto val = arr.get_as<int64_t>(0))
			///		std::cout << "node [0] was an integer with value "sv << **val << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// node [0] was an integer with value 42
			/// \eout
			/// 
			/// \tparam	T	The node's type.
			/// \param 	index	The node's index.
			///
			/// \returns	A pointer to the selected node if it existed and was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] impl::node_of<T>* get_as(size_t index) noexcept
			{
				if (auto val = get(index))
					return val->as<T>();
				return nullptr;
			}

			/// \brief	Gets the node at a specific index if it is a particular type (const overload).
			///
			/// \tparam	T	The node's type.
			/// \param 	index	The node's index.
			///
			/// \returns	A pointer to the selected node if it existed and was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] const impl::node_of<T>* get_as(size_t index) const noexcept
			{
				if (auto val = get(index))
					return val->as<T>();
				return nullptr;
			}

			/// \brief	Equality operator.
			///
			/// \param 	lhs	The LHS array.
			/// \param 	rhs	The RHS array.
			///
			/// \returns	True if the arrays contained the same values.
			friend bool operator == (const array& lhs, const array& rhs) noexcept;

			/// \brief	Inequality operator.
			///
			/// \param 	lhs	The LHS array.
			/// \param 	rhs	The RHS array.
			///
			/// \returns	True if the arrays did not contain the same values.
			friend bool operator != (const array& lhs, const array& rhs) noexcept;

		private:

			template <typename T>
			[[nodiscard]] static bool container_equality(const array& lhs, const T& rhs) noexcept
			{
				using elem_t = std::remove_const_t<typename T::value_type>;
				static_assert(
					impl::is_value_or_promotable<elem_t>,
					"Container element type must be (or be promotable to) one of the TOML value types"
				);

				if (lhs.size() != rhs.size())
					return false;
				if (rhs.size() == 0_sz)
					return true;

				size_t i{};
				for (auto& list_elem : rhs)
				{
					const auto elem = lhs.get_as<impl::promoted<elem_t>>(i++);
					if (!elem || *elem != list_elem)
						return false;
				}

				return true;
			}

			[[nodiscard]] size_t total_leaf_count() const noexcept;

			void flatten_child(array&& child, size_t& dest_index) noexcept;

		public:

			/// \brief	Initializer list equality operator.
			template <typename T>
			[[nodiscard]] friend bool operator == (const array& lhs, const std::initializer_list<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::initializer_list<T>&, template <typename T>)

			/// \brief	Vector equality operator.
			template <typename T>
			[[nodiscard]] friend bool operator == (const array& lhs, const std::vector<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>)

			/// \brief	Flattens this array, recursively hoisting the contents of child arrays up into itself.
			/// 
			/// \detail \cpp
			/// 
			/// auto arr = toml::array{ 1, 2, toml::array{ 3, 4, toml::array{ 5 } }, 6, toml::array{} };
			/// std::cout << arr << std::endl;
			/// 
			/// arr.flatten();
			/// std::cout << arr << std::endl;
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [1, 2, [3, 4, [5]], 6, []]
			/// [1, 2, 3, 4, 5, 6]
			/// \eout
			/// 
			/// \remarks	Arrays inside child tables are not flattened.
			void flatten();

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&);
	};
}


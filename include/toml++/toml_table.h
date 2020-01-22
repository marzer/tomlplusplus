#pragma once
#include "toml_array.h"

namespace toml::impl
{
	template <bool is_const>
	struct table_iterator_ref_proxy final
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

			using reference = table_iterator_ref_proxy<is_const>;

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
}

namespace toml
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;
			friend class node_view<table>;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

		public:

			using iterator = impl::table_iterator<false>;
			using const_iterator = impl::table_iterator<true>;
			
			TOML_NODISCARD_CTOR
			table() noexcept {}

			TOML_NODISCARD_CTOR
			table(table&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) },
				inline_ { other.inline_ }
			{}

			table& operator= (table&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				inline_ = rhs.inline_;
				return *this;
			}

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }

			[[nodiscard]] bool is_table() const noexcept override { return true; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_value() const noexcept override { return false; }

			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }

			[[nodiscard]] bool empty() const noexcept { return values.empty(); }

			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }

			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() };}
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }

			iterator erase(iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}
			iterator erase(const_iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}
			iterator erase(const_iterator first, const_iterator last) noexcept
			{
				return { values.erase(first.raw_, last.raw_) };
			}
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

			template <typename T>
			[[nodiscard]] node_of<T>* get_as(string_view key) noexcept { return do_get_as<T>(values, key); }
			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(string_view key) const noexcept { return do_get_as<T>(values, key); }

			[[nodiscard]] bool contains(string_view key) const noexcept { return do_contains(values, key); }

			[[nodiscard]] inline node_view<table> operator[] (string_view) noexcept;
			[[nodiscard]] inline node_view<const table> operator[] (string_view) const noexcept;

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

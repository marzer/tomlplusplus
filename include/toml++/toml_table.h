#pragma once
#include "toml_node.h"

namespace toml
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;
			friend class default_formatter;
			friend class node_view<table>;

			string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

		public:
			
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

			[[nodiscard]] bool is_table() const noexcept override { return true; }
			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }

			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]]
			node* get(string_view key) noexcept
			{
				if (auto it = values.find(key); it != values.end())
					return it->second.get();
				return nullptr;
			}

			[[nodiscard]]
			const node* get(string_view key) const noexcept
			{
				if (auto it = values.find(key); it != values.end())
					return it->second.get();
				return nullptr;
			}

			template <typename T>
			[[nodiscard]]
			node_of<T>* get_as(string_view key) noexcept
			{
				const auto node = get(key);
				return node ? node->as<T>() : nullptr;
			}

			template <typename T>
			[[nodiscard]]
			const node_of<T>* get_as(string_view key) const noexcept
			{
				const auto node = get(key);
				return node ? node->as<T>() : nullptr;
			}

			[[nodiscard]] inline node_view<table> operator[] (string_view) noexcept;
			[[nodiscard]] inline node_view<const table> operator[] (string_view) const noexcept;
	};
}

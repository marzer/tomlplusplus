#pragma once
#include "toml_common.h"

namespace toml
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;
			friend class impl::writer<void>;
			string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

			[[nodiscard]]
			node* get(string_view key) noexcept
			{
				if (auto it = values.find(key); it != values.end())
					return it->second.get();
				return nullptr;
			}

		public:
			
			[[nodiscard]] bool is_table() const noexcept override { return true; }

			[[nodiscard]] table* as_table() noexcept override { return this; }

			[[nodiscard]] const table* as_table() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }

			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

			TOML_NODISCARD_CTOR
			table() noexcept {}
	};
}

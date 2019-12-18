#pragma once
#include "toml_common.h"

namespace TOML_NAMESPACE
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;
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
			
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] bool is_string() const noexcept override { return false; }
			[[nodiscard]] bool is_int() const noexcept override { return false; }
			[[nodiscard]] bool is_float() const noexcept override { return false; }
			[[nodiscard]] bool is_bool() const noexcept override { return false; }
			[[nodiscard]] bool is_date() const noexcept override { return false; }
			[[nodiscard]] bool is_time() const noexcept override { return false; }
			[[nodiscard]] bool is_datetime() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_table() const noexcept override { return true; }
			[[nodiscard]] bool is_table_array() const noexcept override { return false; }

			[[nodiscard]] value<string>* as_string() noexcept override { return nullptr; }
			[[nodiscard]] value<int64_t>* as_int() noexcept override { return nullptr; }
			[[nodiscard]] value<double>* as_float() noexcept override { return nullptr; }
			[[nodiscard]] value<bool>* as_bool() noexcept override { return nullptr; }
			[[nodiscard]] value<date>* as_date() noexcept override { return nullptr; }
			[[nodiscard]] value<time>* as_time() noexcept override { return nullptr; }
			[[nodiscard]] value<datetime>* as_datetime() noexcept override { return nullptr; }
			[[nodiscard]] array* as_array() noexcept override { return nullptr; }
			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] table_array* as_table_array() noexcept override { return nullptr; }

			[[nodiscard]] const value<string>* as_string() const noexcept override { return nullptr; }
			[[nodiscard]] const value<int64_t>* as_int() const noexcept override { return nullptr; }
			[[nodiscard]] const value<double>* as_float() const noexcept override { return nullptr; }
			[[nodiscard]] const value<bool>* as_bool() const noexcept override { return nullptr; }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return nullptr; }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return nullptr; }
			[[nodiscard]] const value<datetime>* as_datetime() const noexcept override { return nullptr; }
			[[nodiscard]] const array* as_array() const noexcept override { return nullptr; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }
			[[nodiscard]] const table_array* as_table_array() const noexcept override { return nullptr; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }

			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

			TOML_NODISCARD_CTOR
			table() noexcept {}
	};
}

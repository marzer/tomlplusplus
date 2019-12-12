#pragma once
#include "toml_table.h"

namespace TOML_NAMESPACE
{
	class table_array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::unique_ptr<table>> tables;

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
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_table_array() const noexcept override { return true; }

			[[nodiscard]] value<string>* as_string() noexcept override { return nullptr; }
			[[nodiscard]] value<int64_t>* as_int() noexcept override { return nullptr; }
			[[nodiscard]] value<double>* as_float() noexcept override { return nullptr; }
			[[nodiscard]] value<bool>* as_bool() noexcept override { return nullptr; }
			[[nodiscard]] value<date>* as_date() noexcept override { return nullptr; }
			[[nodiscard]] value<time>* as_time() noexcept override { return nullptr; }
			[[nodiscard]] value<datetime>* as_datetime() noexcept override { return nullptr; }
			[[nodiscard]] array* as_array() noexcept override { return nullptr; }
			[[nodiscard]] table* as_table() noexcept override { return nullptr; }
			[[nodiscard]] table_array* as_table_array() noexcept override { return this; }

			[[nodiscard]] const value<string>* as_string() const noexcept override { return nullptr; }
			[[nodiscard]] const value<int64_t>* as_int() const noexcept override { return nullptr; }
			[[nodiscard]] const value<double>* as_float() const noexcept override { return nullptr; }
			[[nodiscard]] const value<bool>* as_bool() const noexcept override { return nullptr; }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return nullptr; }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return nullptr; }
			[[nodiscard]] const value<datetime>* as_datetime() const noexcept override { return nullptr; }
			[[nodiscard]] const array* as_array() const noexcept override { return nullptr; }
			[[nodiscard]] const table* as_table() const noexcept override { return nullptr; }
			[[nodiscard]] const table_array* as_table_array() const noexcept override { return this; }

			[[nodiscard]] int type_id() const noexcept override { return 2; }

			TOML_NODISCARD_CTOR
			table_array() noexcept = default;
	};
}

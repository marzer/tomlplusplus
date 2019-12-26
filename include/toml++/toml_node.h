#pragma once
#include "toml_common.h"

namespace toml
{
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			source_region rgn{};

		public:

			[[nodiscard]] virtual bool is_value() const noexcept { return false; }
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			[[nodiscard]] virtual bool is_bool() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array() const noexcept { return false; }
			[[nodiscard]] virtual bool is_table() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }
			
			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			[[nodiscard]] virtual value<bool>* as_bool() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }

			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_bool() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }

			[[nodiscard]] virtual node_type type() const noexcept = 0;

			[[nodiscard]]
			const source_region& region() const noexcept
			{
				return rgn;
			}

			virtual ~node() noexcept = default;
	};
}

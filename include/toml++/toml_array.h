#pragma once
#include "toml_common.h"

namespace TOML_NAMESPACE
{
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::shared_ptr<node>> values;

		public:
			
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] bool is_string() const noexcept override { return false; }
			[[nodiscard]] bool is_int() const noexcept override { return false; }
			[[nodiscard]] bool is_float() const noexcept override { return false; }
			[[nodiscard]] bool is_bool() const noexcept override { return false; }
			[[nodiscard]] bool is_date() const noexcept override { return false; }
			[[nodiscard]] bool is_time() const noexcept override { return false; }
			[[nodiscard]] bool is_datetime() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return true; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_table_array() const noexcept override { return false; }

			[[nodiscard]] std::shared_ptr<value<string>> as_string() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<int64_t>> as_int() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<double>> as_float() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<bool>> as_bool() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<date>> as_date() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<time>> as_time() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<value<datetime>> as_datetime() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<array> as_array() noexcept override { return std::static_pointer_cast<array>(shared_from_this()); }
			[[nodiscard]] std::shared_ptr<table> as_table() noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<table_array> as_table_array() noexcept override { return {}; }

			[[nodiscard]] std::shared_ptr<const value<string>> as_string() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<int64_t>> as_int() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<double>> as_float() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<bool>> as_bool() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<date>> as_date() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<time>> as_time() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const value<datetime>> as_datetime() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const array> as_array() const noexcept override { return std::static_pointer_cast<const array>(shared_from_this()); }
			[[nodiscard]] std::shared_ptr<const table> as_table() const noexcept override { return {}; }
			[[nodiscard]] std::shared_ptr<const table_array> as_table_array() const noexcept override { return {}; }

			[[nodiscard]] int type_id() const noexcept override { return 0; }

			[[nodiscard]]
			bool is_homogeneous() const noexcept
			{
				using namespace impl; //_sz

				if (values.size() <= 1_sz)
					return true;

				const auto id = values[0]->type_id();
				for (size_t i = 1; i < values.size(); i++)
					if (values[i]->type_id() != id)
						return false;
				return true;
			}

			array() noexcept {}
	};
}

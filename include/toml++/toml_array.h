#pragma once
#include "toml_common.h"

namespace toml
{
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			friend class impl::writer<void>;
			std::vector<std::unique_ptr<node>> values;

		public:
			
			[[nodiscard]] bool is_array() const noexcept override { return true; }

			[[nodiscard]] bool is_array_of_tables() const noexcept override
			{
				if (values.empty())
					return false;

				for (auto& val : values)
					if (!val->is_table())
						return false;

				return true;
			}

			[[nodiscard]] array* as_array() noexcept override { return this; }

			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::array; }

			[[nodiscard]]
			bool is_homogeneous() const noexcept
			{
				using namespace impl; //_sz

				if (values.size() <= 1_sz)
					return true;

				const auto type = values[0]->type();
				for (size_t i = 1; i < values.size(); i++)
					if (values[i]->type() != type)
						return false;
				return true;
			}

			TOML_NODISCARD_CTOR
			array() noexcept = default;
	};
}

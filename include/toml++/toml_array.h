#pragma once
#include "toml_node.h"

namespace toml
{
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			friend class default_formatter;
			std::vector<std::unique_ptr<node>> values;

		public:

			TOML_NODISCARD_CTOR
			array() noexcept = default;

			TOML_NODISCARD_CTOR
			array(array&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) }
			{}

			array& operator= (array&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				return *this;
			}
		
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

			[[nodiscard]]
			bool is_homogeneous() const noexcept
			{
				if (values.size() <= 1_sz)
					return true;

				const auto type = values[0]->type();
				for (size_t i = 1; i < values.size(); i++)
					if (values[i]->type() != type)
						return false;
				return true;
			}

			[[nodiscard]] array* as_array() noexcept override { return this; }
			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::array; }

			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]] node* get(size_t index) noexcept { return values[index].get(); }
			[[nodiscard]] const node* get(size_t index) const noexcept { return values[index].get(); }

			template <typename T>
			[[nodiscard]]
			node_of<T>* get_as(size_t index) noexcept
			{
				return get(index)->as<T>();
			}

			template <typename T>
			[[nodiscard]]
			const node_of<T>* get_as(size_t index) const noexcept
			{
				return get(index)->as<T>();
			}
	};
}

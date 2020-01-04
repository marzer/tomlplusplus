#pragma once
#include "toml_common.h"

namespace toml
{
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			source_region rgn{};

		protected:
			
			node(node&& other) noexcept
				: rgn{ std::move(other.rgn) }
			{}

			node& operator= (node&& rhs) noexcept
			{
				rgn = std::move(rhs.rgn);
				return *this;
			}

		public:

			node() noexcept = default;
			node(const node&) = delete;
			node& operator= (const node&) = delete;
			virtual ~node() noexcept = default;

			[[nodiscard]] virtual bool is_value() const noexcept { return false; }
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			[[nodiscard]] virtual bool is_boolean() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array() const noexcept { return false; }
			[[nodiscard]] virtual bool is_table() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }

			template <typename T>
			[[nodiscard]] bool is() const noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return is_string();
				else if constexpr (std::is_same_v<type, int64_t>) return is_integer();
				else if constexpr (std::is_same_v<type, double>) return is_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return is_boolean();
				else if constexpr (std::is_same_v<type, date>) return is_date();
				else if constexpr (std::is_same_v<type, time>) return is_time();
				else if constexpr (std::is_same_v<type, date_time>) return is_date_time();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, table>) return is_table();
			}
			
			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			[[nodiscard]] virtual value<bool>* as_boolean() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }


			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_boolean() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }

			template <typename T>
			[[nodiscard]] node_of<T>* as() noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, table>) return as_table();
			}

			template <typename T>
			[[nodiscard]] const node_of<T>* as() const noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, table>) return as_table();
			}

			[[nodiscard]] virtual node_type type() const noexcept = 0;

			[[nodiscard]]
			const source_region& region() const noexcept
			{
				return rgn;
			}
	};
}

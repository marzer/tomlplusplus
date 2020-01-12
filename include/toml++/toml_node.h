#pragma once
#include "toml_common.h"

namespace toml
{
	/// \brief	A TOML node.
	///
	/// \detail A parsed TOML document forms a tree made up of tables, arrays and values.
	/// 		This type is the base of each of those, providing a lot of the polymorphic plumbing.
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			source_region source_{};

		protected:
			
			node(node&& other) noexcept
				: source_{ std::move(other.source_) }
			{}

			node& operator= (node&& rhs) noexcept
			{
				source_ = std::move(rhs.source_);
				return *this;
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>* reinterpret_as() noexcept
			{
				return reinterpret_cast<node_of<T>*>(this);
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const node_of<T>* reinterpret_as() const noexcept
			{
				return reinterpret_cast<const node_of<T>*>(this);
			}

			node() noexcept = default;
			node(const node&) = delete;
			node& operator= (const node&) = delete;

		public:

			virtual ~node() noexcept = default;


			/// \brief	Returns the node's type identifier.
			[[nodiscard]] virtual node_type type() const noexcept = 0;


			/// \brief	Returns true if this node is a table.
			[[nodiscard]] virtual bool is_table() const noexcept = 0;
			/// \brief	Returns true if this node is an array.
			[[nodiscard]] virtual bool is_array() const noexcept = 0;
			/// \brief	Returns true if this node is a value.
			[[nodiscard]] virtual bool is_value() const noexcept = 0;

			/// \brief	Returns true if this node is a string value.
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			/// \brief	Returns true if this node is an integer value.
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			/// \brief	Returns true if this node is an floating-point value.
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			/// \brief	Returns true if this node is a boolean value.
			[[nodiscard]] virtual bool is_boolean() const noexcept { return false; }
			/// \brief	Returns true if this node is a local date value.
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			/// \brief	Returns true if this node is a local time value.
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			/// \brief	Returns true if this node is a date-time value.
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			/// \brief	Returns true if this node is an array containing only tables.
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }


			/// \brief	Checks if a node is a specific type.
			///
			/// \tparam	T	The 
			///
			/// \returns	Returns true if this node is an instance
			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is() const noexcept
			{
				using type = value_of<impl::remove_cvref_t<T>>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return is_table();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, string>) return is_string();
				else if constexpr (std::is_same_v<type, int64_t>) return is_integer();
				else if constexpr (std::is_same_v<type, double>) return is_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return is_boolean();
				else if constexpr (std::is_same_v<type, date>) return is_date();
				else if constexpr (std::is_same_v<type, time>) return is_time();
				else if constexpr (std::is_same_v<type, date_time>) return is_date_time();
			}
			
			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			[[nodiscard]] virtual value<bool>* as_boolean() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }

			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_boolean() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>* as() noexcept
			{
				using type = value_of<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const node_of<T>* as() const noexcept
			{
				using type = value_of<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			[[nodiscard]] const source_region& source() const noexcept
			{
				return source_;
			}

		private:

			// this is done using a static helper to preserve const categories
			// (otherwise I'd have to implement this function twice)
			// (const propagation in C++: a modern horror story)
			template <typename N, typename FUNC>
			TOML_GCC_ATTR(nonnull)
			static decltype(auto) do_visit(N* node, FUNC&& visitor) TOML_MAY_THROW
			{
				static_assert(
					std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				static constexpr auto is_exhaustive =
					std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>;

				switch (node->type())
				{
					case node_type::table:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<table>());
						break;
					case node_type::array:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<array>());
						break;
					case node_type::string:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<string>());
						break;
					case node_type::integer:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<int64_t>());
						break;
					case node_type::floating_point:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<double>());
						break;
					case node_type::boolean:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<bool>());
						break;
					case node_type::date:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<date>());
						break;
					case node_type::time:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<time>());
						break;
					case node_type::date_time:
						if constexpr (std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<date_time>());
						break;
					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (is_exhaustive)
					TOML_UNREACHABLE;
			}

		public:

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) TOML_MAY_THROW
			{
				return do_visit(this, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const TOML_MAY_THROW
			{
				return do_visit(this, std::forward<FUNC>(visitor));
			}
	};
}

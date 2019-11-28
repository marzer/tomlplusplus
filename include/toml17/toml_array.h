#pragma once
#include "toml_value.h"

namespace TOML17_NAMESPACE
{
	class array final
	{
		private:
			using storage_t = std::aligned_storage_t <
				std::max({ sizeof(std::vector<std::string>), sizeof(std::vector<int64_t>), sizeof(std::vector<double>), sizeof(std::vector<bool>) }),
				std::max({ alignof(std::vector<std::string>), alignof(std::vector<int64_t>), alignof(std::vector<double>), alignof(std::vector<bool>) })
			> ;
			using value_type = detail::value_type;

			storage_t storage;
			value_type type = value_type::none;

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static std::vector<detail::type_of<AS>>& raw_ref(storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				return *std::launder(reinterpret_cast<std::vector<detail::type_of<AS>>*>(&storage));
			}

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static const std::vector<detail::type_of<AS>>& raw_ref(const storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				return *std::launder(reinterpret_cast<const std::vector<detail::type_of<AS>>*>(&storage));
			}

			template <value_type AS>
			TOML17_ALWAYS_INLINE
			static void destroy(storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				using value_t = std::vector<detail::type_of<AS>>;
				raw_ref<AS>(storage).value_t::~value_t();
			}

			void destroy_value_if_nontrivial() noexcept
			{
				if (type == value_type::none)
					return;

				switch (type)
				{
					case value_type::string:
						destroy<value_type::string>(storage);
						return;

					case value_type::integer:
						destroy<value_type::integer>(storage);
						return;

					case value_type::floating_point:
						destroy<value_type::floating_point>(storage);
						return;

					case value_type::boolean:
						destroy<value_type::boolean>(storage);
						return;

					TOML17_NO_DEFAULT_CASE;
				}
			}

		public:
			array() noexcept = default;

			template <typename T>
			explicit array(std::vector<T>&& vals) noexcept
				: type{ detail::value_type_of<T> }
			{
				static_assert(
					is_value_type<T>,
					"Template type argument must be one of the basic TOML value types"
				);

				new (&storage) std::vector<T>( std::move(vals) );
			}

			template <typename T>
			explicit array(const std::vector<T>& vals) noexcept
				: type{ detail::value_type_of<T> }
			{
				static_assert(
					is_value_type<T>,
					"Template type argument must be one of the basic TOML value types"
				);

				new (&storage) std::vector<T>(vals);
			}

			array(const array& other) noexcept
				: type{ other.type }
			{
				if (type == value_type::none)
					return;

				switch (type)
				{
					case value_type::string:
						new (&storage) std::vector<detail::type_of<value_type::string>>( raw_ref<value_type::string>(other.storage) );
						break;

					case value_type::integer:
						new (&storage) std::vector<detail::type_of<value_type::integer>>(raw_ref<value_type::integer>(other.storage));
						break;

					case value_type::floating_point:
						new (&storage) std::vector<detail::type_of<value_type::floating_point>>(raw_ref<value_type::floating_point>(other.storage));
						break;

					case value_type::boolean:
						new (&storage) std::vector<detail::type_of<value_type::boolean>>(raw_ref<value_type::boolean>(other.storage));
						break;

					TOML17_NO_DEFAULT_CASE;
				}
			}

			array(array&& other) noexcept
				: type{ other.type }
			{
				if (type != value_type::none)
				{
					other.type = value_type::none;
					memcpy(&storage, &other.storage, sizeof(storage));
				}
			}

			array& operator=(const array& rhs) noexcept
			{
				if (&rhs != this)
				{
					destroy_value_if_nontrivial();

					type = rhs.type;

					if (type != value_type::none)
					{
						switch (type)
						{
							case value_type::string:
								new (&storage) std::vector<detail::type_of<value_type::string>>(raw_ref<value_type::string>(rhs.storage));
								break;

							case value_type::integer:
								new (&storage) std::vector<detail::type_of<value_type::integer>>(raw_ref<value_type::integer>(rhs.storage));
								break;

							case value_type::floating_point:
								new (&storage) std::vector<detail::type_of<value_type::floating_point>>(raw_ref<value_type::floating_point>(rhs.storage));
								break;

							case value_type::boolean:
								new (&storage) std::vector<detail::type_of<value_type::boolean>>(raw_ref<value_type::boolean>(rhs.storage));
								break;

							TOML17_NO_DEFAULT_CASE;
						}
					}
				}
				return *this;
			}

			array& operator=(array&& rhs) noexcept
			{
				if (&rhs != this)
				{
					destroy_value_if_nontrivial();

					type = rhs.type;
					rhs.type = value_type::none;
					memcpy(&storage, &rhs.storage, sizeof(storage));
				}
				return *this;
			}

			template <typename T>
			array& operator= (std::vector<T>&& rhs) noexcept
			{
				static_assert(
					is_value_type<T>,
					"Template type argument must be one of the basic TOML value types"
				);

				destroy_value_if_nontrivial();
				type = detail::value_type_of<T>;
				new (&storage) std::vector<T>(std::move(vals));
				return *this;
			}

			template <typename T>
			array& operator= (const std::vector<T>& rhs) noexcept
			{
				static_assert(
					is_value_type<T>,
					"Template type argument must be one of the basic TOML value types"
				);

				destroy_value_if_nontrivial();
				type = detail::value_type_of<T>;
				new (&storage) std::vector<T>(vals);
				return *this;
			}

			~array() noexcept
			{
				destroy_value_if_nontrivial();
			}

			[[nodiscard]]
			explicit operator bool() const noexcept
			{
				return type != value_type::none;
			}

			template <typename T>
			[[nodiscard]]
			bool is() const noexcept
			{
				static_assert(
					is_value_type<T>,
					"Template type argument must be one of the basic TOML value types"
				);

				return type == detail::value_type_of<T>;
			}

			template <typename T>
			[[nodiscard]]
			auto& as()
			{
				using nonconst_value_t = std::remove_const_t<T>;

				static_assert(
					!std::is_reference_v<T> && !std::is_volatile_v<T> && is_value_type<nonconst_value_t>,
					"Template type argument must be one of the basic TOML value types"
				);

				if (!this->is<nonconst_value_t>())
					throw std::bad_cast{ "array::as() requires that the underlying value type match the template type argument" };

				if constexpr (std::is_const_v<T>)
					return static_cast<const std::vector<detail::value_type_of<nonconst_value_t>>&>(
						raw_value<detail::value_type_of<nonconst_value_t>>(storage)
					);
				else
					return raw_value<detail::value_type_of<nonconst_value_t>>(storage)
			}

			template <typename T>
			[[nodiscard]]
			const std::vector<T>& as() const
			{
				using nonconst_value_t = std::remove_const_t<T>;

				static_assert(
					!std::is_reference_v<T> && !std::is_volatile_v<T> && is_value_type<nonconst_value_t>,
					"Template type argument must be one of the basic TOML value types"
				);

				if (!this->is<nonconst_value_t>())
					throw std::bad_cast{ "array::as() requires that the underlying value type match the template type argument" };

				return raw_value<detail::value_type_of<nonconst_value_t>>(storage);
			}
	};
}

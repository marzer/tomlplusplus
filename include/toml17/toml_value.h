#pragma once
#include "toml_common.h"

namespace TOML17_NAMESPACE
{
	//template <typename T>
	//inline constexpr bool is_value_compatible_type =
	//	is_value_type<std::remove_const_t<T>>
	//	|| std::is_same_v<std::remove_const_t<T>, std::string_view>;

	namespace detail
	{
		enum class value_type : uint8_t
		{
			none,
			string,
			integer,
			floating_point,
			boolean
		};

		template <value_type AS>
		struct value_enum_to_type
		{
			using type = void;
		};

		template <> struct value_enum_to_type<value_type::string>			{ using type = std::string; };
		template <> struct value_enum_to_type<value_type::integer>			{ using type = int64_t; };
		template <> struct value_enum_to_type<value_type::floating_point>	{ using type = double; };
		template <> struct value_enum_to_type<value_type::boolean>			{ using type = bool; };

		template <typename T>
		struct value_type_to_enum
		{
			static constexpr auto value = value_type::none;
		};

		template <> struct value_type_to_enum<std::string>	{ static constexpr auto value = value_type::string; };
		template <> struct value_type_to_enum<int64_t>	{ static constexpr auto value = value_type::integer; };
		template <> struct value_type_to_enum<double>		{ static constexpr auto value = value_type::floating_point; };
		template <> struct value_type_to_enum<bool>			{ static constexpr auto value = value_type::boolean; };

		template <value_type AS>
		using type_of = typename value_enum_to_type<AS>::type;

		template <typename T>
		inline constexpr auto value_type_of = value_type_to_enum<T>::value;
	}

	class value final
	{
		private:
			

			using storage_t = std::aligned_storage_t<
				std::max({ sizeof(std::string), sizeof(int64_t), sizeof(double), sizeof(bool) }),
				std::max({ alignof(std::string), alignof(int64_t), alignof(double), alignof(bool) })
			>;
			using value_type = detail::value_type;

			storage_t storage;
			value_type type = value_type::none;

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static detail::type_of<AS>& raw_ref(storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				return *std::launder(reinterpret_cast<detail::type_of<AS>*>(&storage));
			}

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static const detail::type_of<AS>& raw_ref(const storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				return *std::launder(reinterpret_cast<const detail::type_of<AS>*>(&storage));
			}

			template <value_type AS>
			TOML17_ALWAYS_INLINE
			static void destroy([[maybe_unused]] storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				using value_t = detail::type_of<AS>;
				if constexpr (!std::is_trivially_destructible_v<value_t>)
					raw_ref<AS>(storage).value_t::~value_t();
			}

			void destroy_value_if_nontrivial() noexcept
			{
				if (type == value_type::string)
					destroy<value_type::string>(storage);
			}

		public:
			value() noexcept = default;

			explicit value(std::string_view val) noexcept
				: type{ value_type::string }
			{
				new (&storage) std::string{ val };
			}

			explicit value(std::string&& val) noexcept
				: type{ value_type::string }
			{
				new (&storage) std::string{ std::move(val) };
			}

			explicit value(int64_t val) noexcept
				: type{ value_type::integer }
			{
				new (&storage) int64_t{ val };
			}

			explicit value(double val) noexcept
				: type{ value_type::floating_point }
			{
				new (&storage) double{ val };
			}

			explicit value(bool val) noexcept
				: type{ value_type::boolean }
			{
				new (&storage) bool{ val };
			}

			value(const value& other) noexcept
				: type{ other.type }
			{
				if (type == value_type::string)
					new (&storage) std::string{ raw_ref<value_type::string>(other.storage) };
				else
					memcpy(&storage, &other.storage, sizeof(storage));
			}

			value(value&& other) noexcept
				: type{ other.type }
			{
				other.type = value_type::none; 
				memcpy(&storage, &other.storage, sizeof(storage));
			}

			value& operator=(const value& rhs) noexcept
			{
				if (&rhs != this)
				{
					destroy_value_if_nontrivial();

					type = rhs.type;

					if (type == value_type::string)
						new (&storage) std::string{ raw_ref<value_type::string>(rhs.storage) };
					else
						memcpy(&storage, &rhs.storage, sizeof(storage));
				}
				return *this;
			}

			value& operator=(value&& rhs) noexcept
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

			value& operator= (std::string_view rhs) noexcept
			{
				destroy_value_if_nontrivial();
				type = value_type::string;
				new (&storage) std::string{ rhs };
				return *this;
			}

			value& operator= (std::string&& rhs) noexcept
			{
				destroy_value_if_nontrivial();
				type = value_type::string;
				new (&storage) std::string{ std::move(rhs) };
				return *this;
			}

			value& operator= (int64_t rhs) noexcept
			{
				destroy_value_if_nontrivial();
				type = value_type::integer;
				new (&storage) int64_t{ rhs };
				return *this;
			}

			value& operator= (double rhs) noexcept
			{
				destroy_value_if_nontrivial();
				type = value_type::floating_point;
				new (&storage) double{ rhs };
				return *this;
			}

			value& operator= (bool rhs) noexcept
			{
				destroy_value_if_nontrivial();
				type = value_type::boolean;
				new (&storage) bool{ rhs };
				return *this;
			}

			~value() noexcept
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
			T& as()
			{
				using nonconst_value_t = std::remove_const_t<T>;

				static_assert(
					!std::is_reference_v<T> && !std::is_volatile_v<T> && is_value_type<nonconst_value_t>,
					"Template type argument must be one of the basic TOML value types"
				);

				if (!this->is<nonconst_value_t>())
					throw std::bad_cast{ "value::as() requires that the underlying value type match the template type argument" };

				return raw_value<detail::value_type_of<nonconst_value_t>>(storage);
			}

			template <typename T>
			[[nodiscard]]
			const T& as() const
			{
				using nonconst_value_t = std::remove_const_t<T>;

				static_assert(
					!std::is_reference_v<T> && !std::is_volatile_v<T> && is_value_type<nonconst_value_t>,
					"Template type argument must be one of the basic TOML value types"
				);

				if (!this->is<nonconst_value_t>())
					throw std::bad_cast{ "value::as() requires that the underlying value type match the template type argument" };

				return raw_value<detail::value_type_of<nonconst_value_t>>(storage);
			}
	};
}

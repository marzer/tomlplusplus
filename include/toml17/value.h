#pragma once
#include "base.h"

namespace TOML17_NAMESPACE
{
	template <typename T>
	inline constexpr bool is_value_type =
		std::is_same_v<T, std::string>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, double>
		|| std::is_same_v<T, bool>;

	template <typename T>
	inline constexpr bool is_value_compatible_type =
		is_value_type<T>
		|| std::is_same_v<T, std::string_view>;

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
		template <> struct value_enum_to_type<value_type::floating_point>	{ using type = float_t; };
		template <> struct value_enum_to_type<value_type::boolean>			{ using type = bool; };

		template <typename T>
		struct value_type_to_enum
		{
			static constexpr auto value = value_type::none;
		};

		template <> struct value_type_to_enum<std::string>	{ static constexpr auto value = value_type::string; };
		template <> struct value_type_to_enum<int64_t>	{ static constexpr auto value = value_type::integer; };
		template <> struct value_type_to_enum<float_t>		{ static constexpr auto value = value_type::floating_point; };
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
				std::max({ sizeof(std::string), sizeof(int64_t), sizeof(float_t), sizeof(bool) }),
				std::max({ alignof(std::string), alignof(int64_t), alignof(float_t), alignof(bool) })
			>;
			using value_type = detail::value_type;

			storage_t storage;
			value_type type;

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static detail::type_of<AS>& raw_value(storage_t& storage) noexcept
			{
				static_assert(
					AS != value_type::none,
					"Value type cannot be none"
				);

				return *std::launder(reinterpret_cast<detail::type_of<AS>*>(&storage));
			}

			template <value_type AS>
			[[nodiscard]] TOML17_ALWAYS_INLINE
			static const detail::type_of<AS>& raw_value(const storage_t& storage) noexcept
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
					raw_value<AS>(storage).value_t::~value_t();
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

			explicit value(float_t val) noexcept
				: type{ value_type::floating_point }
			{
				new (&storage) float_t{ val };
			}

			explicit value(bool val) noexcept
				: type{ value_type::boolean }
			{
				new (&storage) bool{ val };
			}

			~value() noexcept
			{
				if (type == value_type::string)
					destroy<value_type::string>(storage);
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
					!std::is_reference_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>,
					"Type cannot be const, volatile, or a reference"
				);

				static_assert(
					is_value_type<T>,
					"Type must be one of the basic value types"
				);

				return type == detail::value_type_of<T>;
			}

			template <typename T>
			[[nodiscard]]
			auto as() noexcept
			{
				static_assert(
					!std::is_reference_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>,
					"Type cannot be const, volatile, or a reference"
				);

				static_assert(
					is_value_compatible_type<T>,
					"Type must be one of the basic value types (or convertible to it)"
				);

				//to std::string
				if constexpr (std::is_same_v<T, std::string>)
				{
					using return_t = std::string*;
					return type == value_type::string
						? return_t{ &raw_value<value_type::string>(storage) }
						: return_t{};
				}

				//to std::string_view
				else if constexpr (std::is_same_v<T, std::string_view>)
				{
					using return_t = std::optional<std::string_view>;
					return type == value_type::string
						? return_t{ raw_value<value_type::string>(storage) }
						: return_t{};
				}

				//to integer
				else if constexpr (std::is_same_v<T, int64_t>)
				{
					using return_t = int64_t*;
					return type == value_type::integer
						? return_t{ &raw_value<value_type::integer>(storage) }
						: return_t{};
				}

				//to float
				else if constexpr (std::is_same_v<T, float_t>)
				{
					using return_t = double*;
					return type == value_type::floating_point
						? return_t{ &raw_value<value_type::floating_point>(storage) }
					: return_t{};
				}

				//to bool
				else if constexpr (std::is_same_v<T, bool>)
				{
					using return_t = bool*;
					return type == value_type::boolean
						? return_t{ &raw_value<value_type::boolean>(storage) }
					: return_t{};
				}
			}

			template <typename T>
			[[nodiscard]]
			auto as() const noexcept
			{
				static_assert(
					!std::is_reference_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>,
					"Type cannot be const, volatile, or a reference"
				);

				static_assert(
					is_value_compatible_type<T>,
					"Type must be one of the basic value types (or convertible to it)"
				);

				//to std::string
				if constexpr (std::is_same_v<T, std::string>)
				{
					using return_t = const std::string*;
					return type == value_type::string
						? return_t{ &raw_value<value_type::string>(storage) }
						: return_t{};
				}

				//to std::string_view
				else if constexpr (std::is_same_v<T, std::string_view>)
				{
					using return_t = std::optional<std::string_view>;
					return type == value_type::string
						? return_t{ raw_value<value_type::string>(storage) }
						: return_t{};
				}

				//to integer
				else if constexpr (std::is_same_v<T, int64_t>)
				{
					using return_t = const int64_t*;
					return type == value_type::integer
						? return_t{ &raw_value<value_type::integer>(storage) }
						: return_t{};
				}

				//to float
				else if constexpr (std::is_same_v<T, float_t>)
				{
					using return_t = const double*;
					return type == value_type::floating_point
						? return_t{ &raw_value<value_type::floating_point>(storage) }
					: return_t{};
				}

				//to bool
				else if constexpr (std::is_same_v<T, bool>)
				{
					using return_t = const bool*;
					return type == value_type::boolean
						? return_t{ &raw_value<value_type::boolean>(storage) }
					: return_t{};
				}
			}
	};

}

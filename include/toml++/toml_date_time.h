#pragma once
#include "toml_common.h"

namespace toml
{
	struct date final
	{
		uint16_t year;
		uint8_t month;
		uint8_t day;

		[[nodiscard]]
		friend constexpr bool operator == (date lhs, date rhs) noexcept
		{
			return lhs.year == rhs.year
				&& lhs.month == rhs.month
				&& lhs.day == rhs.day;
		}

		[[nodiscard]]
		friend constexpr bool operator != (date lhs, date rhs) noexcept
		{
			return lhs.year != rhs.year
				|| lhs.month != rhs.month
				|| lhs.day != rhs.day;
		}

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	struct time final
	{
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint32_t nanosecond;

		[[nodiscard]]
		friend constexpr bool operator == (const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour
				&& lhs.minute == rhs.minute
				&& lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		[[nodiscard]]
		friend constexpr bool operator != (const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	struct time_offset final
	{
		int16_t minutes;

		[[nodiscard]]
		static constexpr time_offset from_hh_mm(int8_t hours, int8_t minutes) noexcept
		{
			return time_offset{ static_cast<int16_t>(hours * 60 + minutes) };
		}

		[[nodiscard]]
		friend constexpr bool operator == (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator != (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time_offset& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	struct date_time final
	{
		toml::date date;
		toml::time time;
		std::optional<toml::time_offset> time_offset;

		[[nodiscard]]
		constexpr bool is_local() const noexcept
		{
			return !time_offset.has_value();
		}

		[[nodiscard]]
		friend constexpr bool operator == (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date
				&& lhs.time == rhs.time
				&& lhs.time_offset == rhs.time_offset;
		}

		[[nodiscard]]
		friend constexpr bool operator != (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date != rhs.date
				|| lhs.time != rhs.time
				|| lhs.time_offset != rhs.time_offset;
		}

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date_time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};
}

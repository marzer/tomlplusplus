#pragma once
#include "toml_common.h"

namespace toml
{
	/// \brief	A local date.
	struct date final
	{
		/// \brief	The year component.
		uint16_t year;
		/// \brief	The month component, from 1 - 12.
		uint8_t month;
		/// \brief	The day component, from 1 - 31.
		uint8_t day;


		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS date.
		/// \param 	rhs	The RHS date.
		///
		/// \returns	True if the dates represented the same value.
		[[nodiscard]]
		friend constexpr bool operator == (date lhs, date rhs) noexcept
		{
			return lhs.year == rhs.year
				&& lhs.month == rhs.month
				&& lhs.day == rhs.day;
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS date.
		/// \param 	rhs	The RHS date.
		///
		/// \returns	True if the dates did not represent the same value.
		[[nodiscard]]
		friend constexpr bool operator != (date lhs, date rhs) noexcept
		{
			return lhs.year != rhs.year
				|| lhs.month != rhs.month
				|| lhs.day != rhs.day;
		}


		/// \brief	Prints a date out to a stream as `YYYY-MM-DD` (per RFC 3339).
		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	/// \brief	A local time-of-day.
	struct time final
	{
		/// \brief	The hour component, from 0 - 23.
		uint8_t hour;
		/// \brief	The minute component, from 0 - 59.
		uint8_t minute;
		/// \brief	The second component, from 0 - 59.
		uint8_t second;
		/// \brief	The fractional nanoseconds component, from 0 - 999999999.
		uint32_t nanosecond;

		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS time.
		/// \param 	rhs	The RHS time.
		///
		/// \returns	True if the times represented the same value.
		[[nodiscard]]
		friend constexpr bool operator == (const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour
				&& lhs.minute == rhs.minute
				&& lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS time.
		/// \param 	rhs	The RHS time.
		///
		/// \returns	True if the times did not represent the same value.
		[[nodiscard]]
		friend constexpr bool operator != (const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// \brief	Prints a time out to a stream as `HH:MM:SS.FFFFFF` (per RFC 3339).
		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	/// \brief	A timezone offset.
	struct time_offset final
	{
		/// \brief	Offset from UTC+0, in minutes.
		int16_t minutes;

		/// \brief	Creates a timezone offset from separate hour and minute totals.
		///
		/// \detail \cpp
		/// std::cout << time_offset::from_hh_mm(2, 30) << std::endl;
		/// std::cout << time_offset::from_hh_mm(-2, 30) << std::endl;
		/// std::cout << time_offset::from_hh_mm(-2, -30) << std::endl;
		/// std::cout << time_offset::from_hh_mm(0,0) << std::endl;
		/// 
		/// \ecpp
		/// 
		/// \out
		/// +02:30
		/// -01:30
		/// -02:30
		/// Z
		/// \eout
		/// 
		/// \param 	hours  	The total hours.
		/// \param 	minutes	The total minutes.
		///
		/// \returns	A time_offset.
		[[nodiscard]]
		static constexpr time_offset from_hh_mm(int8_t hours, int8_t minutes) noexcept
		{
			return time_offset{ static_cast<int16_t>(hours * 60 + minutes) };
		}

		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS time_offset.
		/// \param 	rhs	The RHS time_offset.
		///
		/// \returns	True if the time_offsets represented the same value.
		[[nodiscard]]
		friend constexpr bool operator == (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS time_offset.
		/// \param 	rhs	The RHS time_offset.
		///
		/// \returns	True if the time_offsets did not represent the same value.
		[[nodiscard]]
		friend constexpr bool operator != (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		/// \brief	Prints a time_offset out to a stream as `+-HH:MM or Z` (per RFC 3339).
		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time_offset& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	/// \brief	A date-time.
	struct date_time final
	{
		/// \brief	The date component.
		toml::date date;
		/// \brief	The time component.
		toml::time time;
		/// \brief	The timezone offset component.
		///
		/// \remarks The date_time is said to be 'local' if the time_offset is empty.
		std::optional<toml::time_offset> time_offset;

		/// \brief	Returns true if this date_time does not contain timezone offset information.
		[[nodiscard]]
		constexpr bool is_local() const noexcept
		{
			return !time_offset.has_value();
		}

		/// \brief	Equality operator.
		///
		/// \param 	lhs	The LHS date_time.
		/// \param 	rhs	The RHS date_time.
		///
		/// \returns	True if the date_times represented the same value.
		[[nodiscard]]
		friend constexpr bool operator == (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date
				&& lhs.time == rhs.time
				&& lhs.time_offset == rhs.time_offset;
		}

		/// \brief	Inequality operator.
		///
		/// \param 	lhs	The LHS date_time.
		/// \param 	rhs	The RHS date_time.
		///
		/// \returns	True if the date_times did not represent the same value.
		[[nodiscard]]
		friend constexpr bool operator != (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date != rhs.date
				|| lhs.time != rhs.time
				|| lhs.time_offset != rhs.time_offset;
		}

		/// \brief	Prints a date_time out to a stream in RFC 3339 format.
		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date_time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};
}

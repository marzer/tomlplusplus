//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_common.h"

namespace toml
{
	/// \brief	A local date.
	struct TOML_TRIVIAL_ABI date final
	{
		/// \brief	The year component.
		uint16_t year;
		/// \brief	The month component, from 1 - 12.
		uint8_t month;
		/// \brief	The day component, from 1 - 31.
		uint8_t day;

		/// \brief	Equality operator.
		[[nodiscard]]
		friend constexpr bool operator == (date lhs, date rhs) noexcept
		{
			return lhs.year == rhs.year
				&& lhs.month == rhs.month
				&& lhs.day == rhs.day;
		}

		/// \brief	Inequality operator.
		[[nodiscard]]
		friend constexpr bool operator != (date lhs, date rhs) noexcept
		{
			return lhs.year != rhs.year
				|| lhs.month != rhs.month
				|| lhs.day != rhs.day;
		}

	private:

		[[nodiscard]] TOML_ALWAYS_INLINE
		static constexpr uint32_t pack(date d) noexcept
		{
			return static_cast<uint32_t>(d.year) << 16
				| static_cast<uint32_t>(d.month) << 8
				| static_cast<uint32_t>(d.day);
		}

	public:

		/// \brief	Less-than operator.
		[[nodiscard]]
		friend constexpr bool operator <  (date lhs, date rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		/// \brief	Less-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator <= (date lhs, date rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		/// \brief	Greater-than operator.
		[[nodiscard]]
		friend constexpr bool operator >  (date lhs, date rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator >= (date lhs, date rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}
	};

	/// \brief	Prints a date out to a stream as `YYYY-MM-DD` (per RFC 3339).
	/// \detail \cpp
	/// std::cout << toml::date{ 1987, 3, 16 } << std::endl;
	/// \ecpp
	/// 
	/// \out
	/// 1987-03-16
	/// \eout
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	/// \brief	A local time-of-day.
	struct TOML_TRIVIAL_ABI time final
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
		[[nodiscard]]
		friend constexpr bool operator == (const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour
				&& lhs.minute == rhs.minute
				&& lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		/// \brief	Inequality operator.
		[[nodiscard]]
		friend constexpr bool operator != (const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	private:

		[[nodiscard]] TOML_ALWAYS_INLINE
		static constexpr uint64_t pack(time t) noexcept
		{
			return static_cast<uint64_t>(t.hour) << 48
				| static_cast<uint64_t>(t.minute) << 40
				| static_cast<uint64_t>(t.second) << 32
				| static_cast<uint64_t>(t.nanosecond);
		}

	public:

		/// \brief	Less-than operator.
		[[nodiscard]]
		friend constexpr bool operator <  (time lhs, time rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		/// \brief	Less-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator <= (time lhs, time rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		/// \brief	Greater-than operator.
		[[nodiscard]]
		friend constexpr bool operator >  (time lhs, time rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator >= (time lhs, time rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}
	};

	/// \brief	Prints a time out to a stream as `HH:MM:SS.FFFFFF` (per RFC 3339).
	/// \detail \cpp
	/// std::cout << toml::time{ 10, 20, 34 } << std::endl;
	/// std::cout << toml::time{ 10, 20, 34, 500000000 } << std::endl;
	/// \ecpp
	/// 
	/// \out
	/// 10:20:34
	/// 10:20:34.5
	/// \eout
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	/// \brief	A timezone offset.
	struct TOML_TRIVIAL_ABI time_offset final
	{
		/// \brief	Offset from UTC+0, in minutes.
		int16_t minutes;

		/// \brief	Default-constructs a zero time-offset.
		TOML_NODISCARD_CTOR
		constexpr time_offset() noexcept
			: minutes{}
		{}

		/// \brief	Constructs a timezone offset from separate hour and minute totals.
		///
		/// \detail \cpp
		/// std::cout << toml::time_offset{ 2, 30 } << std::endl;
		/// std::cout << toml::time_offset{ -2, 30 } << std::endl;
		/// std::cout << toml::time_offset{ -2, -30 } << std::endl;
		/// std::cout << toml::time_offset{ 0, 0 } << std::endl;
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
		/// \param 	h  	The total hours.
		/// \param 	m	The total minutes.
		TOML_NODISCARD_CTOR
		constexpr time_offset(int8_t h, int8_t m) noexcept
			: minutes{ static_cast<int16_t>(h * 60 + m) }
		{}

		/// \brief	Equality operator.
		[[nodiscard]]
		friend constexpr bool operator == (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		/// \brief	Inequality operator.
		[[nodiscard]]
		friend constexpr bool operator != (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		/// \brief	Less-than operator.
		[[nodiscard]]
		friend constexpr bool operator <  (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes < rhs.minutes;
		}

		/// \brief	Less-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator <= (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes <= rhs.minutes;
		}

		/// \brief	Greater-than operator.
		[[nodiscard]]
		friend constexpr bool operator >  (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes > rhs.minutes;
		}

		/// \brief	Greater-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator >= (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes >= rhs.minutes;
		}
	};

	/// \brief	Prints a time_offset out to a stream as `+-HH:MM or Z` (per RFC 3339).
	/// \detail \cpp
	/// std::cout << toml::time_offset{ 2, 30 } << std::endl;
	/// std::cout << toml::time_offset{ 2, -30 } << std::endl;
	/// std::cout << toml::time_offset{} << std::endl;
	/// std::cout << toml::time_offset{ -2, 30 } << std::endl;
	/// std::cout << toml::time_offset{ -2, -30 } << std::endl;
	/// \ecpp
	/// 
	/// \out
	/// +02:30
	/// +01:30
	/// Z
	/// -01:30
	/// -02:30
	/// \eout
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time_offset& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if TOML_ABI_NAMESPACES
		#ifdef TOML_OPTIONAL_TYPE
			inline namespace abi_custopt {
		#else
			inline namespace abi_stdopt {
		#endif
	#endif

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
		optional<toml::time_offset> time_offset;

		/// \brief	Default-constructs a zero date-time.
		TOML_NODISCARD_CTOR
		constexpr date_time() noexcept
			: date{},
			time{}
		{}

		/// \brief	Constructs a local date-time.
		///
		/// \param 	d	The date component.
		/// \param 	t	The time component.
		TOML_NODISCARD_CTOR
		constexpr date_time(toml::date d, toml::time t) noexcept
			: date{ d },
			time{ t }
		{}

		/// \brief	Constructs an offset date-time.
		///
		/// \param 	d	  	The date component.
		/// \param 	t	  	The time component.
		/// \param 	offset	The timezone offset.
		TOML_NODISCARD_CTOR
			constexpr date_time(toml::date d, toml::time t, toml::time_offset offset) noexcept
			: date{ d },
			time{ t },
			time_offset{ offset }
		{}

		/// \brief	Returns true if this date_time does not contain timezone offset information.
		[[nodiscard]]
		constexpr bool is_local() const noexcept
		{
			return !time_offset.has_value();
		}

		/// \brief	Equality operator.
		[[nodiscard]]
		friend constexpr bool operator == (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date
				&& lhs.time == rhs.time
				&& lhs.time_offset == rhs.time_offset;
		}

		/// \brief	Inequality operator.
		[[nodiscard]]
		friend constexpr bool operator != (const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// \brief	Less-than operator.
		[[nodiscard]]
		friend constexpr bool operator <  (const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.time_offset < rhs.time_offset;
		}

		/// \brief	Less-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator <= (const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.time_offset <= rhs.time_offset;
		}

		/// \brief	Greater-than operator.
		[[nodiscard]]
		friend constexpr bool operator >  (const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs <= rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		[[nodiscard]]
		friend constexpr bool operator >= (const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs < rhs);
		}
	};

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_OPTIONAL_TYPE
	#endif

	/// \brief	Prints a date_time out to a stream in RFC 3339 format.
	/// \detail \cpp
	/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 } } << std::endl;
	/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, { -2, -30 } } << std::endl;
	/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, {} } << std::endl;
	/// \ecpp
	/// 
	/// \out
	/// 1987-03-16T10:20:34
	/// 1987-03-16T10:20:34-02:30
	/// 1987-03-16T10:20:34Z
	/// \eout
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date_time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}
}


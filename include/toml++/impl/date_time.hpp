//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.hpp"
#include "print_to_stream.hpp"
#include "std_optional.hpp"
#include "header_start.hpp"

#if TOML_CPP >= 20
TOML_IMPL_NAMESPACE_START
{
	template <typename ToDuration, typename FromDuration>
	TOML_NODISCARD
	constexpr optional<ToDuration> duration_cast_checked(FromDuration value) noexcept
	{
		using to_rep				  = typename ToDuration::rep;
		const long double seconds	  = static_cast<long double>(value.count())
									  * static_cast<long double>(FromDuration::period::num)
									  / static_cast<long double>(FromDuration::period::den);
		const long double min_seconds = static_cast<long double>((std::numeric_limits<to_rep>::lowest)())
									  * static_cast<long double>(ToDuration::period::num)
									  / static_cast<long double>(ToDuration::period::den);
		const long double max_seconds = static_cast<long double>((std::numeric_limits<to_rep>::max)())
									  * static_cast<long double>(ToDuration::period::num)
									  / static_cast<long double>(ToDuration::period::den);
		if (seconds < min_seconds || seconds > max_seconds)
			return {};
		return std::chrono::duration_cast<ToDuration>(value);
	}
}
TOML_IMPL_NAMESPACE_END;
#endif

TOML_NAMESPACE_START
{
	/// \brief	A local date.
	///
	/// \remarks The fields are not validated. Formatters print them as they are, so a field outside
	/// 		 its range prints as invalid TOML.
	struct TOML_TRIVIAL_ABI date
	{
		/// \brief	The year component.
		uint16_t year;

		/// \brief	The month component, from 1 - 12.
		uint8_t month;

		/// \brief	The day component, from 1 - 31.
		uint8_t day;

		/// \brief Default constructor. Does not initialize the members.
		TOML_NODISCARD_CTOR
		date() noexcept = default;

		/// \brief Constructs a date from individual date component values.
		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<Y, M, D>), typename Y, typename M, typename D)
		TOML_NODISCARD_CTOR
		constexpr date(Y y, M m, D d) noexcept //
			: year{ static_cast<uint16_t>(y) },
			  month{ static_cast<uint8_t>(m) },
			  day{ static_cast<uint8_t>(d) }
		{}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief Constructs a date from a year_month_day value.
		TOML_NODISCARD_CTOR
		constexpr date(std::chrono::year_month_day value) noexcept
			: year{ static_cast<uint16_t>(static_cast<int>(value.year())) },
			  month{ static_cast<uint8_t>(static_cast<unsigned>(value.month())) },
			  day{ static_cast<uint8_t>(static_cast<unsigned>(value.day())) }
		{}

		/// \brief Constructs a date from year/month/day values.
		TOML_NODISCARD_CTOR
		constexpr date(std::chrono::year y, std::chrono::month m, std::chrono::day d) noexcept
			: date{ std::chrono::year_month_day{ y, m, d } }
		{}
#endif

		/// \brief	Equality operator.
		TOML_PURE_GETTER
		friend constexpr bool operator==(const date& lhs, const date& rhs) noexcept
		{
			return lhs.year == rhs.year	  //
				&& lhs.month == rhs.month //
				&& lhs.day == rhs.day;
		}

		/// \brief	Inequality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const date& lhs, const date& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:
		/// \cond

		TOML_PURE_GETTER
		static constexpr uint32_t pack(const date& d) noexcept
		{
			return (static_cast<uint32_t>(d.year) << 16) | (static_cast<uint32_t>(d.month) << 8)
				 | static_cast<uint32_t>(d.day);
		}

		/// \endcond

	  public:
		/// \brief	Less-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		/// \brief	Less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<=(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		/// \brief	Greater-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>=(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief	Converts the date to a std::chrono::year_month_day.
		TOML_PURE_GETTER
		constexpr std::chrono::year_month_day to_chrono() const noexcept
		{
			return { std::chrono::year{ static_cast<int>(year) },
					 std::chrono::month{ month },
					 std::chrono::day{ day } };
		}
#endif

		/// \brief	Prints a date out to a stream as `YYYY-MM-DD` (per RFC 3339).
		///
		/// \detail \cpp
		/// std::cout << toml::date{ 1987, 3, 16 } << "\n";
		/// \ecpp
		///
		/// \out
		/// 1987-03-16
		/// \eout
		friend std::ostream& operator<<(std::ostream& lhs, const date& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	/// \brief	A local time-of-day.
	///
	/// \remarks The fields are not validated. Formatters print them as they are, so a field outside
	/// 		 its range prints as invalid TOML.
	struct TOML_TRIVIAL_ABI time
	{
		/// \brief	The hour component, from 0 - 23.
		uint8_t hour;

		/// \brief	The minute component, from 0 - 59.
		uint8_t minute;

		/// \brief	The second component, from 0 - 59, or 60 for a leap second.
		uint8_t second;

		/// \brief	The fractional nanoseconds component, from 0 - 999999999.
		uint32_t nanosecond;

		/// \brief Default constructor. Does not initialize the members.
		TOML_NODISCARD_CTOR
		time() noexcept = default;

		/// \brief Constructs a time from individual time component values.
		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M, S, NS>),
								  typename H,
								  typename M,
								  typename S  = uint8_t,
								  typename NS = uint32_t)
		TOML_NODISCARD_CTOR
		constexpr time(H h, M m, S s = S{}, NS ns = NS{}) noexcept //
			: hour{ static_cast<uint8_t>(h) },
			  minute{ static_cast<uint8_t>(m) },
			  second{ static_cast<uint8_t>(s) },
			  nanosecond{ static_cast<uint32_t>(ns) }
		{}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief Constructs a time from a hh_mm_ss value.
		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr time(std::chrono::hh_mm_ss<Duration> value) noexcept
			: hour{ static_cast<uint8_t>(value.hours().count()) },
			  minute{ static_cast<uint8_t>(value.minutes().count()) },
			  second{ static_cast<uint8_t>(value.seconds().count()) },
			  nanosecond{ static_cast<uint32_t>(
				  std::chrono::duration_cast<std::chrono::nanoseconds>(value.subseconds()).count()) }
		{}
#endif

		/// \brief	Equality operator.
		TOML_PURE_GETTER
		friend constexpr bool operator==(const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour		//
				&& lhs.minute == rhs.minute //
				&& lhs.second == rhs.second //
				&& lhs.nanosecond == rhs.nanosecond;
		}

		/// \brief	Inequality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:
		/// \cond

		TOML_PURE_GETTER
		static constexpr uint64_t pack(const time& t) noexcept
		{
			return static_cast<uint64_t>(t.hour) << 48 | static_cast<uint64_t>(t.minute) << 40
				 | static_cast<uint64_t>(t.second) << 32 | static_cast<uint64_t>(t.nanosecond);
		}

		/// \endcond

	  public:
		/// \brief	Less-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		/// \brief	Less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<=(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		/// \brief	Greater-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>=(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief	Converts the time to a std::chrono::hh_mm_ss.
		///
		/// \tparam	Duration	The duration type of the result. Finer parts are truncated.
		///
		/// \returns	The time of day, or an empty optional if it is out of range for Duration.
		template <typename Duration = std::chrono::nanoseconds>
		TOML_PURE_GETTER
		constexpr optional<std::chrono::hh_mm_ss<Duration>> to_chrono() const noexcept
		{
			const auto duration = std::chrono::hours{ hour } + std::chrono::minutes{ minute }
								+ std::chrono::seconds{ second } + std::chrono::nanoseconds{ nanosecond };
			if (auto value = impl::duration_cast_checked<Duration>(duration))
				return std::chrono::hh_mm_ss<Duration>{ *value };
			return {};
		}
#endif

		/// \brief	Prints a time out to a stream as `HH:MM:SS.FFFFFF` (per RFC 3339).
		/// \detail \cpp
		/// std::cout << toml::time{ 10, 20, 34 } << "\n";
		/// std::cout << toml::time{ 10, 20, 34, 500000000 } << "\n";
		/// \ecpp
		///
		/// \out
		/// 10:20:34
		/// 10:20:34.5
		/// \eout
		friend std::ostream& operator<<(std::ostream& lhs, const time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	/// \brief	A timezone offset.
	///
	/// \remarks The offset is not validated. Formatters print it as it is, so an offset outside
	/// 		 its range prints as invalid TOML.
	struct TOML_TRIVIAL_ABI time_offset
	{
		/// \brief	Offset from UTC+0, in minutes, from -1439 to 1439.
		int16_t minutes;

		/// \brief	Default constructor. Does not initialize the members.
		TOML_NODISCARD_CTOR
		time_offset() noexcept = default;

		/// \brief	Constructs a timezone offset from individual hour and minute totals.
		///
		/// \detail \cpp
		/// std::cout << toml::time_offset{ 2, 30 } << "\n";
		/// std::cout << toml::time_offset{ -2, 30 } << "\n";
		/// std::cout << toml::time_offset{ -2, -30 } << "\n";
		/// std::cout << toml::time_offset{ 0, 0 } << "\n";
		/// \ecpp
		///
		/// \out
		/// +02:30
		/// -01:30
		/// -02:30
		/// Z
		/// \eout
		///
		/// \tparam H	An integral type.
		/// \tparam M	An integral type.
		///
		/// \param 	h  	The total hours.
		/// \param 	m	The total minutes.
		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M>), typename H, typename M)
		TOML_NODISCARD_CTOR
		constexpr time_offset(H h, M m) noexcept //
			: minutes{ static_cast<int16_t>(
				  (static_cast<impl::common_signed_type<H, M>>(h) * impl::common_signed_type<H, M>{ 60 })
				  + static_cast<impl::common_signed_type<H, M>>(m)) }
		{}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief Constructs a time_offset from hours/minutes values.
		///
		/// \param 	h  	The total hours.
		/// \param 	m	The total minutes.
		TOML_NODISCARD_CTOR
		constexpr time_offset(std::chrono::hours h, std::chrono::minutes m) noexcept //
			: time_offset{ h + m }
		{}

		/// \brief Constructs a time_offset from a minutes value.
		///
		/// \param 	m	The total minutes.
		TOML_NODISCARD_CTOR
		constexpr time_offset(std::chrono::minutes m) noexcept : minutes{ static_cast<int16_t>(m.count()) }
		{}
#endif

		/// \brief	Equality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator==(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		/// \brief	Inequality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		/// \brief	Less-than operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator<(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes < rhs.minutes;
		}

		/// \brief	Less-than-or-equal-to operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator<=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes <= rhs.minutes;
		}

		/// \brief	Greater-than operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes > rhs.minutes;
		}

		/// \brief	Greater-than-or-equal-to operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes >= rhs.minutes;
		}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief	Converts the offset to std::chrono::minutes.
		TOML_PURE_GETTER
		constexpr std::chrono::minutes to_chrono() const noexcept
		{
			return std::chrono::minutes{ minutes };
		}
#endif

		/// \brief	Prints a time_offset out to a stream as `+-HH:MM or Z` (per RFC 3339).
		/// \detail \cpp
		/// std::cout << toml::time_offset{ 2, 30 } << "\n";
		/// std::cout << toml::time_offset{ 2, -30 } << "\n";
		/// std::cout << toml::time_offset{} << "\n";
		/// std::cout << toml::time_offset{ -2, 30 } << "\n";
		/// std::cout << toml::time_offset{ -2, -30 } << "\n";
		/// \ecpp
		///
		/// \out
		/// +02:30
		/// +01:30
		/// Z
		/// -01:30
		/// -02:30
		/// \eout
		friend std::ostream& operator<<(std::ostream& lhs, const time_offset& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);

	/// \brief	A date-time.
	struct date_time
	{
		/// \brief	The date component.
		toml::date date;

		/// \brief	The time component.
		toml::time time;

		/// \brief	The timezone offset component.
		///
		/// \remarks The date_time is said to be 'local' if the offset is empty.
		optional<toml::time_offset> offset;

		/// \brief	Default constructor. Does not initialize the members.
		TOML_NODISCARD_CTOR
		date_time() noexcept = default;

		/// \brief	Constructs a local date-time.
		///
		/// \param 	d	The date component.
		/// \param 	t	The time component.
		TOML_NODISCARD_CTOR
		constexpr date_time(const toml::date& d, const toml::time& t) noexcept //
			: date{ d },
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		/// \brief	Constructs a local date-time.
		///
		/// \param 	d	The date component.
		TOML_NODISCARD_CTOR
		explicit constexpr date_time(const toml::date& d) noexcept //
			: date{ d },
			  time{},
			  offset{} // TINAE - icc bugfix
		{}

		/// \brief	Constructs a local date-time.
		///
		/// \param 	t	The time component.
		TOML_NODISCARD_CTOR
		explicit constexpr date_time(const toml::time& t) noexcept //
			: date{},
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		/// \brief	Constructs an offset date-time.
		///
		/// \param 	d	  	The date component.
		/// \param 	t	  	The time component.
		/// \param 	off	The timezone offset.
		TOML_NODISCARD_CTOR
		constexpr date_time(const toml::date& d, const toml::time& t, const toml::time_offset& off) noexcept
			: date{ d },
			  time{ t },
			  offset{ off }
		{}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief Constructs a date_time from year_month_day/hh_mm_ss values.
		///
		/// \param 	d  	The date component.
		/// \param 	t	The time component.
		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::year_month_day d, std::chrono::hh_mm_ss<Duration> t) noexcept
			: date_time{ toml::date{ d }, toml::time{ t } }
		{}

		/// \brief Constructs a date_time from a year_month_day value.
		///
		/// \param 	d  	The date component.
		TOML_NODISCARD_CTOR
		explicit constexpr date_time(std::chrono::year_month_day d) noexcept //
			: date_time{ toml::date{ d } }
		{}

		/// \brief Constructs a date_time from year_month_day/hh_mm_ss/minutes values.
		///
		/// \param 	d  	The date component.
		/// \param 	t	The time component.
		/// \param 	off	The timezone offset.
		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::year_month_day d,
							std::chrono::hh_mm_ss<Duration> t,
							std::chrono::minutes off) noexcept
			: date_time{ toml::date{ d }, toml::time{ t }, toml::time_offset{ off } }
		{}

		/// \brief Constructs a date_time from a local_time value.
		///
		/// \param 	dt  	The local_time component.
		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::local_time<Duration> dt) noexcept
			: date_time{ std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(dt) },
						 std::chrono::hh_mm_ss<Duration>{ dt - std::chrono::floor<std::chrono::days>(dt) } }
		{}

		/// \brief Constructs a date_time from a local_time value and a timezone offset.
		///
		/// \param 	dt  	The local_time component.
		/// \param 	off	The timezone offset.
		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::local_time<Duration> dt, std::chrono::minutes off) noexcept
			: date_time{ std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(dt) },
						 std::chrono::hh_mm_ss<Duration>{ dt - std::chrono::floor<std::chrono::days>(dt) },
						 off }
		{}
#endif

		/// \brief	Returns true if this date_time does not contain timezone offset information.
		TOML_PURE_INLINE_GETTER
		constexpr bool is_local() const noexcept
		{
			return !offset.has_value();
		}

		/// \brief	Equality operator.
		TOML_PURE_GETTER
		friend constexpr bool operator==(const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date //
				&& lhs.time == rhs.time //
				&& lhs.offset == rhs.offset;
		}

		/// \brief	Inequality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// \brief	Less-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset < rhs.offset;
		}

		/// \brief	Less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<=(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset <= rhs.offset;
		}

		/// \brief	Greater-than operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs <= rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs < rhs);
		}

#if TOML_DOXYGEN || TOML_CPP >= 20
		/// \brief	Converts the date and time to a std::chrono::local_time. The offset is ignored.
		///
		/// \tparam	Duration	The duration type of the result. Finer parts are truncated.
		///
		/// \returns	The local time point, or an empty optional if it is out of range for Duration.
		template <typename Duration = std::chrono::nanoseconds>
		TOML_PURE_GETTER
		constexpr optional<std::chrono::local_time<Duration>> to_chrono() const noexcept
		{
			const auto days			= std::chrono::local_days{ date.to_chrono() }.time_since_epoch();
			const auto time_value	= std::chrono::hours{ time.hour } + std::chrono::minutes{ time.minute }
									+ std::chrono::seconds{ time.second } + std::chrono::nanoseconds{ time.nanosecond };
			const auto day_duration = impl::duration_cast_checked<Duration>(days);
			const auto time_duration = impl::duration_cast_checked<Duration>(time_value);
			if (!day_duration || !time_duration
				|| day_duration->count()
					   > (std::numeric_limits<typename Duration::rep>::max)() - time_duration->count())
				return {};
			return std::chrono::local_time<Duration>{ *day_duration + *time_duration };
		}
#endif

		/// \brief	Prints a date_time out to a stream in RFC 3339 format.
		/// \detail \cpp
		/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 } } << "\n";
		/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, { -2, -30 } } << "\n";
		/// std::cout << toml::date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, {} } << "\n";
		/// \ecpp
		///
		/// \out
		/// 1987-03-16T10:20:34
		/// 1987-03-16T10:20:34-02:30
		/// 1987-03-16T10:20:34Z
		/// \eout
		friend std::ostream& operator<<(std::ostream& lhs, const date_time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END; // TOML_HAS_CUSTOM_OPTIONAL_TYPE
}
TOML_NAMESPACE_END;

#include "header_end.hpp"

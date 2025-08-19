//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_string.hpp"
#include "preprocessor.hpp"

TOML_NAMESPACE_START
{
	/// \brief	The type of a piece of TOML trivia.
	enum class TOML_CLOSED_ENUM trivia_type : uint8_t
	{
		whitespace,
		comment,
		trailing_comma
	};

	/// \brief	TOML trivia piece.
	///
	/// \detail	The trivia between two nodes can be separated into individual trivia pieces such as whitespace and comments.
	struct trivia_piece
	{
		/// \brief	The text representing the trivia.
		std::string text;
		/// \brief	The type of trivia.
		trivia_type type;
	};
}
TOML_NAMESPACE_END;

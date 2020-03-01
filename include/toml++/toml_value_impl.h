#pragma once
#include "toml_array.h"

#if TOML_IMPLEMENTATION

TOML_START
{
	template class value<string>;
	template class value<int64_t>;
	template class value<double>;
	template class value<bool>;
	template class value<date>;
	template class value<time>;
	template class value<date_time>;
}
TOML_END

#endif // TOML_IMPLEMENTATION

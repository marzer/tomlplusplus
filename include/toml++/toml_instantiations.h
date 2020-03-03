#pragma once
#include "toml_value.h"
#include "toml_node_view.h"

TOML_START
{
	template class TOML_API value<string>;
	template class TOML_API value<int64_t>;
	template class TOML_API value<double>;
	template class TOML_API value<bool>;
	template class TOML_API value<date>;
	template class TOML_API value<time>;
	template class TOML_API value<date_time>;

	template class TOML_API node_view<node>;
	template class TOML_API node_view<const node>;
}
TOML_END

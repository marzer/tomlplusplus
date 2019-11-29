#pragma once
#include "toml_common.h"

namespace TOML17_NAMESPACE
{
	class array final
		: public node
	{
		private:
			std::vector<std::shared_ptr<node>> values;
	};
}

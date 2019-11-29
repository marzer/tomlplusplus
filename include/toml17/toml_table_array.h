#pragma once
#include "toml_table.h"

namespace TOML17_NAMESPACE
{
	class table_array final
		: public node
	{
		private:
			std::vector<std::shared_ptr<table>> tables;
	};
}

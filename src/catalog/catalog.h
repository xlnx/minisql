#pragma once

#include <buffer/buffer_aux.h>

namespace minisql
{

namespace __catalog
{

class CatalogManager
{
public:
	CatalogManager();
	~CatalogManager();
};

}

using __catalog::CatalogManager;

}
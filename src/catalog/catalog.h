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

	static void createTable();

	static void lookup(int id);
};

}

using __catalog::CatalogManager;

}
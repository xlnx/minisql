#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <catalog/catalog.h>

// when this obj is linked, the minisql runtime is injected.
namespace minisql::__catalog
{

static CatalogManager dummy;

}

namespace minisql::__index
{

map<int,vector<int>> IndexManager::tablesToTrees;
map<int,int> IndexManager::treesToTables;
map<int, Item> IndexManager::idToTree;

static IndexManager dummy;

}

namespace minisql::__buffer
{

std::vector<File*> BufferManager::files;
heap<Block*> BufferManager::cachedBlocks;
OffType BufferManager::offindex;
ItemIndex BufferManager::erased;

static BufferManager dummy;

}

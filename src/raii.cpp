#include <buffer/buffer_aux.h>
#include <index/indexManager.h>
#include <api/api.h>
#include <catalog/catalog.h>
#include <index/bplusTree.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>

#define SE_FNAME(suffix) (std::string("./SSBsb3ZlIGtvaXNoaS1jaGFu") + suffix + ".dat")

// when this obj is linked, the minisql runtime is injected.
static void handler(int sig)
{
	debug::print::ln("See you next time! >w<");
	exit(0);
}

static volatile int _dummy_start = []
{
	puts("initializing minisql core...");
	if (signal(SIGINT, handler) == SIG_ERR)
	{
		throw std::logic_error("cannot set SIGFPE.");
	}
	return 0;
} ();

namespace minisql::__api
{

bool API::doPrint = true;

}

namespace minisql::__catalog
{

std::map<std::string, TableInfo> CatalogManager::tableInfos;
std::map<std::string, IndexInfo> CatalogManager::indexInfos;
std::fstream CatalogManager::icursor = []
{
	std::fstream fs(SE_FNAME("~"), std::ios::binary | std::ios::in | std::ios::out);
	if (!fs)
	{
		fs.open(SE_FNAME("~"), std::ios::binary | std::ios::trunc | std::ios::out);
		std::string s = "{\"table\":{},\"index\":{}}";
		int size = s.length();
		fs.write(reinterpret_cast<const char*>(&size), sizeof(int));
		fs.write(s.c_str(), s.length());
		fs.close();
		fs.open(SE_FNAME("~"), std::ios::binary | std::ios::in | std::ios::out);
	}
	return fs;
} ();

static CatalogManager dummy;

}

namespace minisql::__index
{

map<int,vector<int>> IndexManager::tablesToTrees;
map<int,int> IndexManager::treesToTables;
map<int, IndexManager::BPlusTree> IndexManager::idToTree;

static IndexManager dummy;

}

namespace minisql::__buffer
{

struct BufferManagerInfo
{
	SizeType numBufferTypes;
	ItemIndex erased;
};

std::vector<File*> BufferManager::files;
heap<Block*> BufferManager::cachedBlocks;
OffType BufferManager::offindex;
ItemIndex BufferManager::erased;
std::fstream BufferManager::icursor = []
{
	std::fstream cur;
	cur.open(SE_FNAME(""), std::ios::binary | std::ios::in | std::ios::out);
	if (!cur) 
	{
		cur.open(SE_FNAME(""), std::ios::binary | std::ios::trunc | std::ios::out);
		cur.seekp(0, std::ios::beg);
		BufferManagerInfo info;
		info.numBufferTypes = 0;
		info.erased = SQL_NULL;
		cur.write(reinterpret_cast<const char*>(&info), SizeOf(BufferManagerInfo));
		cur.close();
		cur.open(SE_FNAME(""), std::ios::binary | std::ios::in | std::ios::out);
	}
	return cur;
} ();

static BufferManager dummy;

}

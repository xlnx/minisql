#pragma once			// lexer.h

#include "lexer_initializer.h"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <sstream>


template <typename CharT = char>
class lexer
{
	lexer_initializer rules;
	using iterator = const CharT*;
	using string_type = std::basic_string<CharT>;
	string_type str;
	iterator iter;
	::std::vector<iterator> lines;
	static const string_type spaces;
public:
	struct token;
	using value_type = token;
	using position_type = std::pair<unsigned, unsigned>;
	struct token
	{
		long long id;
		string_type value;
		unsigned row, col;
	};
	std::set<long long> signs;
	struct exception_type
	{
		exception_type(const ::std::string &token, 
				unsigned row, unsigned col, const ::std::string &line, const ::std::string &reason = ""):
			token(token), row(row), col(col), line(line), reason(reason)
		{
		}
		::std::string token;
		unsigned row, col;
		::std::string line;
		::std::string reason;
		::std::string what() 
		{
			if (row != 0)
			{
				::std::ostringstream os;
				os << ":" << row << ":" << col << ": error: unexpected \'" << token << '\'' << (reason != "" ? " due to " : "") + reason << "\n"
					<< line << "\n";
				for (auto p = &line[0]; p != &line[0] + col; ++p)
				{
					os << (*p == '\t' ? "    " : " ");
				}
				os << "^";
				for (auto p = &token[1]; *p; ++p)
				{
					os << (*p == '\t' ? "~~~~" : "~");
				}
				return os.str();
			}
			else
			{
				return ": error: unexpected EOF";
			}
		}
	};
public:
	template <class... T, typename = typename
			std::enable_if<
				tmp_and<
					std::is_constructible<
						lexer_init_element, T
					>::value...
				>::value
			>::type
		>
		lexer(const T&... args):
			lexer(lexer_initializer(args...)) {}
	lexer(const lexer_initializer& list):
		rules(list), iter(&str[0])
	{ for (auto& elem: list){
		signs.insert(elem.value);
	} }
	virtual ~lexer() = default;
public:
	void reset() { str = ""; iter = &str[0]; }
	bool empty() const { return !*iter; }
	lexer& operator << (string_type&& src)
	{
		auto diff = iter - &str[0];
		str += std::forward<string_type>(src) + "\n";
		iter = &str[0] + diff;
		return *this;
	}
	void operator <= (string_type&& src)
	{
		str = std::forward<string_type>(src);
		iter = &str[0];
		lines.push_back(iter);
		while (*iter && spaces.find_first_of(*iter) != string_type::npos)
				if (*iter++ == '\n') lines.push_back(iter); 
	}
	value_type next()
	{
		static auto str_val = "string"_t.value;

		// static std::regex id("[A-Za-z_]\\w*", std::regex::nosubs | std::regex::optimize);
		static auto id_val = "id"_t.value;

		static std::set<std::string> keys = {"insert", "into", "values", "select", 
			"from", "where", "and", "quit", "execfile", "create", "table", "unique", 
			"primary", "key", "drop", "delete", "index", "on", "show", "tables", 
			"indexes", "int", "float", "char"};

		static std::regex num("\\d*\\.\\d+|\\d+\\.\\d*|\\d+(?:[eE]-?\\d+)?", std::regex::nosubs | std::regex::optimize);
		static auto num_val = "number"_t.value;

		if (*iter)
		{
			if (*iter == '"' || *iter == '\'' || *iter == '`')
			{
				char c = *iter;
				auto it = iter + 1;
				while (*it && *it != c)
				{
					it++;
				}
				if (*it)
				{
					it++;
					unsigned row = lines.size() - 1;
					std::swap(iter, it);
					auto q = iter;
					while (*iter && spaces.find_first_of(*iter) != string_type::npos)
							if (*iter++ == '\n') lines.push_back(iter); 
					return { str_val, std::string(it + 1, q - 1), row, unsigned(q - &lines[row][0]) };
				}
			}
			else if (*iter >= 'a' && *iter <= 'z' || *iter >= 'A' && *iter <= 'Z')
			{
				auto it = iter + 1;
				while (*it >= 'a' && *it <= 'z' || *it >= 'A' && *it <= 'Z' || *it >= '0' && *it <= '9')
				{
					it++;
				}
				auto len = it - iter;
				std::string res(iter, it);
				std::transform(iter, it, res.begin(), ::tolower);
				unsigned row = lines.size() - 1;
				std::swap(iter, it);
				while (*iter && spaces.find_first_of(*iter) != string_type::npos)
						if (*iter++ == '\n') lines.push_back(iter); 
				if (keys.count(res))
				{
					auto val = operator ""_t(res.c_str(), len).value;
					return { val, std::move(res), row, unsigned(it - &lines[row][0]) };
				}
				else
				{
					return { id_val, std::move(res), row, unsigned(it - &lines[row][0]) };
				}
			}
			else if (*iter >= '0' && *iter <= '9')
			{
				auto it = iter;
				while (*it >= '0' && *it <= '9' || *it == '.')
				{
					it++;
				}
				std::string res(iter, it);
				unsigned row = lines.size() - 1;
				std::swap(iter, it);
				while (*iter && spaces.find_first_of(*iter) != string_type::npos)
						if (*iter++ == '\n') lines.push_back(iter); 
				return { num_val, std::move(res), row, unsigned(it - &lines[row][0]) };
			}
			else
			{
				auto len = 1;
				switch (*iter)
				{
					case '>': {
						if (iter[1] == '=')
						{
							len = 2;
						}
					} break;
					case '<': {
						if (iter[1] == '=' || iter[1] == '>') 
						{
							len = 2;
						}
					} break;
					case '!': {
						if (iter[1] == '=')
						{
							len = 2;
						}
					} break;
					case '&': {
						if (iter[1] == *iter)
						{
							len = 2;
						}
					} break;
				}
				std::string res(iter, iter + len);
				unsigned row = lines.size() - 1;
				auto it = iter;
				iter += len;
				while (*iter && spaces.find_first_of(*iter) != string_type::npos)
						if (*iter++ == '\n') lines.push_back(iter); 
				auto val = operator ""_t(res.c_str(), len).value;
				return { val, std::move(res), row, unsigned(it - &lines[row][0]) };
			}
			auto itr = iter;
			auto iter_ln = itr;
			while (*iter_ln && *iter_ln != '\n') ++ iter_ln;
			iter = &str[str.length()];
			throw exception_type(string_type(itr, unsigned(iter_ln - itr)), 
				lines.size(), unsigned(itr - &lines.back()[0]), 
				string_type(lines.back(), iter_ln), "unknown stray");
			// throw std::bad_cast();
		}
		else
		{
			throw std::bad_cast();
		}
	}
	[[noreturn]] void handle_exception()
	{
		throw exception_type(
			"EOF", 0, 0, "", "source incomplete"
		);
	}
	[[noreturn]] void handle_exception(const token &tok, const std::string &reason = "")
	{
		auto p = lines[tok.row];
		while (*p && *p != '\n')
				++p;
		throw exception_type(
			tok.value, tok.row + 1, tok.col, string_type(lines[tok.row], p), reason
		);
	}
};
template <typename CharT>
const typename lexer<CharT>::string_type lexer<CharT>::spaces = " \t\r\n";

template <typename AstTy, typename CharT = char>
class reflected_lexer: public lexer<CharT>
{
	using reflected_lexer_initializer = initializer<
		reflected_lexer_init_element<AstTy, CharT>>;
	reflected_lexer_initializer list;
public:
	std::unordered_map<long long, lexer_callback<AstTy, CharT>> handlers;
	template <class... T, typename = typename
			std::enable_if<
				tmp_and<
					std::is_constructible<
						reflected_lexer_init_element<AstTy, CharT>, T
					>::value...
				>::value
			>::type
		>
		reflected_lexer(const T&... args):
			lexer<CharT>(reflected_lexer_init_element<AstTy, CharT>(args).elem...),
			list({reflected_lexer_init_element<AstTy, CharT>(args)...})
		{ for (auto& reflect: list){
			handlers[reflect.elem.value] = reflect.handler;
		}}
	reflected_lexer(const initializer<reflected_lexer_init_element<AstTy, CharT>> &l):
		lexer<CharT>(gen(l)),
		list(l)
	{ for (auto& reflect: list){
		handlers[reflect.elem.value] = reflect.handler;
	}}
private:
	lexer_initializer gen(const initializer<reflected_lexer_init_element<AstTy, CharT>> &ls)
	{ lexer_initializer ll; for (auto &e: ls) ll.push_back(e.elem); return ll; }
};

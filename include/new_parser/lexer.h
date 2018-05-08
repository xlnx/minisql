#pragma once			// lexer.h
#include "lexer_initializer.h"
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
				unsigned row, unsigned col, const ::std::string &line):
			token(token), row(row), col(col), line(line)
		{
		}
		::std::string token;
		unsigned row, col;
		::std::string line;
		::std::string what() 
		{
			if (row != 0)
			{
				::std::ostringstream os;
				os << ":" << row << ":" << col << ": error: unexpected \'" << token << "\'\n"
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
		if (*iter)
		{
			std::match_results<const CharT*> result;
			for (const auto& rule: rules)
			{
				if (std::regex_search(iter, result, rule.mode, std::regex_constants::match_continuous))
				{
					unsigned row = lines.size() - 1;
					auto q = iter;
					while (iter != result.suffix().first)
							if (*iter++ == '\n') lines.push_back(iter); 
					while (*iter && spaces.find_first_of(*iter) != string_type::npos)
							if (*iter++ == '\n') lines.push_back(iter); 
					string_type res = result[0];
					return { rule.value, res, row, unsigned(q - &lines[row][0]) };
				}
			}
			auto itr = iter;
			auto iter_ln = itr;
			while (*iter_ln && *iter_ln != '\n') ++ iter_ln;
			iter = &str[str.length()];
			throw exception_type(string_type(itr, unsigned(iter_ln - itr)), 
				lines.size(), unsigned(itr - &lines.back()[0]), 
				string_type(lines.back(), iter_ln));
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
			"EOF", 0, 0, ""
		);
	}
	[[noreturn]] void handle_exception(const token &tok)
	{
		auto p = lines[tok.row];
		while (*p && *p != '\n')
				++p;
		throw exception_type(
			tok.value, tok.row + 1, tok.col, string_type(lines[tok.row], p)
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
	std::map<long long, lexer_callback<AstTy, CharT>> handlers;
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

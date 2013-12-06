#include <string>
#include "utility.h"
using namespace std;

string str2filename(string str)
{
	if (str.size()>=2) return str;
	if ( 65 <= str[0] && str[0] <= 90) return string("_")+str;
	if (str[0] == '\\') return string("_右斜");
	if (str[0] == '/') return string("_左斜");
	if (str[0] == ':') return string("_冒号");
	if (str[0] == '*') return string("_星号");
	if (str[0] == '?') return string("_问号");
	if (str[0] == '"') return string("_引号");
	if (str[0] == '<') return string("_小于");
	if (str[0] == '>') return string("_大于");
	if (str[0] == '|') return string("_或");
	return str;
}
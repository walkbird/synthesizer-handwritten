#include <string>
#include "utility.h"
using namespace std;

string str2filename(string str)
{
	if (str.size()>=2) return str;
	if ( 65 <= str[0] && str[0] <= 90) return string("_")+str;
	if (str[0] == '\\') return string("_��б");
	if (str[0] == '/') return string("_��б");
	if (str[0] == ':') return string("_ð��");
	if (str[0] == '*') return string("_�Ǻ�");
	if (str[0] == '?') return string("_�ʺ�");
	if (str[0] == '"') return string("_����");
	if (str[0] == '<') return string("_С��");
	if (str[0] == '>') return string("_����");
	if (str[0] == '|') return string("_��");
	return str;
}
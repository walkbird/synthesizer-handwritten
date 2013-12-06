#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class CONFIG{
public:
	string var2string(string val);
	int var2int(string val);
	double var2double(string val);
	bool load(string filename);
	bool write(string a,string b);
private:
	string configfile;

	map<string,string> name;
	
};

extern CONFIG config;

#endif
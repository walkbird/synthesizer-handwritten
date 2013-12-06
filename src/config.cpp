#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "config.h"
using namespace std;

CONFIG config;

bool CONFIG::load(string filename)
{
	configfile = filename;

	fstream fp;
	//读取的一行字符串
	string line;

	//一行字符串分割成三部分
	string left;
	string opt;
	string right;

	fp.open(filename.c_str(),ios::in);

	if (!fp){
		cout<<"Open file error!"<<endl;
		return false;
	}

	while( !fp.eof() ){
		
		getline(fp,line);
		istringstream ls(line);
		ls>>left>>opt>>right;
		if (left[0] != '#' && opt == "="){
			name[left] = right;
		}
	}
	return true;
}

string CONFIG::var2string(string val){
	map<string ,string >::iterator it = name.find(val);
	if (it != name.end()) return it->second;
	else return string();
}

int CONFIG::var2int(string val){
	int ret;
	map<string ,string >::iterator it = name.find(val);
	if (it != name.end()){
		istringstream str(it->second);
		str>>ret;
		return ret;
	}else{
		return 0;
	}
}

double CONFIG::var2double(string val){
	double ret;
	map<string ,string >::iterator it = name.find(val);
	if (it != name.end()){
		istringstream str(it->second);
		str>>ret;
		return ret;
	}else{
		return 0.0;
	}
}

bool CONFIG::write(string a,string b){
	fstream fp(configfile,ios::app);
	name["image_amt"] = b;
	string str(a + " = " + b + "\n");
	fp.write(str.c_str(),str.size());
	return true;
}
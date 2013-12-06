#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <numeric>
using namespace std;

vector<string> loadWord(string path)
{
	fstream fp ;
	vector<string> text ;
	set<string> inset;

	size_t index = 0 ;
	fp.open(path.c_str(),ios::in) ;
	if (!fp){
		cout<<"openfile error!"<<endl;
		return text;
	}
	while( !fp.eof() ) {
		fp.seekg(index, ios::beg) ;
		char wch[5] = "" ;
		string tmp ;
		fp.read( wch , 2 ) ;
		if( strlen(wch)==0 ) break ;

		//屏蔽不可见字符
		if( wch[0]<=32 && wch[0]>=0) index+=1;
		else if( wch[0]>=33 && wch[0]<=127 ) {
			tmp+=wch[0] ;
			if (!inset.count(tmp)){
				text.insert(text.end() , tmp ) ;
				inset.insert(tmp);
			}
			index+=1 ;
		}else {
			tmp+=wch ;
			if (!inset.count(tmp)){
				text.insert(text.end() , tmp ) ;
				inset.insert(tmp);
			}
			index+=2 ;
		}
	}
	//for( vector<string>::iterator it = text.begin() ; it!=text.end() ; it++ ) {
	//	cout<<*it<<endl ;
	//}
	sort(text.begin(),text.end());
	return text;
}

//每个或符号一个string
//包含空格和回车，回车用'\n'表示
vector<string> loadText(string path)
{
	fstream fp ;
	vector<string> text;
	size_t index = 0 ;

	fp.open(path.c_str(),ios::in) ;
	if (!fp){
		cout<<"openfile error!"<<endl;
		return text;
	}
	while( !fp.eof() ){
		fp.seekg(index, ios::beg) ;
		char wch[5] = "" ;
		string tmp ;
		fp.read( wch , 2 ) ;
		if( strlen(wch)==0 ) break ;
		if (wch[0]!= ' ' && wch[0]!= '\n' && wch[0]<32 && wch[0]>=0) index++;
		if( (wch[0]>=33 && wch[0]<=127) || wch[0]==' ' || wch[0]=='\n' ) {
			tmp+=wch[0] ;
			text.insert(text.end() , tmp ) ;
			if (wch[0]=='\n') index+=2;
			else index+=1 ;
		}else {
			tmp+=wch ;
			text.insert(text.end() , tmp ) ;
			index+=2 ;
		}
	}
	return text;
}

//写到文件，一行一字
bool writeWord(vector<string> &words,string path)
{
	fstream fp(path,ios::out);

	for (vector<string>::iterator it = words.begin();
		it!=words.end();++it){
		
			fp.write((*it).c_str(),it->size());
	}
	fp.close();
	return true;
}
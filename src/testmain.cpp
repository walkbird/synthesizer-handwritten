#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>
#include "pdfmanage.h"
#include "fileop.h"
#include "img.h"
#include "textM.h"
#include "config.h"
#include "wordlistM.h"
#include "utility.h"
using namespace std;

void createProj();
void loadProj();

int main()
{
	if (!config.load("config.ini")){
		system("pause");
		return 1;
	}

	cout<<"welcome to use handFont "<<config.var2string("version")<<" what do you want to do?"<<endl;
	cout<<"\t1.Create a project"<<endl;
	cout<<"\t2.Load a project"<<endl;
	cout<<"\n>>"<<flush;
	string mode;
	cin>>mode;
	while(!isdigit(mode[0])){
		cout<<"Input a digit!"<<endl;
		cout<<"\n>>"<<flush;
		cin>>mode;
	}
	if (mode[0]=='1'){
		createProj();
	}else if (mode[0]=='2'){
		loadProj();
	}

	system("pause");
	return 0;
}

void createProj(){
	string projname;
	bool input_again = true;
	while(input_again){
		cout<<"Set a new Project Name: "<<endl;
		cout<<"\n>>"<<flush;
		cin>>projname;

		fstream fp;
		fp.open(projname + "\\" + projname + ".ini",ios::in);
		if (fp){
			cout<<"ERROR: the Project alread exist!"<<endl;
			input_again = true;
		}else{
			input_again = false;
		}
	}

	fstream fp;
	fp.open(projname + "\\" + projname + ".ini",ios::out);
	fp.close();
	string test_filename;
	cout<<"Your text filename: "<<flush;
	cin>>test_filename;
	CONFIG proj;
	proj.load(projname + "\\" + projname + ".ini");
	proj.write("project_name",projname);
	proj.write("text_filename",test_filename);
	cout<<"Project Created!"<<endl;
}

void loadProj(){
	string projname;
	cout<<"Project Name: \n>>"<<flush;
	cin>>projname;
	string projMain(projname+"\\"+projname+".ini");

	CONFIG proj;
	if (!proj.load(projMain)){
		return;
	}

	string textfile(projname + "\\" + proj.var2string("text_filename"));

	if (textfile == projname + "\\"){
		cout<<"No input text file Specified!"<<endl;
		return;
	}

	cout<<"What to do ?"<<endl;
	cout<<"\t1.Create HandWrite input table."<<endl;
	cout<<"\t2.Load words from HandWrite input table."<<endl;
	cout<<"\t3.Transmit text file to HandWrite."<<endl;
	cout<<"\n>>"<<flush;

	string mode;
	cin>>mode;
	while(!isdigit(mode[0])){
		cout<<"Input a digit!"<<endl;
		cout<<"\n>>"<<flush;
		cin>>mode;
	}

	if (mode[0]=='1'){
		vector<string> wholetext = loadWord(textfile);
		vector<string> text;
		string wordlist_filename = projname + "\\" + config.var2string("wordlist_filename");
		fstream wordfp(wordlist_filename,ios::in);
		if (!wordfp){
			wordfp.close();

			string imageFolder = config.var2string("handWord_folder");
			for (vector<string>::iterator it = wholetext.begin();
				it!=wholetext.end();++it){
			
				fstream fp((imageFolder + str2filename(*it) + ".jpg").c_str(),ios::in);
				if (!fp){
					text.push_back(*it);
				}
			}
			writeWord(text,wordlist_filename);
		}else{
			wordfp.close();
		}
		text = loadWord(wordlist_filename);

		WORDLISTM pdf(projname + "\\" + projname + "_字模");
		
		pdf.write(text);
		cout<<"Table Created!"<<endl;

	}else if (mode[0]=='2'){

		//获取文字列表
		vector<string> text = loadWord(projname + "\\" + config.var2string("wordlist_filename"));
		vector<string>::iterator it = text.begin();
		cout<<text.size()<<" words waiting for load."<<endl;

		size_t amt = proj.var2int("image_amt");
		if (amt == 0){
			cout<<"How much images ?"<<endl;
			cout<<"\n>>"<<flush;
			cin>>amt;
			ostringstream ostr;
			ostr<<amt;
			proj.write("image_amt",ostr.str());
		}

		for (size_t no = 1;no!=amt+1;++no){
			ostringstream ostr;
			ostringstream onum;
			ostr<<"image_filename_"<<no<<flush;
			onum<<"image_wordamt_"<<no<<flush;

			string imgfile = proj.var2string(ostr.str());
			size_t num = proj.var2int(onum.str());
			if ( imgfile == "" ){
				cout<<"No."<<no<<" (filename) : "<<flush;
				cin>>imgfile;
				proj.write(ostr.str(),imgfile);
			}

			imgfile = projname + "\\" + imgfile;

			if (num == 0){
				cout<<"No."<<no<<" (wordAmount) : "<<flush;
				cin>>num;
				ostringstream tmp;
				tmp<<num;
				proj.write(onum.str(),tmp.str());
			}
			cout<<"processing image "<<no<<". Please wait..."<<endl;
			if (!writeImgProcess(imgfile,vector<string>(it,it+num))){
				;
			}else{
				cout<<num<<" words saved."<<endl;
			}

			it += num;
		}
		cout<<"done."<<endl;
		cout<<"Do you want to \"Transmit text file to HandWrite.\" ? (Y/N) " <<flush;
		string flag;
		cin>>flag;
		if (flag == "Y" || flag == "y"){
			mode = "3";
		}
	}
	if (mode[0]=='3'){
		cout<<"Save to \"*.pdf\" (\".pdf\" will add automatically): "<<endl;
		cout<<">>"<<flush;
		string pdfname;
		cin>>pdfname;
		vector<string> text = loadText(textfile);

		TEXTM pdf(projname + "\\" + pdfname.c_str());
		pdf.write(text);

		cout<<"done."<<endl;
	}
}
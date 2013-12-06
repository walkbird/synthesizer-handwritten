#ifndef TEXTM_H
#define TEXTM_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>
#include <sstream>
#include "hpdf.h"
#include "pdfmanage.h"
#include "config.h"
using namespace std;

class TEXTM:public PDFM{
public:
	TEXTM(string _fname):PDFM(_fname){
		setSize(config.var2double("text_wordsize"));
		setSpace(config.var2double("text_hspace"),config.var2double("text_vspace"));
	}
	void write(vector<string> &words);
private:
	string imageFolder;			//手写体存储文件名

	HPDF_REAL hstep;
	HPDF_REAL vstep;

	void _drawImage(string imgFilename, float x,float y);
};

#endif
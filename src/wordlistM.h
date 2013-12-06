#ifndef WORDLISTM_H
#define WORDLISTM_H

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

class WORDLISTM:public PDFM{
public:
	WORDLISTM(string _fname):PDFM(_fname),iwspace(2),boxSize(25),footerSize(10){
		setSize(config.var2double("wordlist_wordsize"),config.var2double("wordlist_boxsize"));
	}
	void setSize(HPDF_REAL _wordSize = 10,HPDF_REAL _boxSize =25){ wordSize = _wordSize; boxSize = _boxSize;}
	void write(vector<string> &words);
private:
	HPDF_REAL iwspace;
	HPDF_REAL boxSize;
		
	HPDF_REAL footerSize;

	void _writeAWord(HPDF_REAL x,HPDF_REAL y,string word);
	void _writeFooter(HPDF_UINT page_num,HPDF_UINT pages,HPDF_UINT word_amt,HPDF_UINT word_beg,HPDF_UINT word_end);
};

#endif
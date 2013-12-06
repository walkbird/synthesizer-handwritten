#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>
#include <sstream>
#include "wordlistM.h"
#include "pdfmanage.h"
#include "hpdf.h"
using namespace std;

void WORDLISTM::write(vector<string> &words){

	//字个数
	HPDF_UINT amt = words.size();
		
	HPDF_REAL hstep = boxSize + wordSize + iwspace + hspace;
	HPDF_REAL vstep = boxSize + vspace;
	HPDF_UINT cols = floor(width - board_left - board_right)/ceil(hstep);
	HPDF_UINT rows = floor(height - board_top - board_buttom)/ceil(vstep);

	HPDF_UINT pages = ceil((double)amt/(double)(cols*rows));

	HPDF_REAL posx = board_left;
	HPDF_REAL posy = height - board_top - boxSize;

	int cnt = 0;
	HPDF_UINT page_words;		//当前页字数
	HPDF_UINT page_words_beg;
	HPDF_UINT page_words_end;

	for (HPDF_UINT page_num = 1;cnt<amt && page_num <= pages ;++page_num){

		if (page_num!=1){
			page = HPDF_AddPage (pdf);
			HPDF_Page_SetLineWidth (page, LineWidth);
		}
		_setFont(wordSize);
		page_words = 0;
		page_words_beg = cnt+1;

		for (HPDF_UINT r = 1;cnt<amt && r <= rows; ++r){

			posy = height - board_top - r*vstep;

			for (HPDF_UINT c = 1;cnt<amt && c <= cols; ++c){

				posx = board_left + (c-1)*hstep;
				_writeAWord(posx,posy,words[cnt++]);
				++page_words;
			}
		}
		page_words_end = cnt;

		//页脚
		_writeFooter(page_num,pages,page_words,page_words_beg,page_words_end);

	}

	_save();

}

void WORDLISTM::_writeAWord(HPDF_REAL x,HPDF_REAL y,string word){
	HPDF_REAL plx = x;
	HPDF_REAL ply = (boxSize - wordSize)/2.0 + y;

	HPDF_Page_BeginText (page);
	HPDF_Page_TextOut (page, plx, ply, word.c_str());
	HPDF_Page_EndText (page);

	plx += wordSize + iwspace;
	ply = y;

	HPDF_Page_Rectangle (page, plx, ply, boxSize, boxSize);
	HPDF_Page_Stroke (page);
}

void WORDLISTM::_writeFooter(HPDF_UINT page_num,HPDF_UINT pages,HPDF_UINT word_amt,HPDF_UINT word_beg,HPDF_UINT word_end){
	
	ostringstream stream;
	stream<<"HandFont Alpha 手写体录入表";
	if (page_num != 0 && pages !=0){
		stream<<"( "<<page_num<<"/"<<pages<<" )"<<flush;
	}
	string str(stream.str());

	//HPDF_REAL tw = HPDF_Page_TextWidth(page,str.c_str());

	HPDF_UINT Panding_top = 0;

	_setFont(footerSize);

	HPDF_UINT posx = board_left;
	HPDF_UINT posy = board_buttom - footerSize - Panding_top;

	HPDF_Page_BeginText (page);
	HPDF_Page_TextOut (page, posx, posy, str.c_str());
	
	posy -= footerSize + vspace;
	stream.str("");
	stream<<"当前页字数："<<word_amt<<"( "<<word_beg<<" - "<<word_end<<" )"<<flush;
	str = stream.str();
	HPDF_Page_TextOut (page, posx, posy, str.c_str());

	HPDF_Page_EndText (page);

}
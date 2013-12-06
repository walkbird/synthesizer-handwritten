#ifndef PDFMANAGE_H
#define PDFMANAGE_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>
#include <sstream>
#include "hpdf.h"
using namespace std;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler (HPDF_STATUS   error_no,
				HPDF_STATUS   detail_no,
				void         *user_data);

class PDFM{
public:
	PDFM(string _fname):
	  fname(_fname+".pdf"),
	  wordSize(10),LineWidth(1),
	  board_left(50),board_right(50),board_top(50),board_buttom(50),
	  hspace(5),vspace(5)
	  { _initpdf(); }
	~PDFM(){ HPDF_Free (pdf); }
	void setFileName(string _fname){ fname = _fname+".pdf"; }
	void setSize(HPDF_REAL _wordSize = 10){wordSize = _wordSize;}
	void setSpace(HPDF_REAL h = 5,HPDF_REAL v=5){hspace = h,vspace=v;}
	void write(vector<string> &words);
protected:
	
	HPDF_Doc  pdf;				//pdf文件指针
	string fname;				//pdf文件名
	HPDF_Page page;
    HPDF_Font def_font;			//默认字体
    
	HPDF_REAL height;
    HPDF_REAL width;

	HPDF_REAL board_left;		//左边距
	HPDF_REAL board_right;		//右边距
	HPDF_REAL board_top;		//上边距
	HPDF_REAL board_buttom;		//下边距

	HPDF_REAL hspace;			//水平间距
	HPDF_REAL vspace;			//垂直间距

	HPDF_REAL LineWidth;
	HPDF_REAL wordSize;

	void _initpdf();
	void _save();
	void _setFont(HPDF_REAL _size = 16);
};

extern jmp_buf env;

#endif
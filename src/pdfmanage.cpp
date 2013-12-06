#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>
#include <sstream>
#include "pdfmanage.h"
#include "hpdf.h"
using namespace std;

jmp_buf env;		//¥ÌŒÛ¥¶¿Ì

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler (HPDF_STATUS   error_no,
				HPDF_STATUS   detail_no,
				void         *user_data)
{
	printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
				(HPDF_UINT)detail_no);
	longjmp(env, 1);
}



void PDFM::_initpdf(){
	pdf = HPDF_New (error_handler, NULL);
	if (!pdf) {
		printf ("error: cannot create PdfDoc object\n");
	}
	if (setjmp(env)) {
		HPDF_Free (pdf);
	}

	/* Init Chinese Fonts */
	HPDF_UseCNSFonts(pdf);
	HPDF_UseCNSEncodings (pdf);

	/* Add a new page object. */
	page = HPDF_AddPage (pdf);

	height = HPDF_Page_GetHeight (page);
	width = HPDF_Page_GetWidth (page);

	//fonts/line setting
	def_font = HPDF_GetFont (pdf,"SimSun","GBK-EUC-H");
	HPDF_Page_SetLineWidth (page, LineWidth);
	_setFont(wordSize);
}

void PDFM::_save(){
	HPDF_SaveToFile (pdf, fname.c_str());
}

void PDFM::_setFont(HPDF_REAL _size){
	HPDF_Page_SetFontAndSize (page, def_font, wordSize);
}
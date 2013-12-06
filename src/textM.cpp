#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "textM.h"
#include "pdfmanage.h"
#include "hpdf.h"
#include "config.h"
#include "utility.h"
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace cv;

void TEXTM::write(vector<string> &words)
{
	srand((unsigned)time(NULL));

	imageFolder = config.var2string("handWord_folder");

	//×Ö¸öÊý
	HPDF_UINT amt = words.size();
		
	hstep = wordSize + hspace;
	vstep = wordSize + vspace;

	HPDF_REAL posx = board_left;
	HPDF_REAL posy = height - board_top - wordSize;
	HPDF_REAL preDelt = 0;

	HPDF_UINT cnt = 0;

	for (HPDF_UINT page_num = 1;cnt<amt;++page_num){

		if (page_num!=1){
			page = HPDF_AddPage (pdf);
		}

		posy = height - board_top;

		for (HPDF_UINT r = 1;cnt<amt; ++r){

			posy -= vstep;

			if (posy < board_buttom) break;
			HPDF_REAL delt = config.var2double("text_random_indent")*(rand()%200-100)/100.0;
			posx = board_left - hstep + preDelt + delt;
			preDelt = delt;

			for (HPDF_UINT c = 1;cnt<amt; ++c){

				posx += hstep;

				if (posx > width - board_right) break;

				if (words[cnt][0]=='\n'){
					++cnt;
					break;
				}
				if (words[cnt][0]!=' '){
					if (config.var2int("text_english_word_follow") && (words[cnt][0]>=65 && words[cnt][0]<=90 || words[cnt][0]>=97 && words[cnt][0]<=122)){
						posx -= hstep;
						while(words[cnt][0]>=65 && words[cnt][0]<=90 || words[cnt][0]>=97 && words[cnt][0]<=122){
							posx += hstep;
							fstream fp;
							fp.open((imageFolder + str2filename(words[cnt]) + ".jpg").c_str(),ios::in);
							if (fp)
								_drawImage((str2filename(words[cnt])+".jpg").c_str(),posx,posy);
							fp.close();
							++cnt;
							
						}
					}else{
						fstream fp;
						fp.open((imageFolder + str2filename(words[cnt]) + ".jpg").c_str(),ios::in);
						if (fp)
							_drawImage((str2filename(words[cnt])+".jpg").c_str(),posx,posy);
						fp.close();
						++cnt;
						while (cnt<amt && (
							words[cnt][0] == '.' ||
							words[cnt][0] == ',' ||
							words[cnt][0] == ';' ||
							words[cnt][0] == ':' ||
							words[cnt][0] == '!' ||
							words[cnt][0] == '?' ||
							words[cnt][0] == ')' ||
							words[cnt][0] == ']' ||
							words[cnt][0] == '}' ||
							
							words[cnt] == "¡£" ||
							words[cnt] == "£¬" ||
							words[cnt] == "£º" ||
							words[cnt] == "£»" ||
							words[cnt] == "¡±" ||
							words[cnt] == "£¡" ||
							words[cnt] == "£¿" ||
							words[cnt] == "£©" 

							)){
							
							posx += hstep;
							fstream fp;
							fp.open((imageFolder + str2filename(words[cnt]) + ".jpg").c_str(),ios::in);
							if (fp)
								_drawImage((str2filename(words[cnt])+".jpg").c_str(),posx,posy);
							fp.close();

							++cnt;
						}
					}
				}else{
					++cnt;
				}
				
				
			}
		}

	}

	_save();
	return;
}

void TEXTM::_drawImage(string imgFilename, float x,float y)
{
	imgFilename = imageFolder + imgFilename;
	HPDF_Image image;

	image = HPDF_LoadJpegImageFromFile (pdf, imgFilename.c_str());
	HPDF_UINT h = HPDF_Image_GetHeight(image);
	HPDF_UINT w = HPDF_Image_GetWidth(image);
	double frate = min((double)wordSize/(double)w, (double)wordSize/(double)h);
	h = h*frate;
	w = w*frate;
	HPDF_Page_DrawImage (page, image, x, y, w, h);

	hstep = w + hspace;
}
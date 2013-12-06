#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cmath>
#include <cstring>
#include <list>
#include "img.h"
#include "config.h"
#include "utility.h"
using namespace cv;
using namespace std;

size_t boxWidth;
size_t diffWidth;
Mat gGray;
Mat gGray_backup;

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

inline bool inPloy(Point p,vector<Point> &square)
{
	vector<size_t> stat(4,0);
	for (vector<Point>::iterator it = square.begin();
		it != square.end(); ++it){
		size_t score = 0;
		if (p.x < it->x) score += 2;
		if (p.y < it->y) score += 1;
		if (stat[score]) return false;
		else stat[score] = true;
	}
	return true;
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();
    
    Mat pyr, timg, gray0(image.size(), CV_8U),gray;
	vector<double> area_list;
    
    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

	// find squares in gray plane of the image
	cvtColor(timg,gray0,CV_RGB2GRAY);
	adaptiveThreshold(gray0,gray,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,
		config.var2int("thres_blocksize"),
		config.var2double("thres_C"));

	gray.copyTo(gGray);

	//去除图像中的微小噪点
	if (config.var2int("dilate_erode_dots")){
		
		//备份gGray，用于提取文字
		gray.copyTo(gGray_backup);

		if (config.var2int("dilate_erode_dots_show"))
			imwrite("im_beforeDilate.jpg",gGray);
		
		//用于削弱噪点
		dilate(gGray, gGray, Mat(), Point(-1,-1),config.var2int("dilate_erode_dots_level"));
		
		if (config.var2int("dilate_erode_dots_show"))
			imwrite("im_afterDilate.jpg",gGray);
		
		//用于恢复被削弱的文字
		erode(gGray, gGray, Mat(), Point(-1,-1),config.var2int("dilate_erode_dots_level"));
		
		if (config.var2int("dilate_erode_dots_show"))
			imwrite("im_afterErode.jpg",gGray);
	}

	if (config.var2int("img_threshold_output"))
		imwrite(config.var2string("img_threshold_output_filename"),gray);

	//修补不完整的边框
	erode(gray, gray, Mat(), Point(-1,-1),config.var2int("erode_iterations"));
	
	if (config.var2int("img_erode_output"))
		imwrite(config.var2string("img_erode_output_filename"),gray);

    // find contours and store them all as a list
    findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    vector<Point> approx;

    // test each contour
    for( size_t i = 0; i < contours.size(); i++ )
    {
        // approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
		
        if( approx.size() == 4 ){
			double area = fabs(contourArea(Mat(approx)));
			if (area > config.var2double("contour_area_min") && area < config.var2double("contour_area_max") &&
				isContourConvex(Mat(approx)) )
			{
				double maxCosine = 0;

				for( int j = 2; j < 5; j++ )
				{
					// find the maximum cosine of the angle between joint edges
					double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
					maxCosine = MAX(maxCosine, cosine);
				}

				// if cosines of all angles are small
				// (all angles are ~90 degree) then write quandrange
				// vertices to resultant sequence
				if( maxCosine < config.var2double("Contour_maxConsine") ){
					Point cent(0,0);
					bool isInPoly = false;

					Scalar tsc;
					tsc = mean(approx);
					cent.x = (int)tsc.val[0];
					cent.y = (int)tsc.val[1];
					
					
					size_t no = 0;
					for (vector< vector<Point> >::iterator it = squares.begin();
						it != squares.end(); ++it){

						if (inPloy(cent,*it)){
							isInPoly = true;

							if (config.var2int("rect_getBigger"))
							if (area - area_list[no] > area*config.var2double("rect_rejectrate")){
								*it = approx;
								area_list[no] = area;
							}
							break;
						}
						++no;
					}
					

					if (!isInPoly){
						squares.push_back(approx);
						area_list.push_back(area);
					}
				}
			}
		}
    }
}


// the function draws all the squares in the image
void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
	
	Mat show;

    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    }
	resize(image,show,Size(image.size().width/8,image.size().height/8));
    
	if (config.var2int("img_rect_window")){
		namedWindow( config.var2string("img_rect_window_title"), 1 );
		imshow( config.var2string("img_rect_window_title"), show);
	}

	if (config.var2int("img_rect_output"))
		imwrite(config.var2string("img_rect_output_filename"),image);
}

Mat getWord(vector<Point> square)
{
	vector<size_t> idx(4,0);
	Scalar p;
	p = mean(square);
	for (size_t i = 0; i < 4; ++i){
		size_t score = 0;
		if (p.val[0] < square[i].x) score += 2;
		if (p.val[1] < square[i].y) score += 1;
		idx[score] = i;
	}
	size_t board = config.var2int("word_board");
	size_t baseup = min(square[idx[1]].y,square[idx[3]].y);
	size_t basebuttom = max(square[idx[0]].y,square[idx[2]].y);
	size_t baseleft = max(square[idx[0]].x,square[idx[1]].x);
	size_t baseright = min(square[idx[2]].x,square[idx[3]].x);

	size_t up = baseup - board;
	size_t buttom = basebuttom + board;
	size_t left = baseleft + board;
	size_t right = baseright - board;
	bool flag = false;

	//limit up
	if (config.var2int("word_limit_up")){
		flag = false;
		for (;up>buttom;--up){
			uchar *pt = gGray.data + gGray.cols * up;
			for (int k = left;k<=right;++k){
				if (*(pt + k)==0){
					flag = true;
					break;
				} 
			}
			if (flag) break;
		}
	}

	//limit buttom
	if (config.var2int("word_limit_buttom")){
		flag = false;
		for (;buttom<up;++buttom){
			uchar *pt = gGray.data + gGray.cols * buttom;
			for (int k = left;k<=right;++k){
				if (*(pt + k)==0){
					flag = true;
					break;
				} 
			}
			if (flag) break;
		}
	}

	//limit left
	if (config.var2int("word_limit_left")){
		flag = false;
		for (;left<right;++left){
			uchar *pt = gGray.data + left;
			for (size_t k = buttom;k<=up;++k){
				if (*(pt + k * gGray.cols)==0){
					flag = true;
					break;
				} 
			}
			if (flag) break;
		}
	}

	//limit right
	if (config.var2int("word_limit_right")){
		flag = false;
		for (;right>left;--right){
			uchar *pt = gGray.data + right;
			for (size_t k = buttom;k<=up;++k){
				if (*(pt + k * gGray.cols)==0){
					flag = true;
					break;
				}
			}
			if (flag) break;
		}
	}

	uchar *pt;
	Mat ret(up-buttom+1,right-left+1,CV_8U);
	for (size_t i=buttom;i<=up;++i){

		if (config.var2int("dilate_erode_dots")){
			//从备份的gGray中提取完整的文字
			pt = gGray_backup.data + gGray_backup.cols * i;
		}else{
			pt = gGray.data + gGray.cols * i;
		}

		for (size_t j=left;j<=right;++j){
			*(ret.data + (i-buttom) * ret.cols + (j-left)) = *(pt+j);
		}
	}
	return ret;
}

bool square_cmp(const vector<Point> &a,const vector<Point> &b )
{
	Scalar ta,tb;
	ta = mean(a);
	tb = mean(b);
	if (abs(ta.val[1]-tb.val[1]) < diffWidth){
		return ta.val[0] < tb.val[0];
	}else{
		return ta.val[1] < tb.val[1];
	}
}

bool writeImgProcess(string filename,vector<string> &wordlist)
{
    
    vector<vector<Point> > squares;

    Mat image = imread(filename, 1);
    if( image.empty() ){
        cout << "Couldn't load " << filename << endl;
        return false;
    }

    findSquares(image, squares);
	drawSquares(image, squares);

	boxWidth = max( abs(squares[0][0].x -  squares[0][1].x),abs(squares[0][0].x -  squares[0][2].x));
	diffWidth = boxWidth/3;

	sort(squares.begin(),squares.end(),square_cmp);

	if (squares.size() != wordlist.size()){
		cout<<"image is not good.only "<<squares.size()<<" words recognised! with "<<wordlist.size()<<" words!"<<endl;
		return false;
	}

	for (size_t imgno = 0;imgno<squares.size(); ++imgno){
		Mat wordImg = getWord(squares[imgno]);
		ostringstream ss;
		ss<<config.var2string("handWord_folder")<<str2filename(wordlist[imgno])<<".jpg"<<flush;
		if (!imwrite(ss.str().c_str(),wordImg))
			cout<<wordlist[imgno]<<"output error!"<<endl;
	}

    

	if (config.var2int("img_rect_window")){
		waitKey();
		destroyWindow( config.var2string("img_rect_window_title") );
	}
	return true;
}
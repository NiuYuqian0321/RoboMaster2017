/*
@author:NiuYuqian
@last date:2017-08-25

judge if it is the armor:
1. the s in hsv is high enough
2. the area of the contours is in a proper range
3. the area of the contours / the area of the rect of the contours is in a proper range
4. the rect is upright
5. the angle of the rect is small enough
6. the small side of the rect , the large side of the rect
7. the large side difference / the large sidr of the rect
8. the large side of the rect / the small side of the rect
9. the distance between two center point
10. the difference of the y of the two center point 
11. color


promotion1：only process 160-270 rows in the picture.
promotion2: to avoid the light is so small that can't be seen, find contours again after draw the light by size 2
promotion3: only choose the lowest point if there are more chariots
 */

#include "CArmorDetection.h"
#include <unistd.h>
#include <sys/time.h>

size_t stAreaMax = 1000;
size_t stAreaMin = 12;
int iSmallSideMax = 20;
int iLargeSideMax = 60;
float iLargeSideDiffRatio = 0.35;
double dRatioMax = 10;
double dRatioMin = 1.5;
double dAngleMax = 3;//3;//6;
double dDisLargeRatioMax = 4;
double dDisLargeRatioMin = 1.2;
double dAreaRatioMin = 0.5;
double dDisYMax = 25;
double dDisMin = 15;//??
double dDisMax = 120;
int thresh = 233;

extern int xBit;
extern int yBit;
extern float yaw;
extern float pitch;


#if ((defined MODE_SAVEVIDEOLOG) && (defined MODE_SHOWVIDEOLOG))
	CVideolog cVideolog(270,640,true,true,CV_8UC1, 1, 2,1);//show && save
#else
	CVideolog cVideolog(270,640,false,true,CV_8UC1, 1, 2,1);//save
#endif
	
	ofstream foutArmor("./Runlog/CArmor.txt");

	char szOpTxt[64]; // Output text, almost used in function CVideolog::AddText


int CArmorDetection::judgeColor(Mat rect)
{
	vector<Mat> channels_rgb;
	split(rect,channels_rgb);
	
	//b-1/2r-1/2g
	Mat color_white;
	addWeighted(channels_rgb[0],1,channels_rgb[2],-0.5,0,color_white);
	addWeighted(color_white,1,channels_rgb[1],-0.5,0,color_white);

	//threshold
	Mat whiteThresh;
	threshold(color_white,whiteThresh,30,255,CV_THRESH_BINARY);

	//findContours
	vector<vector<Point>> whiteContours;
	findContours(whiteThresh, whiteContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//the size of Contours
	double maxWhiteArea = 0;
	for(size_t i=0;i<whiteContours.size();i++)
	{
		maxWhiteArea = maxWhiteArea < whiteContours[i].size() ? whiteContours[i].size():maxWhiteArea;
	}

	sprintf(szOpTxt,"maxWhiteArea:  %f",maxWhiteArea);

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt);
#endif

	if(maxWhiteArea == 0) return RED;
	else return BLUE;
}
void CArmorDetection::DrawRotatedRect(Mat & mImg, RotatedRect cRotatedRect, int iDrawStrength)
{
	iDrawStrength = min(255, max(iDrawStrength, 0));

	Scalar tDraw(iDrawStrength);
	
	if (mImg.channels() == 3)
	{
		tDraw = Scalar(0, 0, iDrawStrength);
	}

	Point2f tCorner[4];
	cRotatedRect.points(tCorner);
	
	for (int i = 0; i < 4; i++)
	{
		line(mImg, tCorner[i], tCorner[(i + 1) % 4], tDraw);
	}
	circle(mImg, tCorner[0], 2, tDraw);
}
static double Timer()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((double(tv.tv_sec) * 1000) + (double(tv.tv_usec) / 1000));
}


int CArmorDetection::FindArmor(Mat mImg,Point& res)
{
	double dTimeStart = Timer();
	
	Rect ROI(0,160,640,270);
	mImg = mImg(ROI);

	int iMaxRect=1;

	Point res1(-1,-1);
	Point res2(-1,-1);

	Mat kernel(3,3,CV_8U,Scalar(1));
	
	//hsv
	Mat hsv;
	cvtColor(mImg, hsv, CV_BGR2HSV);
	vector<Mat> channels;
//7ms
	//cout << "Use time: " << Timer() - dTimeStart << endl;
	split(hsv,channels);
//8ms
	//threshold
	Mat v1;
	threshold(channels[2],v1,thresh,255,CV_THRESH_BINARY);
	//
	//imshow("v1",v1);
//8ms
	
	//morphology close
	//double dTimeStart = Timer();
	//Mat v2;
	//morphologyEx(v1,v2,MORPH_CLOSE,kernel);
	//imshow("v2",v2);
//17ms
	//cout << "Use time: " << Timer() - dTimeStart << endl;
	//findContours
	vector<vector<Point>> vContours;
	vContours.clear();
	findContours(v1, vContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat mContoursAll(v1.size(), CV_8UC1, Scalar(0));
	//18ms
	drawContours(mContoursAll, vContours, -1, Scalar(255), 2);
	//findContours again
	findContours(mContoursAll, vContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

//20ms

	//imshow("cout",mContoursAll);
//#ifdef MODE_SHOWVIDEOLOG
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	Mat mContoursAll2(v1.size(), CV_8UC1, Scalar(0));
	drawContours(mContoursAll2, vContours, -1, Scalar(255), 1);
	
#endif

	//Contous is ok
	sprintf(szOpTxt,"stAreaMin: %d stAreaMax: %d",stAreaMin,stAreaMax);

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt);
#endif
	foutArmor<<"stAreaMin: "<<stAreaMin<<"stAreaMax: "<<stAreaMax<<endl;
	foutArmor<<"dRatioMin: "<<dRatioMin<<endl;

	vector<Point> vContoursCentroid;
	if(vContours.size()!=0)
	{
		for (vector<vector<Point> >::iterator itrContours = vContours.begin();
			itrContours != vContours.end(); )
		{	
			//cout<<contourArea((*itrContours),false)<<endl;
			if( (contourArea((*itrContours),false)< stAreaMin) ||
				(contourArea((*itrContours),false)> stAreaMax))
			{
				itrContours = vContours.erase(itrContours);
			}
			else
			{
				//cout<<contourArea((*itrContours),false)<<endl;
				Moments cMom = moments(Mat(*itrContours));
				vContoursCentroid.push_back(Point(cMom.m10 / cMom.m00, cMom.m01 / cMom.m00));
				itrContours++;
			}
		}

	}
	//cout << "Use time: " << Timer() - dTimeStart << endl;
	//RotateRect
	vector<RotatedRect> vRectContours;
	for (size_t ui = 0; ui < vContours.size(); ui++)
	{
		RotatedRect cRect = minAreaRect(vContours[ui]);
		
		if( contourArea(vContours[ui],false) < dAreaRatioMin * cRect.size.area() )
			continue;
			
		if((fabs(cRect.angle)>70 && cRect.size.width > cRect.size.height)
		   ||(fabs(cRect.angle)<20 && cRect.size.width < cRect.size.height))
				vRectContours.push_back(cRect);
	}

	//RotatedRect is ok
	sprintf(szOpTxt,"dAngleMax: %.2f iLargeSideMax:%d  dAreaRatioMin:%.2f  dDisMin:%f  dDisMax:%.2f ",dAngleMax,iLargeSideMax,dAreaRatioMin,dDisMin,dDisMax);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt);
#endif
	foutArmor<<"dAngleMax: "<<dAngleMax<<"iLargeSideMax: "<<iLargeSideMax<<"dAreaRatioMin: "<<dAreaRatioMin<<"dDisMin: "<<dDisMin<<"dDisMax: "<<dDisMax<<endl;

	//20ms
	Rect r1 ;
	Rect r2;
	
	for (size_t ui1 = 0; ui1 < vRectContours.size(); ui1++)
	{

		for (size_t ui2 = ui1 + 1; ui2 < vRectContours.size(); ui2++)
		{
			
			if(fabs(vRectContours[ui2].angle)<10 && vRectContours[ui2].size.width > vRectContours[ui2].size.height)
			{
				continue;
			}
			
			if(fabs(vRectContours[ui2].angle)>80 && vRectContours[ui2].size.width < vRectContours[ui2].size.height)
			{
				continue;
			}		

			//angle
			//printf("angle: %f\n",fabs(fabs(vRectContours[ui1].angle+45) - fabs(vRectContours[ui2].angle+45)));
			if (abs(fabs(vRectContours[ui1].angle+45) - fabs(vRectContours[ui2].angle+45)) > dAngleMax)
				continue;
			
			//MaxSide MinSide
			int iMaxRect1 = max(vRectContours[ui1].size.width, vRectContours[ui1].size.height);
			int iMinRect1 = min(vRectContours[ui1].size.width, vRectContours[ui1].size.height);
			
			int iMaxRect2 = max(vRectContours[ui2].size.width, vRectContours[ui2].size.height);
			int iMinRect2 = min(vRectContours[ui2].size.width, vRectContours[ui2].size.height);
			
			//cout<<iMaxRect1<<" "<<iMinRect1<<" "<<iMaxRect2<<" "<<iMinRect2<<endl;
			if(iMinRect1 > iSmallSideMax || iMinRect2 > iSmallSideMax)
				continue;

			if(iMaxRect1 > iLargeSideMax || iMaxRect2 > iLargeSideMax)
				continue;
			
			//printf("ratio: %f\n",fabs(iMaxRect1 - iMaxRect2)/max(iMaxRect1,iMaxRect2));

			if ((fabs(iMaxRect1 - iMaxRect2)) > iLargeSideDiffRatio * max(iMaxRect1,iMaxRect2) )
				continue;
			
			//Ratio
			
			//cout<<iMaxRect1/iMinRect1<<endl;
			//cout<<iMaxRect2/iMinRect2<<endl;
			if (iMaxRect1 < dRatioMin * iMinRect1 ||
				iMaxRect1 > dRatioMax * iMinRect1||
				iMaxRect2 < dRatioMin * iMinRect2 ||
				iMaxRect2 > dRatioMax * iMinRect2)
				continue;

			//the dist between two points
			double d2 = (vContoursCentroid[ui1].x-vContoursCentroid[ui2].x)*(vContoursCentroid[ui1].x-vContoursCentroid[ui2].x)+(vContoursCentroid[ui1].y-vContoursCentroid[ui2].y)*(vContoursCentroid[ui1].y-vContoursCentroid[ui2].y);
					
			if(d2 < dDisMin*dDisMin || d2 > dDisMax*dDisMax)
				continue;
			//else
				//cout<<d2<<endl;
			

			double dYDis = abs(vContoursCentroid[ui1].y - vContoursCentroid[ui2].y);
			//cout<<dYDis<<endl;
			if( dYDis > dDisYMax )
				continue;

			//the dist/largeSide 
			iMaxRect = max(iMaxRect1,iMaxRect2);
			//cout<<iMaxRect<<endl;
			if(d2 < iMaxRect * dDisLargeRatioMin * iMaxRect * dDisLargeRatioMin || d2 > iMaxRect * dDisLargeRatioMax * iMaxRect * dDisLargeRatioMax)
				continue;
			//Color is ok
			//1.Get each rect
			r1 = boundingRect(vContours[ui1]);
			r2 = boundingRect(vContours[ui2]);
			//2.Get ROI
			Mat mRect1(mImg,r1);
			Mat mRect2(mImg,r2);
			//3.Get Each ROI color
			int color1 = judgeColor(mRect1);
			int color2 = judgeColor(mRect2);
			if( color1 != color2 || color1 != OBJECTCOLOR)
				continue;

			res1 = vContoursCentroid[ui1];
			res2 = vContoursCentroid[ui2];
			Point resTemp = Point( (res1.x+res2.x)/2,(res1.y+res2.y)/2);
			
			if(resTemp.y>res.y) 
			{
				res = resTemp;
			}
		}
	}
	//cout << "Use time: " << Timer() - dTimeStart << endl;
	//20ms
	
	sprintf(szOpTxt,"res.x: %d res.y: %d iMaxRect: %d",res.x,res.y,iMaxRect);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
		cVideolog.AddText(szOpTxt);
#endif
	foutArmor<<"res.x:"<<res.x <<"res.y:"<<res.y <<" iMaxRect:"<<iMaxRect<<endl;
	

#ifdef MODE_SAVEVIDEOLOG
	Mat mShowImg;
	cvtColor(mImg,mShowImg,CV_RGB2GRAY);
	circle(mShowImg, res, 4, Scalar(255),3);
	circle(mShowImg, Point(xBit,yBit), 4, Scalar(255),3);
	cVideolog.AddFrame(0, 0, mShowImg);
	
//#endif

//#ifdef MODE_SHOWVIDEOLOG
	for (size_t ui = 0; ui < vRectContours.size(); ui++)
	{
		DrawRotatedRect(mContoursAll2, vRectContours[ui], 100);
	}
	//last res
	circle(mContoursAll2, res1, 4, Scalar(255),3);
	circle(mContoursAll2, res2, 4, Scalar(255),3);
	circle(mContoursAll2, res, 4, Scalar(255),3);
	rectangle(mContoursAll2,r1,Scalar(255),2);
	rectangle(mContoursAll2,r2,Scalar(255),2);

	cVideolog.AddFrame(0, 1, mContoursAll2);

	//cVideolog.Flash();
#endif
//27ms
#ifdef MODE_SAVEVIDEOLOG
	cVideolog.Flash();//20ms
#endif
	Mat TEMP(10,10,CV_8UC1);
	imshow("temp",TEMP);

//50ms
	
	return iMaxRect;
}

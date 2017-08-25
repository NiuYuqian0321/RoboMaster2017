
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <libv4l2.h>
#include <linux/videodev2.h>

#include "CArmorDetection.h"
#include "ChariotEx.h"


#define PRE(s) ("$ " s)

extern size_t stAreaMax;
extern size_t stAreaMin;
extern int iSmallSideMax;
extern int iLargeSideMax ;
extern int iLargeSideDiffMax;
extern double dRatioMax ;
extern double dRatioMin ;
extern double dAngleMax;
extern double dDisLargeRatioMax;
extern double dDisLargeRatioMin;
extern double dAreaRatioMin ;
extern double dDisYMax;
extern double dDisYLargeRatioMax;
extern double dDisMin;
extern double dDisMax ;
extern int thresh ;

extern float KPX;
extern float KIX;
extern float KIX15;
extern float KDX;
extern float KPY;
extern float KIY;
extern float KDY;

extern int ex;//上上一次的误差(像素级别)
extern int ey;//上上一次的误差
extern int ex0;//上上一次的误差(像素级别)
extern int ey0;//上上一次的误差
extern int ex1;//上一次的误差(像素级别)
extern int ey1;//上一次的误差
extern int exSum;
extern int eySum;
extern int exSum15;
extern float give;
extern float give1;

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
extern CVideolog cVideolog;
#endif

extern ofstream foutArmor;

extern char szOpTxt[64]; // Output text, almost used in function CVideolog::AddText

int iMaxRectRes = 1;

static double Timer()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((double(tv.tv_sec) * 1000) + (double(tv.tv_usec) / 1000));
}

int main()
{
	// V4L2 to set exposure

	char szCameraVideo[16];
	sprintf(szCameraVideo, "/dev/video%d", 0); // Device ID is 0
	
	int fdDevice = v4l2_open(szCameraVideo, O_RDWR | O_NONBLOCK);
	if (fdDevice == -1)
	{
		cout << "Camera V4L2 device open failed." << endl;
	}
	int iExposure = 280;//??
	if(OBJECTCOLOR == BLUE)
		iExposure = 260;// BLUE:520 ,70

	struct v4l2_control tV4L2;
	tV4L2.id = V4L2_CID_EXPOSURE_AUTO;
	tV4L2.value = 1; // Here set the M/A, 1 is manual, 3 is auto
	if (v4l2_ioctl(fdDevice, VIDIOC_S_CTRL, &tV4L2) != 0)
	{
		cout << "Failed to set... " << strerror(errno) << endl;
	}
	cout << "Set exposure: " << iExposure << endl;
	
	v4l2_set_control(fdDevice, V4L2_CID_EXPOSURE_ABSOLUTE, iExposure);
	
	v4l2_close(fdDevice);

	// Armor detect

	CArmorDetection cArmor;
	CChariotBase cBase;

   	 VideoCapture cCaptrue(0);
    
	//VideoWriter vWriterSrc("src.avi",CV_FOURCC('D','I','V','X'),20,Size(640,480),true);

    if (!cCaptrue.isOpened())
    {
		cout << PRE("Fail to open camera capture!") << endl;
		exit(1);
    }

	cCaptrue.set(CV_CAP_PROP_FRAME_WIDTH,  640);
	cCaptrue.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    Mat mFrame;
    int count =0; 
    int countTrue=0;
    int countFalse=0;
    
    cBase.Check();

    while (true)
    {
	//cout << "iExposure" <<iExposure <<endl;
		double dTimeStart = Timer();

		if (! (cCaptrue.read(mFrame)))
		{
			cout << PRE("Fail to read camera data!") << endl;
			exit(2);
		}
		static int iLog = 1;

		char cKey = waitKey(1);
		switch (cKey)
		{
		case 'q':
		case 'Q':
			cCaptrue.release();
			return 0;
			break;
		case 's':
		case 'S':
			char szFileName[16];
			sprintf(szFileName, "Cam%d.bmp", iLog);
			imwrite(szFileName, mFrame);
			cout << "Save cam pic: " << szFileName << endl;
			iLog++;
			break;
		case 'o':
			KPX+=0.0001;
			break;
		case 'k':
			KPX-=0.0001;
			break;
		case 'i':
			KIX+=0.00001;
			break;
		case 'j':
			KIX-=0.0001;
			break;
		case 'u':
			KDX+=0.0001;
			break;
		case 'h':
			KDX-=0.00001;
			break;
		case 'y':
			KPY+=0.001;
			break;
		case 'g':
			KPY-=0.001;
			break;
		case 't':
			KIY+=0.0001;
			break;
		case 'f':
			KIY-=0.0001;
			break;
		case 'r':
			KDY+=0.0001;
			break;
		case 'd':
			KDY-=0.0001;
			break;
		case 'w':
			give+=0.0001;
			break;
		case 'a':
			give-=0.0001;
			break;
		case 'e':
			give1+=0.0001;
			break;
		case 'z':
			give1-=0.0001;
			break;
		case 'p':
			fdDevice = v4l2_open(szCameraVideo, O_RDWR | O_NONBLOCK);
			iExposure +=10;
			cout<<iExposure<<endl;
			v4l2_set_control(fdDevice, V4L2_CID_EXPOSURE_ABSOLUTE, iExposure);
			v4l2_close(fdDevice);
			break;
		case 'l':
			fdDevice = v4l2_open(szCameraVideo, O_RDWR | O_NONBLOCK);
			iExposure -=10;
			cout<<iExposure<<endl;
			v4l2_set_control(fdDevice, V4L2_CID_EXPOSURE_ABSOLUTE, iExposure);
			v4l2_close(fdDevice);
			break;
		default:
			break;
		//goto LOOP;
		}

		//LOOP:
		Point res(-1,-1);
	//	cout << PRE("Use time: ") << Timer() - dTimeStart << endl;
		iMaxRectRes = cArmor.FindArmor(mFrame,res);//key function
		
	
		if(res.x != -1 && res.y != -1)
		{
			cBase.CalHead(res.x,res.y);
			countTrue++;
			countFalse=0;
			sprintf(szOpTxt,"-------Find: Ture");
			cout<<"-------Find: Ture"<<endl;
			foutArmor<<"-------Find: True"<<endl;

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
			cVideolog.AddText(szOpTxt);
#endif
		}
		else
		{
			countFalse++;
			if(countFalse>10)
			{
				cBase.CtrlLostTarget();
				sprintf(szOpTxt,"-------Find: False");
				cout<<"-------Find: False"<<endl;
				foutArmor<<"-------Find: False"<<endl;
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
				cVideolog.AddText(szOpTxt);
#endif
			}

		}
		count++;
		sprintf(szOpTxt,"count: %d",count);
		foutArmor<<"count:"<<count<<endl;

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
		cVideolog.AddText(szOpTxt);
#endif
		
		sprintf(szOpTxt,"ex: %d ey: %d || exSum: %d eySum: %d || exSum15: %d ",ex,ey,exSum,eySum,exSum15);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
		cVideolog.AddText(szOpTxt);
#endif
		foutArmor<<"ex"<<ex<<"--ey"<<ey<<"--exSum"<<exSum<<"--eySum"<<eySum<<"--exSum15"<<exSum15<<endl;

		sprintf(szOpTxt,"KPX: %f KIX: %f KIX15: %f  KDX: %f || KPY: %f KIY: %f KDY: %f ",KPX,KIX,KIX15,KDX,KPY,KIY,KDY);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
		cVideolog.AddText(szOpTxt);
#endif
		foutArmor<<"KPX"<<KPX<<"--KIX"<<KIX<<"--KDX"<<KDX<<"--KPY"<<KPY<<"--KIY"<<KIY<<"--KDY"<<KDY<<endl;
		
		cout<<"KPX"<<KPX<<"--KIX"<<KIX<<"--KDX"<<KDX<<endl;
		cout<<"KPY"<<KPY<<"--KIY"<<KIY<<"--KDY"<<KDY<<endl;
		//printf("KPX: %f KIX: %f KIX15: %f  KDX: %f || KPY: %f KIY: %f KDY: %f \n",KPX,KIX,KIX15,KDX,KPY,KIY,KDY);
		
		sprintf(szOpTxt,"give: %f give1: %f",give,give1);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
		cVideolog.AddText(szOpTxt);
#endif
		foutArmor<<"give:"<<give<<"  give1:"<<give1<<endl;
		cout<<"give:"<<give<<"  give1:"<<give1<<endl;


		//cout<<res.x<<" "<<res.y<<endl;
		foutArmor<<res.x<<" "<<res.y<<endl;
		foutArmor<<iExposure<<endl;
//#ifdef DEBUG_ARMOR
		//circle(mFrame, res, 4, Scalar(0, 0, 255), 3);
		//imshow("mFrame", mFrame);
		//vWriterSrc.write(mFrame);
//#endif
		//cout << PRE("Use time: ") << Timer() - dTimeStart << endl;
		cout << PRE("FPS: ") << 1000 / (Timer() - dTimeStart) << endl;
		//cout<<iExposure<<endl;
	
    }

    return 0;
}


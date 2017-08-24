#include<iostream>
using namespace std;

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

int main()
{
	VideoCapture cCaptrue(0);
    
	int iLog = 1;
    if (!cCaptrue.isOpened())
    {
		cout << "Fail to open camera capture!" << endl;
		exit(1);
    }

	cCaptrue.set(CV_CAP_PROP_FRAME_WIDTH,  640);
	cCaptrue.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	Mat mFrame;
	Point xy(294,315);

    while (true)
    {

		if (! (cCaptrue.read(mFrame)))
		{
			cout << "Fail to read camera data!" << endl;
			exit(2);
		}

		char cKey = waitKey(1);
		switch (cKey)
		{
		case 'p':
			xy.x++;
			break;	
		case 'l':
			xy.x--;
			break;
		case 'o':
			xy.y++;
			break;	
		case 'k':
			xy.y--;
			break;
		case 'q':
		case 'Q':
			goto FINI;

		case 's':
		case 'S':
			char szFileName[16];
			sprintf(szFileName, "Cam%d.bmp", iLog);
			imwrite(szFileName, mFrame);
			cout << "Save cam pic: " << szFileName << endl;
			iLog++;
			
			goto LOOP;
		}

LOOP:
		circle(mFrame,xy,3,Scalar(255),3);
		imshow("mFrame",mFrame);
		cout<<xy.x<<" "<<xy.y<<endl;
    }

	FINI:

	cCaptrue.release();
	
	return 0;
}

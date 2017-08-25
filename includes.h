#include <iostream>
#include <fstream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <errno.h>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

#include "Mode.h"
#include "Videolog.h"

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	CVideolog cVideolog(480,640,true,true,CV_8UC1, 1, 2,0.5);
#endif

	char szOpTxt[64]; // Output text, almost used in function CVideolog::AddText

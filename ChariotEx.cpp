// ChaiotEx.cpp
// Version 1.1-Base
// Started by Deyou Kong, 2017-06-25
// Checked by Deyou Kong, 2017-06-25
// Finished by NiuYuqian, 2017-08-25

#include "ChariotEx.h"

//#define P
//#define PD
#define PID

int xBit = 295;//320;
int yBit = 125; //275-160;///bianchang20
float yaw = 0;
float pitch = 0;
float KPX = 0.0024;//0.0006;
float KIX = 0.0;//0.00003;
float KIX15 = 0.0;//0.00006;
float KDX = 0.0001;//0.0086;
float KPY = 0.05;//ok
float KIY = 0.0001;//ok
float KDY = 0.0035;//ok
int ex=0;
int ey=0;
int ex0 = 0;//上上一次的误差(像素级别)
int ey0 = 0;//上上一次的误差
int ex1 = 0;//上一次的误差(像素级别)
int ey1 = 0;//上一次的误差
int exSum = 0;
int eySum = 0;
int exSum15 = 0;

float give = 0.00;//0.203;//0.17;
float give1 = 0.00;//-0.006;

extern int iMaxRectRes;

#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	extern CVideolog cVideolog;
#endif

extern ofstream foutArmor;

char szOpTxt2[64];

bool CChariotBase::CtrlLostTarget()
{
	return CtrlShoot(0x10, false);
}

void CChariotBase::Check()
{
	CtrlShoot(0, true);
}

void CChariotBase::CalHead(int x,int y)
{
	cout<<"x:"<<x<<",y:"<<y<<endl;
	cout<<"iMaxRectRes: "<<iMaxRectRes<<endl;
	
	int threshBitX = 25;
	int threshBitY = 25;

	if(iMaxRectRes < 15)
	{
		threshBitX = 18;
		threshBitY = 18;
		yBit = 130;//yBit = 135;// 295-160;//282-160
	}
	
	if(iMaxRectRes > 23)
	{
		//yBit = 215;//yBit = 175;// 335-160;
		threshBitX=37;
		threshBitY=37;
	}
	ex = ((-1)*0.6 * iMaxRectRes + 14) *( x - xBit);//这一次的误差
	ey = y - yBit;//这一次的误差

	exSum += ex;
	eySum += ey;
	cout<<abs(ex)<<"---abs---"<<abs(ey)<<endl;
	sprintf(szOpTxt2,"abs(ex)%d ---abs(ey)%d",abs(ex),abs(ey) );
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt2);
#endif

#ifdef P
	float yaw   = iMaxRectRes * KPX * ex;
	float pitch = iMaxRectRes * KPY * ey;
	//float yaw   =45;
	//float pitch = 0;
#endif
#ifdef PD
	float yaw   = iMaxRectRes * KPX * ex + KDX * (ex-ex0);
	float pitch = iMaxRectRes * KPY * ey + KDY * (ey-ey0);
#endif

#ifdef PID
	float yaw   =   KPX * ex + KIX * exSum + KDX * (ex-ex0) ;
	float pitch =  KPY * ey + KIY * eySum + KDY * (ey-ey0);

#endif

foutArmor<<yaw<<"||"<<pitch<<endl;
	
	sprintf(szOpTxt2,"yaw:%2f || pitch:%2f",yaw,pitch);
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt2);
#endif
	
	cout<<"exSum15"<<exSum15<<endl;
	cout<<"KPX:"<<KPX<<"KIX:"<<KIX<<" KDX:"<<KDX<<"KIX15"<<KIX15<<endl;

	ex0 = ex1;
	ey0 = ey1;
	ex1 = ex;
	ey1 = ey;
 
	if((yaw<1.5 && yaw>0)||(yaw>-1.5 && yaw<0)) 
	{
		yaw += KIX15 * exSum15;
		exSum15 += ex;
	}
	else
		exSum15=0;
	//if(pitch<1 && pitch>0) pitch=1;
	//else if(pitch>-1 && pitch<0) pitch=-1;

	cout<<"..................................give:"<<give<<endl;
	cout<<"..................................give1:"<<give1<<endl;
	//float yaw10 = yaw*give;
	yaw = yaw + yaw*give + yaw*yaw*give1;

	
	cout<<yaw<<"||"<<pitch<<endl;
	

	//shoot
	if(abs(ex) < threshBitX && abs(ey) < threshBitY)//keyiyouhua
	{
		cout<<abs(ex)<<"---abs---"<<abs(ey)<<endl;
		CtrlShoot(0, true);
		eySum=0;
		exSum=0;
		//exSum15=0;
		cout<<"------------------------------shoot------"<<endl;
		foutArmor<<"------------------------------shoot------"<<endl;
		
		sprintf(szOpTxt2,"------------------------------shoot------");
#if ((defined MODE_SAVEVIDEOLOG) || (defined MODE_SHOWVIDEOLOG))
	cVideolog.AddText(szOpTxt2);
#endif
		
		if(abs(ex) < threshBitX-5 && abs(ey) < threshBitY-5)
			return;
	}	
		
	//调节
	//ex>0,move right
	//ey>0,move down

	
	CtrlHead(yaw,pitch);
	
	
}

/*bool CChariotBase::Scanning()
{
	return CtrlHead(1, 0);
}*/

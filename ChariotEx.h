// ChariotEx.h
// Version 1.1-Base
// Started by Deyou Kong, 2017-06-25
// Checked by Deyou Kong, 2017-06-25

#pragma once

//#include "includes.h"
#include "Chariot.h"
#include "Mode.h"
#include "Videolog.h"


class CChariotBase : public CChariot
{
public:
	void CalHead(int x,int y);
	//bool Scanning();	
	bool CtrlLostTarget();
	void Check();
};

/*class CChariotHero : public CChariot
{
};*/

/*class CChariotSoldier : public CChariot
{
};*/

/*class CChariotEnginer : public CChariot
{
};*/

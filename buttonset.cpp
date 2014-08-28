#include "StdAfx.h"
#include "buttonset.h"

void CButtonSet::ModeWait()
{
	b[0].ShowWindow(true);
	b[1].ShowWindow(true);
	b[0].EnableWindow(false);
	b[1].EnableWindow(false);
	for(int i=2;i<10;++i)
		b[i].ShowWindow(false);
}

void CButtonSet::ModeOK()
{
	b[0].ShowWindow(true);
	b[1].ShowWindow(true);
	b[0].EnableWindow(true);
	b[1].EnableWindow(false);
	for(int i=2;i<10;++i)
		b[i].ShowWindow(false);
}

void CButtonSet::ModeOKCancel()
{
	b[0].ShowWindow(true);
	b[1].ShowWindow(true);
	b[0].EnableWindow(true);
	b[1].EnableWindow(true);
	for(int i=2;i<10;++i)
		b[i].ShowWindow(false);
}

void CButtonSet::ModeShow23()
{
	for(int i=0;i<10;++i)
	{
		if(i==2||i==3)b[i].ShowWindow(true);
		else b[i].ShowWindow(false);
	}
}

void CButtonSet::ModeShow45()
{
	for(int i=0;i<10;++i)
	{
		if(i==4||i==5)b[i].ShowWindow(true);
		else b[i].ShowWindow(false);
	}
}

void CButtonSet::ModeShow67()
{
	for(int i=0;i<10;++i)
	{
		if(i==6||i==7)b[i].ShowWindow(true);
		else b[i].ShowWindow(false);
	}
}

void CButtonSet::ModeShow89()
{
	for(int i=0;i<10;++i)
	{
		if(i==8||i==9)b[i].ShowWindow(true);
		else b[i].ShowWindow(false);
	}
}

#ifndef BUTTON_SET_H
#define BUTTON_SET_H

class CButtonSet
{
public:
	CButton b[10];

	void ModeWait();
	void ModeOK();
	void ModeOKCancel();
	void ModeShow23();
	void ModeShow45();
	void ModeShow67();
	void ModeShow89();
};

#endif
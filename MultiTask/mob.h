#pragma once

typedef struct _stRECTM
{
	LONG x;
	LONG y;
	LONG w;
	LONG h;
}RECTM, *LPLECTM;


class CMob
{
public:
	CMob();
	~CMob();

public:
	DWORD ID;
	TCHAR type[16];
	RECTM area;
	DWORD status;
	void setpos(LONG x, LONG y) { area.x = x; area.y = y; return; }
	void setpos(int x, int y) { area.x = x; area.y = y; return; }
	void setsize(LONG w, LONG h) { area.x = w; area.y = h; return; }
	void setsize(int w, int h) { area.x = w; area.y = h; return; }
	void setstatus(int i) { status = i; return; }
};

class CHarai : public CMob
{
public:
	CHarai() {};
	~CHarai() {};
private:

};
class CCrush : public CMob
{
public:
	CCrush() {};
	~CCrush() {};
private:

};


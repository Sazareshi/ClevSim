#pragma once

#define MOB_TYPE_NUM			32	//Mob�̃^�C�v��
#define MOB_MAX_NUM				128 //Mob�̐�

#define MOB_ID_BC				0 //�x���g�R���x��ID
#define MOB_ID_HARAI			1 //�����o���@ID
#define MOB_ID_CRUSH			2 //�N���b�V���[ID
#define MOB_ID_CUL				3 //CUL ID
#define MOB_ID_TRIPPER			4 //�ςݕt���@ ID
#define MOB_ID_EROOM			5//���O�ݔ����䎺 ID




#define MOB_STAT_IDLE			0
#define MOB_STAT_FAULT			0xFF
#define MOB_STAT_ACT0			1
#define MOB_STAT_ACT1			2
#define MOB_STAT_ACT2			3
#define MOB_STAT_ACT3			4

#define MOB_TYPE_BC			TEXT("BC0000")
#define MOB_TYPE_HARAI		TEXT("HARAI0")
#define MOB_TYPE_CRUSH		TEXT("CRUSH0")
#define MOB_TYPE_CUL		TEXT("CUL000")
#define MOB_TYPE_TRIPPER	TEXT("TRIP00")
#define MOB_TYPE_EROOM		TEXT("EROOM0")


typedef struct _stCUBEM //mob���W�\����3D
{
	LONG x;	LONG y;	LONG z; LONG w;	LONG h; LONG d;
}CUBEM, *LPCUBEM;

class CShoot {
public: 
	CShoot() {};
	~CShoot() {};
};

class CMob
{
public:
	CMob();
	~CMob();

public:
	TCHAR type[8];
	int type_code;
	DWORD ID;
	TCHAR name[16];
	double radz;//�p�xradian z�����
	CUBEM area; //�V�~�����[�V������ԏ�̔z�u�G���A
	CUBEM area_real;//���V�X�e����ԏ�̔z�u�G���A
	HBITMAP hBmp_mob;
	LONG bmpw; 
	LONG bmph;
	DWORD status;//�Ώۂ̏��
	DWORD command;//�Ώۂւ̎w��
	ULONG* ptime_now;
	ULONG* ptime_last;

	void setpos(LONG x, LONG y, LONG z) { area.x = x; area.y = y; area.z = z; return; }
	void setpos(int x, int y, int z) { area.x = x; area.y = y; area.z = z; return; }
	void setsize(LONG w, LONG h, LONG d) { area.x = w; area.h = h; area.d = d; return; }
	void setsize(int w, int h, int d) { area.x = w; area.d = d; return; }
	void setstatus(int i) { status = i; return; }
};

#define LD_BIO		0x80//�o�C�I�}�X
#define LD_COAL1	0x01//�ΒY�^�C�v1
#define LD_COAL2	0x02//�ΒY�^�C�v2
#define LD_METAL1	0x10//�����L��

typedef struct _stLoad {
	WORD material;//�f�ސ����@
	WORD density;//	���x�@kg/m3
	WORD vol;//	BC1m�̑̐ρ@1000cm3(l)
	WORD weight;//BC1m������̏d��kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//�ؑփ_���p�L
#define BC_TSHOOT	0x0002//�e���X�R�V���[�g�L
#define BC_2WAY		0x0004//2����
#define BC_2MOTOR	0x0010//2���[�^
#define BC_HVOLT	0x0020//�������[�^
#define BC_TRP		0x0100//�g���b�p�[�t
#define BC_DROP1	0x0200//���o�p1��
#define BC_DROP2	0x0400//���o�p2��
#define BC_RCV_MAP	8//BC�̉׎󂯏ꏊ�ő吔
#define BC_MAX_LEN	800//BC�̍ő咷��



class CBC : public CMob
{
public:
	CBC() {};
	~CBC() {};
	DWORD BCtype; 
	char cline;//A,B,C
	int nrcv;//�ΒY�󂯌���he
	int pos_rcv[BC_RCV_MAP];//�ΒY�󂯈ʒum
	DWORD l;//����mm
	DWORD w;//��mm
	int  spd;//���ݑ��x�@mm/s
	int  trq;//�g���N
	STLOAD	belt[BC_MAX_LEN];//�x���g��1024mm�P�ʂōl����
	int	ihead;//�x���g�w�b�h�ʒu�C���f�b�N�X
	DWORD	headpos_mm;
	int put_load(int pos, STLOAD load);
	STLOAD pop_load(int pos);
private:

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

#define NUM_DROP_POINT_CUL 2

class CCUL : public CMob
{
public:
	CCUL() {};
	~CCUL() {};
	STLOAD dc_load;//�r�o�ωד��e
	CBC*	pBCA, pBCB;//�r�o��R���x��
	int		iputA, iputB;//�r�o��R���x���̈ʒu�C���f�b�N�X
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	LPSTLOAD pt_drop[NUM_DROP_POINT_CUL];//�R���x���ւ̃h���b�v�|�C���g

private:
	int discharge();
	STLOAD set_load(WORD material,WORD density,	WORD vol,WORD weight);
};

class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};
private:

};

class CEroom : public CMob
{
public:
	CEroom() {};
	~CEroom() {};
private:

};

#define BC_LINES	3//BC�̌n����A,B,C
#define BC_LINE_NUM	30//BC�̊e�n���̐�
#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_EROOM			3

//���L�������z�u��`
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
}STMobsBody, *LPSTMobsBody;


typedef struct _stMobs {
	BOOL	bUpdate;
	CMob* 	pmobs[MOB_TYPE_NUM][MOB_MAX_NUM];
	STMobsBody	mobs;
}STMobs, *LPSTMobs;




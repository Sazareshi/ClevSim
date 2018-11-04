#pragma once
#include "EParts.h"

#define MOB_TYPE_NUM			32	//Mob�̃^�C�v��
#define MOB_MAX_NUM				128 //Mob�̐�

#define MOB_ID_BC				0 //�x���g�R���x��ID
#define MOB_ID_HARAI			1 //�����o���@ID
#define MOB_ID_CRUSH			2 //�N���b�V���[ID
#define MOB_ID_CUL				3 //CUL ID
#define MOB_ID_TRIPPER			4 //�ςݕt���@ ID
#define MOB_ID_EROOM			5//���O�ݔ����䎺 ID
#define MOB_ID_SILO				6//SILO ID

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
#define MOB_TYPE_SILO		TEXT("SILO00")

#define MASK_DIR_X			0x00ff
#define MASK_DIR_LEFT		0x00ff
#define MASK_DIR_UP			0x00ff
#define MASK_DIR_Y			0xff00

#define MOB_COM_UPDATE	0x0000
#define MOB_COM_RESET	0x0001

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
	TCHAR type[8];//�@���ޖ�
	DWORD ID;//�@��ID HIWORD=type LOWORD=id
	TCHAR name[16];//�@�햼
	double radz;//�p�xradian z�����
	CUBEM area; //�V�~�����[�V������ԏ�̔z�u�G���A
	CUBEM area_real;//���V�X�e����ԏ�̔z�u�G���A
	HBITMAP hBmp_mob;
	LONG bmpw; 
	LONG bmph;
	DWORD status;//�Ώۂ̏��
	DWORD command;//�Ώۂւ̎w��
	ULONG* ptime_now;//�V�~�����[�V�������ݎ����Q�ƃA�h���X
	ULONG  time_last;//�V�~�����[�V�����O�񎞍��ێ��l
	BOOL exist;//���݂��邩�ǂ����AON�Ȃ����������{

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
	WORD vol;//	�̐ρ@1000cm3(l)
	WORD weight;//�d��kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//�ؑփ_���p�L
#define BC_TSHOOT	0x0002//�e���X�R�V���[�g�L
#define BC_2WAY		0x0004//2����
#define BC_2MOTOR	0x0010//2���[�^
#define BC_3MOTOR	0x0020//3���[�^
#define BC_HVOLT	0x0040//�������[�^
#define BC_TRP		0x0100//�g���b�p�[�t
#define BC_DROP1	0x0200//���o�p1��
#define BC_DROP2	0x0400//���o�p2��
#define BC_RCV_MAX	12//BC�̉׎󂯏ꏊ�ő吔
#define BC_MAX_LEN	800//BC�̍ő咷��
#define BC_MOTOR_MAX	3//�ő僂�[�^��
#define BC_LINK_MAX		3//BC�ڑ���ő吔

#define BC_COAL_DISP_PIXW	5//BC�@�ΒY�\���s�N�Z����


#define SIRO_COLUMN_NUM	12//�T�C���敪��

class CSilo : public CMob 
{
public:
	CSilo() {};
	~CSilo() {};
	DWORD SILOtype;
	DWORD l;//����mm
	DWORD l_offset;//�R���x�����C����̃I�t�Z�b�g�ʒu
	DWORD w;//��mm
	DWORD capa_all;//�S�e�ʁ@kg
	DWORD capa1;//1��悠�����i

	STLOAD column[SIRO_COLUMN_NUM];//�T�C���敪�G���A
	int put_load(int pos, STLOAD load);
	int remove_load(int pos, STLOAD load);
	int clear_load();
	DWORD pix2kg;
	int pix_columw;

private:

private:
};


#define COM_TRP_IDLE	0
#define COM_TRP_DISCHARGE	1
#define COM_TRP_MOVE		2


class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};

	int  spd;//���ݑ��x�@mm/s
	int discharge(DWORD com, LONG dt);
	int pos_drop[SIRO_COLUMN_NUM];
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	CSilo* silo[4];//�R�t���T�C��

private:

};

#define BC_HEAD_0		0
#define BC_HEAD_DUMPER	1
#define BC_HEAD_SHOOT	2

class CBCHead
{
public:
	CBCHead() {};
	~CBCHead() {};

	int type;// 0:plane, 1:dumper, 2:shoot
	int pos;

	int	activate(int com) { pos = com; };
};
class CBC : public CMob
{
public:
	CBC();
	~CBC();
	DWORD BCtype; 
	int nrcv;//�ΒY�󂯌���
	int pos_rcv[BC_RCV_MAX];//�ΒY�󂯈ʒum
	POINT imgpt_rcv[BC_RCV_MAX];//�ΒY�󂯉�ʈʒu
	POINT imgpt_top[3],imgpt_tail;

	DWORD dir;//��ʂł̌����@HIWORD�@�c���@0�����@LOWORD�@0��������
	DWORD l;//����mm
	DWORD w;//��mm
	DWORD ability;//��i�����\�́@ton/h
	int  spd;//���ݑ��x�@mm/s
	int  base_spd;//��i���x�@mm/s
	int  trq;//�g���N
	STLOAD	belt[BC_MAX_LEN];//�x���g��1024mm�P�ʂōl����
	int belt_size;//belt�z��̃T�C�Y
	int	ihead;//�x���g�w�b�h�ʒu�C���f�b�N�X
	int	ihead_last;//�x���g�w�b�h�ʒu�O��l
	DWORD	headpos_mm;
	DWORD	headpos_pix;
	DWORD	Kg100perM;//BC1m������̒�i�����d��
	CBCHead head_unit;
	CTripper* ptrp;
	
	CBC* bclink[BC_LINK_MAX];//�r�o��BC�|�C���^
	CSilo* silolink;//�r�o��T�C���|�C���^�@�g���b�p�A�X�N���[�p�t��BC�̂�
	int bclink_i[BC_LINK_MAX];//�r�o��BC�̐ڑ�Drop�ʒu�C���f�b�N�X
	int put_load(int pos, STLOAD load);
	void conveyor(DWORD com, LONG dt);
	void bc_reset() { headpos_mm = 0; memset(&pos_rcv, 0, sizeof(STLOAD) * BC_MAX_LEN); return; };//�x���g�w�b�h�ʒu��0���Z�b�g���đS�ΒY���N���A
	CMotor motor[BC_MOTOR_MAX];
	DWORD pix2mm;
	
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
#define COM_CUL_IDLE	0
#define COM_CUL_DISCHARGE	1


class CCUL : public CMob
{
public:
	CCUL();
	~CCUL();
	STLOAD dc_load;//�r�o�ωד��e
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s

	CBC* bclink[NUM_DROP_POINT_CUL];//�r�o��BC�|�C���^
	int bclink_i[NUM_DROP_POINT_CUL];//�r�o��BC�̐ڑ�Drop�ʒu�C���f�b�N�X

	int bc_selbc;//�����o��line�I�� 0:A,1:B

	int discharge(DWORD com, LONG dt);
	
	STLOAD set_load(WORD material, WORD density, WORD vol, WORD weight);
	STLOAD load_base;

private:

};

class CEroom : public CMob
{
public:
	CEroom() {};
	~CEroom() {};
private:

};

#define BC_LINES	3//BC�̌n����A,B,C+�z��
#define BC_LINE_NUM	26//BC�̊e�n���̐�
#define NUM_OF_HARAI			9
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_EROOM			3
#define SILO_LINES				7	// 1��3line�@2���ݔ�2line�@BIO�@2line
#define SILO_LINE_NUM			4	//�e���C���̃T�C�����@Max�@1���@4�@�iBIO��8�T�C����1�Ɗ��Z�j


//���L�������z�u��`
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
	CSilo		silo[SILO_LINES][BC_LINE_NUM];
}STMobsBody, *LPSTMobsBody;


typedef struct _stMobs {
	BOOL	bUpdate;
	CMob* 	pmobs[MOB_TYPE_NUM][MOB_MAX_NUM];
	STMobsBody	mobs;
}STMobs, *LPSTMobs;


#define LINE_A 0
#define LINE_B 1
#define LINE_C 2
#define LINE_D 3
#define LINE_E 4
#define LINE_F 5
#define LINE_G 6
#define DUMP1 0
#define DUMP2 1
#define DUMP3 2

#define BC_L1 0
#define BC_L2 1
#define BC_L3 2
#define BC_L4_1 3
#define BC_L4_2 4
#define BC_L4_3 5
#define BC_L5  6
#define BC_L6 7
#define BC_L7_1 8
#define BC_L7_2 9
#define BC_L7_3 10
#define BC_L8 11
#define BC_L11 12
#define BC_L12 13
#define BC_L13 14
#define BC_L15 15
#define BC_L16 16
#define BC_L17 17
#define BC_L18_1 18
#define BC_L18_2 19
#define BC_L19 20
#define BC_L20 21
#define BC_L21 22
#define BC_L22 23
#define BC_L23 24

#define BC_L30 0
#define BC_L31 1
#define BC_L32 2
#define BC_L33 3
#define BC_L34 4
#define BC_L35A 8
#define BC_L35B 9
#define BC_L8D 12





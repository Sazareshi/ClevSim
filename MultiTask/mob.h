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
#define MOB_ID_SCRAPER			7//�X�N���[�p�[ ID
#define MOB_ID_HARAI_BIO		8//�o�C�I�T�C�������o���@ ID
#define MOB_ID_SCREEN			9//�X�N���[�� ID


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
#define MOB_TYPE_SCRAPER	TEXT("SCRP00")
#define MOB_TYPE_HARAI_BIO	TEXT("HARAIB")
#define MOB_TYPE_SCREEN		TEXT("SCREEN")

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
	BOOL b_bmp_aline_bottom;

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
	LONG weight;//�d��kg
} STLOAD, *LPSTLOAD;

#define BC_DUMPER	0x0001//�ؑփ_���p�L
#define BC_TSHOOT	0x0004//�e���X�R�V���[�g3port
#define BC_TSHOOT4	0x0008//�e���X�R�V���[�g4port
#define BC_2WAY		0x0002//2����
#define BC_2MOTOR	0x0010//2���[�^
#define BC_3MOTOR	0x0020//3���[�^
#define BC_HVOLT	0x0040//�������[�^
#define BC_TRP		0x0100//�g���b�p�[�t
#define BC_SQR		0x0200//�X�N���[�p�t
#define BC_DROP1	0x0400//���o�p1��
#define BC_DROP2	0x0800//���o�p2��
#define BC_DROPB	0x1000//���o�p�o�C�I
#define BC_CRUSH	0x2000//�N���b�V���[�t
#define BC_RCV_MAX	12//BC�̉׎󂯏ꏊ�ő吔
#define BC_MAX_LEN	800//BC�̍ő咷��
#define BC_MOTOR_MAX	3//�ő僂�[�^��
#define BC_LINK_MAX		4//BC�ڑ���ő吔

#define BC_COAL_DISP_PIXW	5//BC�@�ΒY�\���s�N�Z����

#define SILO_COLUMN_NUM			12//�T�C���敪��
#define SILO_COLUMN_NUM_BIO		8//�T�C���敪��
#define SILO_COLUMN_NUM_BANK	6//�T�C���敪��
#define SILO_TYPE_1				0x0001//1���T�C��
#define SILO_TYPE_2				0x0002//2���T�C��
#define SILO_TYPE_BIO			0x0004//�o�C�I�T�C��
#define SILO_TYPE_BANK			0x0008//�o���J�[


class CSilo : public CMob
{
public:
	CSilo() {};
	~CSilo() {};
	LONG SILOtype;
	LONG l;//����mm
	LONG w;//��mm
	LONG pos_bc_origin_put;//�σR���x�����C����̃I�t�Z�b�g�ʒum�@�e�[������
	LONG pos_bc_origin_pop;//���R���x�����C����̃I�t�Z�b�g�ʒum�@�e�[������
	LONG capa_all;//�S�e�ʁ@kg
	LONG capa1;//1��悠�����i
	LONG thresh_level;//�ώR�������̔���l�i�׃Z���Ƃ̍��j
	DWORD dir;//��ʂł̌����@HIWORD�@�c���@0�����@LOWORD�@0��������

	STLOAD column[SILO_COLUMN_NUM];//�T�C���敪�G���A
	int put_load(int pos, STLOAD load);//pos �T�C����󂯈ʒu��
	STLOAD pop_load(int pos, STLOAD load);
	int clear_load();
	LONG pix2kg;
	int pix_columw;
	int ini_stock_percent;//�����X�g�b�N��

private:

private:
};


#define NUM_DROP_POINT_CUL 2
#define COM_CUL_IDLE	0
#define COM_CUL_DISCHARGE	1



#define BC_HEAD_0		0x0000
#define BC_HEAD_DUMPER	0x0001
#define BC_HEAD_DUAL	0x0002
#define BC_HEAD_SHOOT3	0x0004
#define BC_HEAD_SHOOT4	0x0008
#define BC_SILO_LINK_MAX	4

#define BC_LINK_REVERSE	2 //�t�R���x���@�t�������̃����N��͔z��2�ɌŒ�
#define BC_22HEAD_BANK	1 

class CBCHead
{
public:
	CBCHead() {};
	~CBCHead() {};

	int type;// 0:plane, 1:dumper,2:Dual,3:shoot3,4:shoot4
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
	LONG l;//����mm
	LONG w;//��mm
	LONG ability;//��i�����\�́@ton/h
	int  spd;//���ݑ��x�@mm/s
	int  base_spd;//��i���x�@mm/s
	int  trq;//�g���N
	STLOAD	belt[BC_MAX_LEN];//�x���g��1024mm�P�ʂōl����
	int belt_size;//belt�z��̃T�C�Y
	int	ihead;//�x���g�w�b�h�ʒu�C���f�b�N�X
	int	ihead_last;//�x���g�w�b�h�ʒu�O��l
	LONG	headpos_mm;
	LONG	headpos_pix;
	LONG	Kg100perM;//BC1m������̒�i�����d��
	CBCHead head_unit;
	CMob* ptrp;
	
	CBC* bclink[BC_LINK_MAX];//�r�o��BC�|�C���^
	CSilo* silolink[BC_SILO_LINK_MAX];//�r�o��T�C���|�C���^�@�g���b�p�A�X�N���[�p�t��BC�̂�
	int bclink_i[BC_LINK_MAX];//�r�o��BC�̐ڑ�Drop�ʒu�C���f�b�N�X
	int put_load_i(int i_pos, STLOAD load);//i_pos BC����ʒu�z��C���f�b�N�X
	STLOAD put_load(int pos, STLOAD load);//pos BC����ʒum
	STLOAD pop_load(int pos, STLOAD load);//pos BC����ʒum
	void conveyor(DWORD com, LONG dt);
	void bc_reset() { headpos_mm = 0; memset(&pos_rcv, 0, sizeof(STLOAD) * BC_MAX_LEN); return; };//�x���g�w�b�h�ʒu��0���Z�b�g���đS�ΒY���N���A
	CMotor motor[BC_MOTOR_MAX];
	LONG pix2mm;
	LONG put_test_load;//0�ȊO�Ńe�[���|�W�V��������̃C���f�b�N�X�ʒu�ɏd�ʓ���

	BOOL b_rverse;
private:

};


class CScreen : public CMob
{
public:
	CScreen() {};
	~CScreen() {};
	STLOAD buffer;//�N���b�V���[�p�o�b�t�@
	CBC* pbc;
	int trans_ratio;//���ߗ�

	STLOAD pop_load(STLOAD load);//�N���b�V���[�p�o�b�t�@����Pop

private:

};

class CCrush : public CMob
{
public:
	CCrush() {};
	~CCrush() {};
	CBC* pbc;
	CScreen* pscreen;
	STLOAD crush_load(STLOAD load);//�N���b�V���[�p�o�b�t�@����Pop����BC��Put
private:

};


#define COM_TRP_IDLE	0
#define COM_TRP_DISCHARGE	1
#define COM_TRP_MOVE		2
#define TRP_SIRO_DRP_NUM	6
#define TRP_SIRO_DRP_LAYER	2
#define TRP_MOVE_COMPLETE_RANGE	500

class CTripper : public CMob
{
public:
	CTripper() {};
	~CTripper() {};

	int  spd;//���ݑ��x�@mm/s
	int  pos;//���݈ʒu�@mm/s�@�R�t��BC��e�[������̈ʒu
	int  pos_max, pos_min;//�ړ��͈͍ő�l�A�ŏ��lmm


	int pos_drop[TRP_SIRO_DRP_LAYER][TRP_SIRO_DRP_NUM];//SILO��ڕW�ʒu��
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	CSilo* psilo[4];//�R�t���T�C��
	CBC* pbc;//�R�tBC
	

	int discharge(DWORD com, LONG dt);//�R�t��BC����R�t���T�C���֐ΒY���ڍ�
	int move(DWORD com, LONG dt,int target);//�ړ�/�ڍځ@�߂�l1�F�ړ��L��
	void set_command(DWORD com) { command |= com; };//�w���R�}���h
	void reset_command(DWORD com) { command &= ~com; };//�w���R�}���h
	DWORD get_command() { return command; };//�w���R�}���h
	void set_target(int target) { if (target < pos_min)pos_target = pos_min;else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//�w���R�}���h
	int get_target() { return pos_target; }
	void set_param();//�p�����[�^��W�J

	void set_area(int pos) { area.y = pbc->area.y + pos/ pbc->pix2mm; return; };

private:
	int pos_target;

};

#define COM_SCRP_IDLE	0
#define COM_SCRP_DISCHARGE	0x0001
#define COM_SCRP_ACT1		0x0002
#define COM_SCRP_ACT2		0x0004
#define COM_SCRP_ACT3		0x0008
#define COM_SCRP_ACT4		0x0010
#define COM_SCRP_ACT5		0x0020
#define COM_SCRP_ACT6		0x0040
#define COM_SCRP_ACT7		0x0080
#define COM_SCRP_ACT8		0x0100
#define COM_SCRP_ACT9		0x0200
#define COM_SCRP_ACT10		0x0400
#define COM_SCRP_ACT11		0x0800
#define COM_SCRP_ACT12		0x1000

#define SCRP_SIRO_DRP_NUM	6
#define SCRP_SIRO_DRP_LAYER	2
#define SCRP_MOVE_COMPLETE_RANGE	500
#define SCRP_8		0x0002
#define SCRP_9_2	0x0004
#define SCRP_23		0x0008

class CScraper: public CMob
{
public:
	CScraper() {};
	~CScraper() {};

	int  spd;//���ݑ��x�@mm/s
	int  pos;//���݈ʒu�@mm/s�@�R�t��BC��e�[������̈ʒu
	int  pos_max, pos_min;//�ړ��͈͍ő�l�A�ŏ��lmm


	int pos_drop[SCRP_SIRO_DRP_LAYER][SCRP_SIRO_DRP_NUM];//SILO��ڕW�ʒu��
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	CSilo* psilo[4];//�R�t���T�C��
	CBC* pbc;//�R�tBC
	DWORD SCRPtype;


	int discharge(DWORD com, LONG dt);//�R�t��BC����R�t���T�C���֐ΒY���ڍ�
	int move(DWORD com, LONG dt, int target);//�ړ�/�ڍځ@�߂�l1�F�ړ��L��
	void set_command(DWORD com) { command |= com; };//�w���R�}���h
	void reset_command(DWORD com) { command &= ~com; };//�w���R�}���h
	DWORD get_command() { return command; };//�w���R�}���h
	void set_target(int target) { if (target < pos_min)pos_target = pos_min; else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//�w���R�}���h
	int get_target() { return pos_target; }
	void set_param();//�p�����[�^��W�J

	void set_area(int pos) { 
		if (SCRPtype == SCRP_23) area.x = pbc->area.x + pos / pbc->pix2mm;
		else area.y = pbc->area.y + pos / pbc->pix2mm; 
		return; 
	};
		
private:
	int pos_target;

};


#define COM_HARAI_IDLE	0
#define COM_HARAI_DISCHARGE	1

#define COM_HARAI_MOVE		2
#define HARAI_SIRO_DRP_NUM	12
#define HARAI_MOVE_COMPLETE_RANGE	500

class CHarai : public CMob
{
public:
	CHarai() {};
	~CHarai() {};

	int  spd;//���ݑ��x�@mm/s
	int  pos;//���݈ʒu�@mm/s�@�R�t��BC��e�[������̈ʒu
	int  pos_max, pos_min;//�ړ��͈͍ő�l�A�ŏ��lmm


	int pos_drop[HARAI_SIRO_DRP_NUM];//SILO��ڕW�ʒu��
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	CSilo* psilo[4];//�R�t���T�C��
	CBC* pbc;//�R�tBC


	int discharge(DWORD com, LONG dt);//�R�t��BC�֕R�t���T�C������ΒY���ڍ�
	int move(DWORD com, LONG dt, int target);//�ړ�/�ڍځ@�߂�l1�F�ړ��L��
	void set_command(DWORD com) { command |= com; };//�w���R�}���h
	void reset_command(DWORD com) { command &= ~com; };//�w���R�}���h
	DWORD get_command() { return command; };//�w���R�}���h
	void set_target(int target) { if (target < pos_min)pos_target = pos_min; else if (target > pos_max)pos_target = pos_max; else pos_target = target; };//�w���R�}���h
	int get_target() { return pos_target; }
	void set_param();//�p�����[�^��W�J

	void set_area(int pos) { 
		area.y = pbc->area.y + pos / pbc->pix2mm; 
		return; };

private:
	int pos_target;


private:

};

#define COM_HARAI_BIO_IDLE	0
#define COM_HARAI_BIO_DISCHARGE1	0x0001
#define COM_HARAI_BIO_DISCHARGE2	0x0002
#define COM_HARAI_BIO_DISCHARGE3	0x0004
#define COM_HARAI_BIO_DISCHARGE4	0x0008
#define COM_HARAI_BIO_DISCHARGE5	0x0010
#define COM_HARAI_BIO_DISCHARGE6	0x0020
#define COM_HARAI_BIO_DISCHARGE7	0x0040
#define COM_HARAI_BIO_DISCHARGE8	0x0080

#define STAT_HARAI_BIO_IDLE			0x0000
#define STAT_HARAI_BIO_DISCHARGE	0x0001


class CHaraiBio : public CMob
{
public:
	CHaraiBio() {};
	~CHaraiBio() {};

	int stat[SILO_COLUMN_NUM_BIO];//�e�����o�����u�̏��
	int com_column[SILO_COLUMN_NUM_BIO];//�e�����o�����u�ւ̃R�}���h���
	int ability;//�����o���\�́@kg/s�@900ton/h->250kg/s
	CSilo* psilo;//�R�t���T�C��
	CBC* pbc;//�R�tBC

	int discharge(DWORD com, LONG dt);//�R�t��BC�֕R�t���T�C������ΒY���ڍ�
	int move(DWORD com, LONG dt, int target);//�ړ�/�ڍځ@�߂�l1�F�ړ��L��
	void set_command(DWORD com, int i) { com_column[i] |= com; };//�w���R�}���h
	void reset_command(DWORD com, int i) { com_column[i] &= ~com; };//�w���R�}���h
	DWORD get_command() { return command; };//�w���R�}���h
	void set_param();//�p�����[�^��W�J

	void set_area(int pos) {
		area.y = pbc->area.y + pos / pbc->pix2mm;
		return;
	};

private:

private:

};

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
	static STLOAD load_base;

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
#define NUM_OF_HARAI			15
#define NUM_OF_CRUSH			2
#define NUM_OF_CUL				2
#define NUM_OF_TRIPPER			3
#define NUM_OF_SCRAPER			6
#define NUM_OF_EROOM			3
#define SILO_LINES				9	// 1��3line�@2���ݔ�2line�@BIO�@2line BANKER 2line
#define SILO_LINE_NUM			4	//1���H���e���C���̃T�C����
#define SILO_LINE_NUM2			3	//2���H���e���C���̃T�C����
#define SILO_LINE_NUM_BIO		1	//�o�C�I�e���C���̃T�C����
#define SILO_LINE_NUM_BANK		2	//�o���J�[���C���̃T�C����
#define NUM_OF_HARAI_BIO		2   //�o�C�I�T�C���̃T�C����
#define NUM_OF_SCREEN			2


//���L�������z�u��`
typedef struct _stMobsBody {
	CBC			bc[BC_LINES][BC_LINE_NUM];
	CHarai		haraiki[NUM_OF_HARAI];
	CCrush		crusher[NUM_OF_CRUSH];
	CCUL		cul[NUM_OF_CUL];
	CTripper	tripper[NUM_OF_TRIPPER];
	CEroom		eroom[NUM_OF_EROOM];
	CSilo		silo[SILO_LINES][SILO_LINE_NUM];
	CScraper	scraper[NUM_OF_SCRAPER];
	CHaraiBio	haraikiBio[NUM_OF_HARAI_BIO];
	CScreen		screen[NUM_OF_SCREEN];
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
#define LINE_H 7
#define LINE_I 8
#define LINE_D_SCR 0
#define LINE_E_SCR 1
#define LINE_F_SCR 2
#define LINE_G_SCR 3
#define LINE_H_SCR 4
#define LINE_I_SCR 5
#define DUMP1 0
#define DUMP2 1
#define DUMP3 2
#define DUMP4 3

#define BC_L1	0
#define BC_L2	1
#define BC_L3	2
#define BC_L4_1 3
#define BC_L4_2 4
#define BC_L4_3 5
#define BC_L5	6
#define BC_L6	7
#define BC_L7	8
#define BC_L8	9
#define BC_L9	10
#define BC_L9_1 11
#define BC_L9_2 12
#define BC_L11	13
#define BC_L12	14
#define BC_L13	15
#define BC_L15	16
#define BC_L16	17
#define BC_L17	18
#define BC_L18	19
#define BC_L19	20
#define BC_L20	21
#define BC_L21	22
#define BC_L22	23
#define BC_L23	24
#define BC_LRC	25

#define BC_L30 0
#define BC_L31 1
#define BC_L32 2
#define BC_L33 3
#define BC_L34 4
#define BC_BIO 5

#define HARAI_11A	0
#define HARAI_11B	1
#define HARAI_11C	2
#define HARAI_12A	3
#define HARAI_12B	4
#define HARAI_12C	5
#define HARAI_13A	6
#define HARAI_13B	7
#define HARAI_13C	8
#define HARAI_15A	9
#define HARAI_15B	10
#define HARAI_15C	11
#define HARAI_16A	12
#define HARAI_16B	13
#define HARAI_16C	14
#define HARAI_17A	0
#define HARAI_17B	1



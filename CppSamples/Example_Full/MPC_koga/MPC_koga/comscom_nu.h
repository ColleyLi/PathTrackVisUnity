#pragma once

/// COMSCOM�v���W�F�N�g�p���L�f�[�^�\���̒�`�t�@�C��
/// ���̃t�@�C����ҏW�����ꍇ�Cc#����comscom.cs�����킹�ďC�����邱�ƁD

/** 
	�����n���Ƃ̋��L�������̍\���F
	��n�ǁF
		COMSCOM
		COMS_SENSOR0  COMS_SENSOR1  COMS_SENSOR2
		COMS_CONTROL0 COMS_CONTROL1 COMS_CONTROL2

	�e�ԗ� (n�͎ԗ�ID, 0,1,2�j�F
		COMSCOM
		COMS_SENSORn
		COMS_CONTROLn
		
	- COMSCOM�̓�����TinyBee2Share���s��
	- �e�ԗ���COMS_SENSORn, COMS_CONTROLn�̊�n�ǂւ̃A�b�v���[�h��WiFi2Share���s���i�������j
 **/

#define RENEW_DATA_STRUCT


#include <cstring>

#pragma pack(push, 8)

/// ################## ���� COMS C#�����v���Z�X ���L�f�[�^�\���� ########################
/// <summary>
/// �ЂƂ���LRF�f�[�^���L�p�\����
/// </summary>
/// 1024�_�܂ł̌v�����ƁC�e���C�̊p�x���𑗂�D
/// �p�x�͕�����LRF�����C�ɂ������邽�߂ɗp�ӂ������C
/// ����Ȃ������D(or�n�_�C�I�_�p�x��������Ɛߖ�Ɂc)
struct LRF_DATA
{
	enum {
		MAX_NUM_OF_RAY = 1024,
	};

	//float position[3];                  ///< LRF���t���ʒu�i�ԑ̍��W[m]�j
	//float orientation;                  ///< LRF���t�������i�ォ�猩�Ĕ����v���[rad]�j
    int   num_of_ray;
    float range [MAX_NUM_OF_RAY];		///< �����f�[�^[m]
	float angle [MAX_NUM_OF_RAY];		///< �p�x�f�[�^[deg] �O����0deg
	//float origin[2];					///< �_�Q�`�掞�ɕK�v�Ȍ��_
	//float points[2*MAX_NUM_OF_RAY];		///< �_�Q�f�[�^

    LRF_DATA(int x=0)
    {
		num_of_ray = x;
		//memset(position, 0, sizeof(position));
		//orientation = 0.0f;
		memset(range , 0, sizeof(range ));
		memset(angle , 0, sizeof(angle ));
		//memset(origin, 0, sizeof(origin));
		//memset(points, 0, sizeof(points));
    }
};

/// <summary>
/// �^�C���X�^���v����COMS����聨���v���Z�X�ɒʒm����\����    
/// </summary>
struct COMS_TIME
{
    /// ����t���[���ԍ�[step] (�����v���O�������N���b�N�𐧌�)
    int t_frame;
    /// ����J�n����̎���[msec] (�����v���O�������珑������)
    float time_elap;
    /// �T���v�����O����(��l)
    float time_Ts;
    /// �T���v�����O����(�����l)
    float time_intvl;

    /// ����
    COMS_TIME(int x = 0)
    {
        t_frame = 0;
        time_elap = 0;
        time_Ts = 0;
        time_intvl = 0;
    }
};

/// <summary>
/// �G���R�[�_�⑼�Z���T���CLRF�ȊO�̃f�[�^���L�p�\����
/// </summary>
struct SENSOR_DATA
{
    //int l_enc;       /// ���G���R�[�_�J�E���g�l
    //int r_enc;       /// �E�G���R�[�_�J�E���g�l
	//int l_enc_diff;  /// ���G���R�[�_����(���b�v�ς�)
    //int r_enc_diff;  /// �E�G���R�[�_����(���b�v�ς�)
	//float pot_accel;	/// �A�N�Z���y�_���|�e���V��(0:���܂Ȃ��`1:max����)
	//float pot_brake;	/// �u���[�L�y�_���|�e���V��(0:���܂Ȃ��`1:max����)
	//float pot_steer;	/// �X�e�A�����O  �|�e���V��(-1:���`1:�E)(�����͕ς��\��������܂�)	
	//float frc_accel;	/// �A�N�Z���y�_������
	//float frc_brake;	/// �u���[�L�y�_������
    float l_vel_mps;   /// ���^�C�����x(m/s)
    float r_vel_mps;   /// �E�^�C�����x(m/s)
	float steer_angle;   ///< ���ǐ؂�p[rad]�i�X�e�A�����O�|�e���V���d������̊��Z�l�j
    float tire_angle;	///< �^�C���p[rad]

	SENSOR_DATA(){
		l_vel_mps = 0.0f;
		r_vel_mps = 0.0f;
		steer_angle = 0.0f;
		tire_angle = 0.0f;
	}
};

#ifdef RENEW_DATA_STRUCT
struct ODOMETRY_DATA{
	float v_mps;		/// �I�h���g���ɂ��ԗ��O�㑬�x(m/s)
    float w_radps;		/// �I�h���g���ɂ��ԗ��p���x(rad/s)
	float trip;			/// ����J�n������̑��s����(v_mps�̐��l�ϕ�)
	float lcl_x;		/// ����J�n�������x�D�����p���ɂ�����O����������D
    float lcl_y;		/// ����J�n�������y�D�����p���ɂ����鍶�E���������D
	float angle;		/// ����J�n���̌�����0�Ƃ������̊p�x(w_radps�̐��l�ϕ�)
};
#else
struct ODOMETRY_DATA{
	float v_mps;		/// �I�h���g���ɂ��ԗ��O�㑬�x(m/s)
    float w_radps;		/// �I�h���g���ɂ��ԗ��p���x(rad/s)
	float trip;			/// ����J�n������̑��s����(v_mps�̐��l�ϕ�)
	float angle;		/// ����J�n���̌�����0�Ƃ������̊p�x(w_radps�̐��l�ϕ�)
	float lcl_x;		/// ����J�n�������x�D�����p���ɂ�����O����������D
    float lcl_y;		/// ����J�n�������y�D�����p���ɂ����鍶�E���������D
};
#endif 

#ifdef RENEW_DATA_STRUCT
/// <summary>
/// �O���[�o���ʒu�����DGPS����擾�����l�D
/// </summary>
/// GPS�Ŏ擾�����ܓx�C�o�x�ȂǁD��Έʒu�ŁD // added by Okuda, 2016 03 03
struct SEMIGLOBAL_DATA
{
    float y_diff;     /// distance from 35deg 10min in N-S direction
    float x_diff;    /// distance from 137deg + 5min in E-W direction
    float z;		    /// altitude
    float roll;              /// roll by GNSS
    float pitch;             /// pitch by GNSS
    float yaw;		        /// yaw by GNSS
    int state;       // state of GNSS                                    
};
#endif

struct GPS_DATA{
	/// ���V�[�o�����M������
	float	time;			///< UTC�^�C���]�[���ł̎��� hhmmss.ss
	float	latitude;		///< �ܓx 1234.56�Ȃ�k��12�x34.56��
	float	longitude;		///< �o�x 1234.56�Ȃ瓌�o12�x34.56��
	int		quality;		///< �i��
	int		numSats;		///< �v�Z�Ɏg��ꂽ�q����
	float	dilution;		///< ���x
	float	altitude;		///< �W���i�W�I�C�h�ʂ���̍����j
	float	geoidHeight;	///< �ȉ~�̂���̃W�I�C�h�ʂ̍���

	/// ���Z���ē�������
	float	latMeter;		///< �o�x�̃��[�g�����Z
	float	lonMeter;		///< �ܓx�̃��[�g�����Z
};

struct JOYSTICK_DATA{
	/// �X�e�B�b�N�̖���
	struct Axis{
		enum{ H1, V1, H2, V2, Num };
	};
	/// �{�^���̖���
	struct Button{
		enum{ Y, X, B, A, L1, R1, L2, R2, Left, Right, Select, Start, Num };
	};

	/// �X�e�B�b�N�ψʂ͈̔�
	enum{
		AxisMin = -32768,
		AxisMax =  32767,
	};

	short	axis  [Axis  ::Num];
	short	button[Button::Num];

	JOYSTICK_DATA(){
		for(int i = 0; i < Axis::Num; i++)
			axis[i] = 0;
		for(int i = 0; i < Button::Num; i++)
			button[i] = false;
	}
};

/// ################## ���Ȉʒu����v���Z�X�����v���Z�X�ւ̍\���� ########################
/// <summary>
/// ���Ȉʒu���茋�ʂ������o�����L�������̈�
/// </summary>

struct LOCALIZE_DATA{
	float g_x;		/// ���Ȉʒu����v���O�����ɂ��O���[�o���ʒux(m)
    float g_y;		/// ���Ȉʒu����v���O�����ɂ��O���[�o���ʒuy(m)
	float g_yaw;	/// ���Ȉʒu����v���O�����ɂ��O���[�o���ʒuyaw(rad)

#ifdef RENEW_DATA_STRUCT
	float g_z;		/// added by Okuda @ 2016_03_07 to keep consistency with comscom_nu.cs
#endif 
	float trip;		/// ����J�n������̑��s����(m)
	float v_mps;	/// ���s���x(m/s)
	float w_radps;	/// �p���x(m/s)
};
		
#define COMS_LOCALIZE_NAME ("COMS_LOCALIZE")
#define COMS_LOCALIZE_MUTEX_NAME ("COMS_LOCALIZE_MUTEX")
struct COMS_LOCALIZE
{
	/// ���ԏ��(�ŐV�f�[�^���v�Z���ꂽ�ۂ�)
	COMS_TIME time;
	/// ���Ȉʒu���茋��
	LOCALIZE_DATA state;
	/// �p�[�e�B�N���ޓx�Ȃ�
	float Likelihood;

};

/// <summary>
/// Information of leading vehicle for following
/// </summary>
/// Information of leading car and relative info between ego and lead car
struct LEADINGCAR_DATA
{
    float range;         /// Distance to rear of leading vehicle (m)
    float range_rate;    /// Relative velocity (m/s)
    float speed;		    /// Driving speed of leading car (m/s)
    float lateral;       /// Lateral displacement (m)
    float direction;     /// Direction to leading vehicle (rad)

};


/**
	COMS��CS�v���O��������C���̃v���Z�X�Ɍ�������񔭐M�p�\����
 **/
#define COMS_SENSOR_NAME ("COMS_SENSOR")
#define COMS_SENSOR_MUTEX_NAME ("COMS_SENSOR_MUTEX")
struct COMS_SENSOR
{
    ///-------------------------- ���ԊǗ��� -----------------------
    COMS_TIME		time;

    ///-------------------------- �Z���V���O�f�[�^ -----------------------
    LRF_DATA		lrf_front;
	LRF_DATA        lrf_rear;
	LRF_DATA        lrf_right;
    LRF_DATA        lrf_left;

	SENSOR_DATA		sense_data;

	ODOMETRY_DATA	odo_data;

    /// GPS�ɂ�郍�[�J���C�Y���
    LOCALIZE_DATA gps_lcl_data;

    /// ���Ȉʒu���
    LOCALIZE_DATA lcl_data;

    /// �Ǐ]�f�[�^
    LEADINGCAR_DATA lcar_data;

#ifdef RENEW_DATA_STRUCT
	/// semi global from GPS
	SEMIGLOBAL_DATA semiglobalpos_data;
#endif 

	///-------------------------- ������� -----------------------
    //JOYSTICK_DATA   joy_data;

    static const char* GetLRFSuffix(int idx){
        switch (idx){
            case 0: return "front";
            case 1: return "rear";
            case 2: return "right";
            case 3: return "left";
        }
        return "";
    }
	LRF_DATA* GetLRF(int idx){
		switch (idx){
            case 0: return &lrf_front;
            case 1: return &lrf_rear ;
            case 2: return &lrf_right;
            case 3: return &lrf_left ;
        }
        return 0;
	}
    
    COMS_SENSOR(){}
};

/// �ԗ�(COMS)�ւ̑���ʋ��L�p�\����
/// ################## ���� ���v���Z�X��COMS C# ���L�f�[�^�\���� ########################
/// <summary>
/// ���傻�̑��v���Z�X �� COMS����v���O����(C#)�̃f�[�^���L�p�\����
/// </summary>

/// <summary>
/// ����ʋ��L�p�\����
/// </summary>
/// 
struct COMS_OUTPUT
{
	int odr_type;            ///< �w�߃^�C�v�B 0 : �g���N�A 1 : ���x+�^�C���p
    float vel_ref;

    float acc_ref;           ///< �w�߉����x�l(m/s2)�F�ǉ�by���c

    float tire_angle_ref;
    float accel_trq;         ///< �A�N�Z���ʃg���N(�ЂƂ܂����K��0�`1) 0:���� 1:max
    float brake_trq;         ///< �u���[�L�ʃg���N(�ЂƂ܂����K��0�`1) 0:���� 1:max
    float steer_trq;         ///< �n���h���ʃg���N(�ЂƂ܂����K��-1�`1) -1:��max  0:���� 1:�Emax
    int gearR;				///< ���o�[�X�M�A  0:�j���[�g���� 1:R�M�A
	int gearD;				///< �h���C�u�M�A  0:�j���[�g���� 1:D�M�A
	int winkerL;				///< ���E�B���J�[  0:���� 1:�_��
	int winkerR;				///< �E�E�B���J�[  0:���� 1:�_��
};

/// <summary>
/// �O�����L�p�\����
/// </summary>
struct PATH_DATA
{
    int		num_of_points;
	float	x[1024];
	float	y[1024];
	float	yaw[1024];

	PATH_DATA(int n=0)
    {
        num_of_points = n;
		memset( (void*)x, 0, sizeof(x));
		memset( (void*)y, 0, sizeof(y));
		memset( (void*)yaw, 0, sizeof(yaw));
    }
};

/// <summary>
/// ���肵�����Ȉʒu�C�ʐM�œ������̓��C���ԏ����܂߂����L�p�\����
/// </summary>
#define COMS_CONTROL_NAME ("COMS_CONTROL")
#define COMS_CONTROL_MUTEX_NAME ("COMS_CONTROL_MUTEX")
struct COMS_CONTROL
{
	// COMS�Ɏw�肵���������
    COMS_OUTPUT coms_output;

	/// �ڕW�O��
	PATH_DATA	path_data;

	COMS_CONTROL(int x = 0)
    {
    }
};

//





#pragma pack(pop)

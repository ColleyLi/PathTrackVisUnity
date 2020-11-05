#pragma once

//#include "../Cars/Cars.h"
#include "CarsimImple.h"
#include "Course.h"
//#include "../utils.h"


namespace CarManagement{

		struct Gain{
		double e, de, ie, prev_e, Kp, Ki, Kd, dt;

		double PID(double speed_ref, double speed, double pedal_ref){
			double pedal;

			e = speed_ref - speed;

			//�덷����
			de = (e - prev_e)/dt;

			//�덷�ϕ�
			ie = e + de * dt;

			prev_e = e;

			return pedal = Kp * e + Ki * ie + Kd * de + pedal_ref;
		}

		Gain(){
			prev_e = 0;
			dt = 0.1;
		}

		void SetParam(double n1, double n2, double n3){
			Kp = n1;
			Ki = n2;
			Kd = n3;
		}

	};


	struct ShrCarState{
		int m_carID; // �Ԃ�ID(�C���f�b�N�X�Ɠ�����)
		int m_carTypeID; // �Ԏ����肷�邽�߂�ID�C��ɕ\���p(���b�V�����X�g����K�X�I��)
		int m_groupID; // �Ԃ̃O���[�v����肷��ID(���ԁC���Ԑ��ԁC�E�Ԑ��ԁC�Ό��ԁc)

		// ���W�n��2�ʂ�Ƃ肠���������Ă����C�ǂ���������ɗp���Đ��䂷�邩�̓T�u�N���X�ɔC����
		double m_u, m_v, m_w; // �R�[�X�ɉ��������W(m_u�͎��񐔊܂߂��l�Ŏ���)
		double m_x, m_y, m_z; // �O���[�o���ȍ��W
		double m_roll, m_pitch, m_yaw; // // �O���[�o���ȍ��W�n�ɑ΂���p��(roll->pitch->yaw) -pi/2�`pi/2
		double m_speed; // ���i���x
		double m_accel; // ���i���������x

		// added for bycycle model control by Okuda @ 2014 10/29
		double m_slip_angle; // Slip angle of vehicle

		// �X���b�g���C�u���[�L�C�X�e�A�����O�D
		// ���ۂɎԗ��ɓK�p����鑀���
		double m_rpm;		// �G���W����]��(added by ���c)	//�ȉ��C���Ԃ����삷��\��������̂�
		double m_throtpdl;	// �X���b�g��(added by ���c)
		double m_brakepdl;	// �u���[�L(added by ���c)
		double m_steering;	// �X�e�A�����O(added by ���c)

		// ���� : added by okuda 2014/07/27
		//double m_throttle_input;
		//double m_brake_input;
		//double m_steering_input;

		// �t���O�Ƃ�
		// 0bit: �������ꂷ���t���O��pattern_gentle_lead
		// 1-bit : ��
		int m_flg;

		ShrCarState() : m_groupID(0), m_carID(0), m_carTypeID(0),
			m_u(0), m_v(0), m_w(0), m_x(0), m_y(0), m_z(0), 
			m_roll(0), m_pitch(0), m_yaw(0), m_flg(0), 
			m_speed(0), m_accel(0),m_rpm(0), m_throtpdl(0), m_brakepdl(0), m_steering(0),
			m_slip_angle(0)
		{}
	};

	// �Ԃ������X�[�p�[�N���X
	class Car :public ShrCarState, public Gain
	{
	public:

		//////////// car fundamental information ///////////////

		int m_drivingPatternID; // �Ԃ̋����̎�ނ���肷��ID
		int m_courseID; // �Ԃ��Ƃɑ��点�郋�[�g��ς�����ł���悤�ɁC�R�[�X��ID�ɂ���ĕʂ̂ɂ���

		Course *m_course; // �Ԃ��ƂɃR�[�X��ς�����悤�ɃR�[�X�ւ̃|�C���^����������

		// �Ԃ̏�ԍX�V���s���p�^�[���N���X�̊��|�C���^
		// update()����Ăяo�����
		// ��{�I�ɂ��̊֐���ς��邱�ƂŁC�Ԃ̃p�^�[����ς���悤�ɂ���
		// �����ƍ��{�I�ɕς�������΁Cupdate()���I�[�o�[���C�h����ׂ�
		//PatternBase *m_pattern;
		//std::string m_patternfile; // �p�^�[����ǂݍ��ރt�@�C��(�g��Ȃ��ꍇ��)

		// Car�N���X������CarManager���Q�Ƃł���悤�Ƀ|�C���^���������Ă���
		//static CarManager *cm;

		// �Փ˔���I�u�W�F�N�g
		//OBB_Character m_col_tester;

		//////////////////////  interfaces  ////////////////////////////////

		// �h���N���X�̃f�X�g���N�^���Ăяo�����悤��virtual�錾
		Car();
		virtual ~Car();	   

		// ID�Z�b�^�[
		//void setIDs(int carID, int carTypeID, int drivingPatternID, int groupID);

		// �p�����v�Z����֐�
		void calcRollPitchYaw();
		// ���͂��o��
		virtual int GetReactionForce(){return 0;};

		// �O���t�B�b�N��ύX
		int SetMeshID(int n);

		// �ȉ��K�v�ɉ����ăI�[�o�[���C�h���Ă�������

		// ���O�֐�
		//virtual void DataLog(DSDataUnit& tmpdata)
		//{}

		// �Ԃ̈ʒu(�Ȃ�)�X�V�֐�
		virtual void update(double t, double dt);
		// �Ԃ̈ʒu(�Ȃ�)���Z�b�g�֐�
		virtual void reset();

		// �O���Ԃ��擾
		virtual Car *getFrontCar() const = 0;

		// �������W�n�ɑ΂���get�֐�
		virtual double getX() const = 0;
		virtual double getY() const = 0;
		virtual double getZ() const = 0;
		virtual void getXYZ(double *x, double *y, double *z) const = 0;
		// �R�[�X���W�n�ɑ΂���get�֐�(u�͎��񐔂��܂߂��R�[�X�ɉ��������s����)
		virtual double getU() const = 0;
		virtual double getV() const = 0;
		virtual double getW() const = 0;
		virtual void getUVW(double *u, double *v, double *w) const = 0;

		//
		void SetCarState(const ShrCarState &obj);
	};



	class CarsimCar2014 : public Car
	{
	public:
		// carsim manager
		CarsimImple* carsim_imple;
		int integrate_repetation;

		// states

		// extended state
		double m_init_u;
		double m_init_v;
		double m_init_yaw;

		double m_init_x;
		double m_init_y;
		double m_init_z;

		//double pure_throttle;
		//double pure_brake;
		//double pure_steer;

		double m_fuel_used;

		double m_speed_ref; // kmph
		double m_steer_ref;	// deg on steer

		double m_tire_angle_ref;	// rad on tire
		double m_vel_ref;	// mps
		double m_acc_ref;

		std::string sim_file_name;

		CarsimCar2014(std::string sim_fn)
		{
			carsim_imple = NULL;
			integrate_repetation = 1;
			sim_file_name = sim_fn;
			LoadCarsim(sim_file_name);

			m_fuel_used = 0;
					// extended state
			m_init_u = 0;
			m_init_v = 0;
			m_init_yaw = 0;

			m_init_x = 1.15;
			m_init_y = 0;
			m_init_z = 0;

			m_fuel_used = 0;

			m_speed_ref = 0;
			m_steer_ref = 0;

			m_tire_angle_ref = 0;
			m_vel_ref = 0;

		}

		~CarsimCar2014(void)
		{
			ReleaseCarsim();
		}

		void LoadCarsim(std::string sim_fn)
		{
			try{
				carsim_imple = new CarsimImple(sim_fn);
			}
			catch(...){
				carsim_imple = NULL;
				return;
			}
			// create entry of map to set data
			carsim_imple->input_variable_map["IMP_STEER_SW"] = NULL;
			carsim_imple->input_variable_map["IMP_THROTTLE_ENGINE"] = NULL;
			carsim_imple->input_variable_map["IMP_FBK_PDL"] = NULL;

			// create entry of map to extract data
			carsim_imple->output_variable_map["XCG_TM"] = NULL;
			carsim_imple->output_variable_map["YCG_TM"] = NULL;
			carsim_imple->output_variable_map["YAW"] = NULL;

			carsim_imple->output_variable_map["VX"] = NULL;
			carsim_imple->output_variable_map["VY"] = NULL;
			carsim_imple->output_variable_map["AVZ"] = NULL;	// Yaw rate

			// Add by Okuda @ 2014 10/28, for TTDC
			carsim_imple->output_variable_map["BETA"] = NULL;	// Vehicle slip angle

			carsim_imple->output_variable_map["TSTART"] = NULL;
			carsim_imple->output_variable_map["PBK_CON"] = NULL;
			carsim_imple->output_variable_map["MFUEL"] = NULL;
			carsim_imple->output_variable_map["AV_ENG"] = NULL;
			//carsim_imple->output_variable_map["GEARAUTO"] = NULL;

			//carsim_imple->set_def_by_map();
		}


		void ReleaseCarsim()
		{
			if(carsim_imple!=NULL)
			{
				delete carsim_imple;
				carsim_imple = NULL;
			}
		}

		void SetState( InfoToCarsim &state )
		{

			*carsim_imple->input_variable_map["IMP_STEER_SW"]= state.IMP_STEER_SW;
			*carsim_imple->input_variable_map["IMP_THROTTLE_ENGINE"]= state.IMP_THROTTLE_ENGINE;
			*carsim_imple->input_variable_map["IMP_FBK_PDL"]= state.IMP_FBK_PDL;

		}

		InfoFromCarsim GetState()
		{
			InfoFromCarsim tmp;
			tmp.AVZ = *carsim_imple->output_variable_map["AVZ"];
			tmp.AV_ENG = *carsim_imple->output_variable_map["AV_ENG"];
			//tmp.GEARAUTO = *carsim_imple->output_variable_map["GEARAUTO"];
			tmp.MFUEL = *carsim_imple->output_variable_map["MFUEL"];
			tmp.PBK_CON = *carsim_imple->output_variable_map["PBK_CON"];
			tmp.TSTART = *carsim_imple->output_variable_map["TSTART"];
			tmp.VX = *carsim_imple->output_variable_map["VX"];
			tmp.VY = *carsim_imple->output_variable_map["VY"];
			tmp.XCG_TM = *carsim_imple->output_variable_map["XCG_TM"];
			tmp.YAW = *carsim_imple->output_variable_map["YAW"];
			tmp.YCG_TM = *carsim_imple->output_variable_map["YCG_TM"];

			// slip angle
			tmp.BETA = *carsim_imple->output_variable_map["BETA"];
			return tmp;
		}


		// override how to run
		int run_all()
		{
			return carsim_imple->run_all();
		}

		int run_init_step()
		{
			return carsim_imple->run_init_step();
		}

		int run_integrate()
		{
			carsim_imple->integrate_count = integrate_repetation;
			return carsim_imple->run_integrate();
		}

		int run_terminate()
		{
			return carsim_imple->run_terminate();
		}

		// 
		virtual void reset()
		{
			run_terminate();
			ReleaseCarsim();
			LoadCarsim(sim_file_name);

			run_init_step();
		}

		virtual void updateInfo(double t, double dt) 
		{
			// to implement
			InfoFromCarsim tmp = GetState();

			double x = tmp.XCG_TM;
			double y = tmp.YCG_TM;
			// m_z = car_info.pos.z - 1.5;// skip this
			m_roll = 0;
			m_pitch = 0;
			m_yaw = tmp.YAW;
			m_rpm = tmp.AV_ENG  / 111.31f * 1062.92f;

			m_slip_angle = tmp.BETA;
			m_fuel_used = tmp.MFUEL;

			m_speed = tmp.VX;
			static double prev_m_speed = 10e30;
			if( prev_m_speed == 10e30 )
			{
				m_accel = 0;
			}else{
				m_accel = (m_speed - prev_m_speed)/3.6/dt;
			}
			prev_m_speed = m_speed;

			// transform positions
			m_x = m_init_x + x * cos(m_init_yaw) - y * sin(m_init_yaw);
			m_y = m_init_y + x * sin(m_init_yaw) + y * cos(m_init_yaw);
			
			getUVW(&m_u, &m_v, &m_w);

		}





		void UpdateOutput()
		{
			
			// compute steering from tire-angle

			const double steering_sense_ratio = 1.0;
			//m_steer_ref = m_tire_angle_ref;
			// steer by force feed back
			// steer_deg_control( steer_ref / steering_sense_ratio );
			// steer by wire ident
			m_steer_ref = m_tire_angle_ref / 0.058917; // Steering vs tire angle gear ratio

			m_steering = m_steer_ref;

			// compute throttle from speed order
			m_speed_ref = m_vel_ref / 3.6f;
			double target_speed = m_speed_ref;

		
#ifdef VEL_CONTROL
			//// ���x���䃂�[�h
			if (m_speed < (target_speed - 5/3.6)){
				m_throtpdl = 5.0;
				m_brakepdl = 0;
			}
			else if (m_speed < target_speed){

				const double k_p = 10.0;
				const double k_i = 0.1;

				double error = target_speed - m_speed;
				static double i_e = 0;
				i_e += error * dt;

				m_throtpdl = error * k_p + i_e * k_i;
				m_brakepdl = 0;
			}
			else if (m_speed > target_speed){
				m_throtpdl = 0;
				m_brakepdl = 0.2;
			}
#else
			if(m_throtpdl>1.0) m_throtpdl=1.0;
			if(m_throtpdl<0) m_throtpdl=0;

			if(m_brakepdl>1.0) m_brakepdl=1.0;
			if(m_brakepdl<0) m_brakepdl=0;

			//if (m_acc_ref > -3) //�A�N�Z��
			//{
			//  if (m_throtpdl < 0) m_throtpdl = 0;
			//  if (m_throtpdl > 1) m_throtpdl = 1;

			//	m_brakepdl = 0;
			//}

			//else if(m_acc_ref > -5) //�G���W���u���[�L
			//{
			//	m_throtpdl = 0;
			//	m_brakepdl = 0;
			//}

			//else //�u���[�L
			//{
			//	m_throtpdl = 0;
			//	m_brakepdl = 0.2;
			//	//if (m_brakepdl < 0) m_brakepdl = 0;
			//	//if (m_brakepdl > 1) m_brakepdl = 1;
			//}
			
			//double d = 0.7;
			//m_throtpdl = d * floor(m_throtpdl / d + 0.5); 


#endif

			//Carsim����ɑ���f�[�^�𑗂�
			InfoToCarsim tocarsim;
			//m_steering_input = -m_steering*0.02;
			//m_steering = m_steering_input;
			tocarsim.IMP_STEER_SW = m_steering;
			tocarsim.IMP_FBK_PDL = m_brakepdl*150;
			tocarsim.IMP_THROTTLE_ENGINE = m_throtpdl;

			SetState(tocarsim);

			run_integrate();

			// get height			
			//double tmpx, tmpy, tmpz;
			//boost::tuples::tie( tmpx, tmpy, tmpz ) = cm->terrainX->getHeight( (float)m_x , (float)m_y, (float)m_z + 1.5f, 0.0f);
			m_z = 0;//tmpz;


			// integration of mfuel;




			// �Փ˔��肨�Ԃ������Ƃ��X�V
			//m_col_tester.SetPositionAndPosture( (float)m_x, (float)m_y, (float)m_z, (float)m_roll, (float)m_pitch, (float)m_yaw);

		}

		double getX() const
		{
			double x, y, z;
			getXYZ(&x, &y, &z);
			return x;
		}
		double getY() const
		{
			double x, y, z;
			getXYZ(&x, &y, &z);
			return y;
		}
		double getZ() const
		{
			double x, y, z;
			getXYZ(&x, &y, &z);
			return z;
		}
		void getXYZ(double *x, double *y, double *z) const
		{
			*x = m_x;// + m_offX;
			*y = m_y;// + m_offY;
			*z = m_z;// + m_offZ;
		}

		double getU() const
		{
			double u, v, w;
			getUVW(&u, &v, &w);
			return u;
		}
		double getV() const
		{
			double u, v, w;
			getUVW(&u, &v, &w);
			return v;
		}
		double getW() const
		{
			double u, v, w;
			getUVW(&u, &v, &w);
			return w;
		}
		void getUVW(double *u, double *v, double *w) const
		{
			double x, y, z;
			getXYZ(&x, &y, &z);
			m_course->XYZ2UVW(x, y, z, u, v, w);
		}
		void setStatus(double x, double y, double z, double roll=0, double pitch=0, double yaw=0)
		{
			//m_offX = x; m_offY = y; m_offZ = z;
			m_init_x = x; 
			m_init_y = y;
			m_init_yaw = yaw;

			run_init_step();

			calcUVW();	
		}

		void calcUVW()
		{
			getUVW(&m_u, &m_v, &m_w);
		}

		void setStatusByUVW(double u, double v, double w, double roll, double pitch, double yaw)
		{
			double x, y, z;
			m_course->UVW2XYZ(u, v, w, &x, &y, &z);

			int chkpt = m_course->getPrevCheckPointIndex( u, v, w);
			//double roll, pitch, yaw;
			m_course->checkPoint[chkpt].getRollPitchYaw( &roll, &pitch, &yaw );

			m_init_u = u;
			m_init_v = v;
			m_init_yaw = yaw;

			m_init_x = x;
			m_init_y = y;
			m_init_z = z;

			//carsim->StopSim();
			//carsim->Reset( x, y, z, roll, pitch, yaw);
			//carsim->RunSim();
			//setStatus(x, y, z, roll, pitch, yaw);
		}

		Car *getFrontCar() const
		{
			/*			double u = m_u;
			double min_u = 10e10;
			int target = -1;
			for(int i=0; i < cm->cars.size(); ++i)
			{
			// the car is front of me
			if(cm->cars[i]->m_u > m_u )
			{
			if(min_u > cm->cars[i]->m_u)
			{
			min_u > cm->cars[i]->m_u;
			target = i;
			}
			}
			}
			if(target==-1)return NULL;
			return cm->cars[target];*/
			return NULL;
		}
	};

}
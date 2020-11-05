
// SMDriveCpp.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <string.h>
#include "direct.h"
#include "PacketToDSViz.h"
#include <minwinbase.h>
#include <random>

//#include "Course.h"

using namespace std;
using namespace Eigen;



#include "optdata_st.h"
#include "MPCSates.h"
#include "parameters.h"

// for small functions
#include "utils.h"
#include "CarDynamicsDelegator.h"
#include "lateral_control.h"
#include "longitudinal_control.h"

void DataTranslationToUnity(
	int ctrl_cnt, double time_elap,
	LOG_DATA car_state, UDPSender &udp_data);


int _tmain(int argc, _TCHAR* argv[])
{

	RTCLib::StopWatch keisoku;	//�v���p
	RTCLib::StopWatch stp;		//�������
	RTCLib::StopWatch perform;	//�p�t�H�[�}���X����p
	RTCLib::StopWatch msr;

	bool LoopAlive  = true;
	int ctrl_cnt = 0;
	
	//-----------------------------------------------------
	// for debug process
	RTCLib::SharedMemoryAccessor<MPCStates> shr_fordebug;
	shr_fordebug.TryOpen( "MPC_DEBUG_INFO", "MPC_DEBUG_INFO_MUTEX");

	//-----------------------------------------------------

	////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////// Parameter
	//////////////////////////////////////////////////////////// �p�����[�^�ݒ�
	ParamFromSharedMemory shared_param;

	// �����Ńp�����[�^�̃f�t�H���g��ݒ肷��D
	StateFBControlParameter ssbParam;

	// �o�͂̓X�e�A�����O�p�x���H�@false�Ȃ�p���x
	ssbParam.IsSteeringOutputIsDelta = 1;
	// �f�b�h�]�[�����g����
	ssbParam.IsDeadZoneEnabled = 0;
	// �f�b�h�]�[���̏o�̓I�t�Z�b�g��0�ɂ��邩
	ssbParam.IsDeadZoneOffsetCanceled = 0;

	ssbParam.IsLookAheadEnabled = 1;

	// ����Q�C���Fy_e, y_e_dot, theta_e, theta_e_dot;
	ssbParam.K_y_e = -0.1;
	ssbParam.K_y_e_dot = 0;
	ssbParam.K_th_e = -0.1;
	ssbParam.K_th_e_dot = 0;

	ssbParam.dd_K_y_e = 0.1;
	ssbParam.dd_K_y_e_dot = 0;
	ssbParam.dd_K_th_e = 0.1;
	ssbParam.dd_K_th_e_dot = 0;

	ssbParam.l_lookahead = 5;

	ssbParam.n_mov_avg = 50;
	// ���S�Ɍ��藝����C�W���΍���1 / sqrt(N)�ɂȂ�̂�
	ssbParam.sigma = 0.1 * sqrt(ssbParam.n_mov_avg);

	// �f�b�h�]�[��臒l�Fy_e, y_e_dot, theta_e, theta_e_dot;
	double dead_zone_minus[4] = { -0.1, -0.1, 0, 0 };
	double dead_zone_plus[4] = { 0.1, 0.1, 0, 0 };

	// �������L���������L���Ȃ�p�����[�^���Ƃ��Ă��ď㏑��
	shared_param.UpdateParamFromSharedMemory(ssbParam);


	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////







	UDPSender udp_data;
	udp_data.Open("127.0.0.1", 30001);

	// Launch data viewer if it is possible
#ifdef ENABLE_RESULT_VISUALIZER
	try{
	ShellExecuteA(NULL, "open", "COMSMPCDataView.exe", NULL, NULL, SW_SHOWNORMAL);
	}catch(...)
	{
	}
#endif
	//-----------------------------------------------------

	// Tracking reference  -> for look ahead control
	CarManagement::Course* ref_course;
	// this is instantiated in CarDynamicsDelegator

	// ��ԃ��t�@�����X�̓ǂݍ���
	RTCLib::CSVLoader map(DRIVING_MAP,1);
	
	
	//parameters from csv
	RTCLib::CSVLoader prm("parameter.csv",1);

	// read true path of sim file
	std::ifstream ifs("c:\\simfile_place.txt");
	std::string new_fn;
	ifs >> new_fn;
	ifs.close();

	// ���H�Z���^�[�̂��߂̃R�[�X(�ȑO�̎Ԑ������R�[�X���W���v�Z���邽��)
	auto_ptr<CarManagement::Course> lane_course( new CarManagement::Course(DRIVING_LANE_CENTER));
	RTCLib::CSVLoader lane_map(DRIVING_LANE_MAP, 1);


	// vehicle dynamics computer
	CarDynamicsDelegator car(new_fn);

	// lateral controller
	std::shared_ptr<LateralControl> lat_ctrl(new LateralControl());
	//std::shared_ptr<LongitudinalControl> lon_ctrl(new LongitudinalControl());
	std::shared_ptr<CarModel> car_model(new CarModel());
	std::shared_ptr<ControlParameter> ctrl_param(new ControlParameter());
	
	// data logger:
	RTCLib::DataStorage<LOG_DATA> logger;
	LOG_DATA::RegisterMember(logger);

	keisoku.start();

	

	//�ϐ��A�萔�̐錾////////////////////////////////////////////////
	//�c�p
	double acc; //�ڕW�����x

	//���p
	double yaw; //���݂̎ԗ��̃��[�p
	double tire_angle; //���݂̎ԗ��̃^�C���p
	double v; //�ڕW�i�\���j�ԗ����x
	
	//����
	double u;	//���݂̎ԗ���u���W
	double vel;//���݂̎ԗ��̑��x
	double y_e, y_e_dot, theta_e, theta_e_dot, theta_d; //��ԕϐ�
	double input_delta = 0;	// ���̓X�e�A�����O�p�x
	double pre_input = 0;

	const double pi = 3.141592654;
	//////////////////////////////////////////////////////////////////

	//for...
	//for (int h=0; h<3; h++){

	//�ϐ��A�萔�̏�����//////////////////////////////////////////////
	int h = 0;
	opt_slt& ltrl = lat_ctrl->CreateParameter(prm,h);	//�X�e�A�����O
	// opt_slt& lgtd = lon_ctrl->CreateParameter(prm,h);	//���x
	parameter& car_mdl = car_model->SetParameter(prm,h);	//�ԗ��p�����[�^�ݒ�
	parameter& ctrl_prm = ctrl_param->SetParameter(prm,h);	//����p�p�����[�^�ݒ�

	
	// ####################### DS ���[�h�� CARSIM ���[�h�̐؂芷�� ########
	//car.Init(CAR_DYN_DS);
	car.Init(CAR_DYN_CARSIM, ltrl.tf );
	//car.Init(CAR_DYN_COMS);

	// for look ahead
	ref_course = car.carsim.m_course;

	// generate noise
	std::normal_distribution<double> nd(0, 1);

	// different seeds every time
	std::random_device rd;
	std::default_random_engine re(rd());
	// use same seeds every time
	//std::default_random_engine rd(0);

	// normal distribution with sigma
	double input_noise = nd(re) * ssbParam.sigma;
	// take mving average
	moving_average noise_smoother(ssbParam.n_mov_avg);

	double pre_y_e=0, pre_theta_e=0, pre_vel=0, pre_u=0, 
	pre_rho=0, pre_rho_input=0, pre_rho_diff=0, pre_rho_n=0;
	double vel_ref = 0;

	//����
	int n_lap = 1;

	//��ԗʂ̎���
	double dim_x = 4;

	//�}�b�v�̍ŏ�����X�^�[�g�I
	int p; //�}�b�v�Q�Ɨp
	int p_zero;
	p_zero = 0;
	//////////////////////////////////////////////////////////////////


	stp.start();

	msr.reset();
	// Loop 
	while(LoopAlive)
	{
		// �e���ڂ̏������Ԍv���̂��߂̃J�E���^
		int t1 = 0;
		
		perform.reset();
		perform.start();
#define	MEASURE_BLOCK_TIME	{if(false){printf("Term%d:%f\n",t1++,perform.lap_in_ms());}}

		// input command tests
		string cmd = "";
		int key_ret = _kbhit();
		if(key_ret != 0)
		{
			// if any key is pushed, stop and wait command
			cin >> cmd;
			cout << "Command [" << cmd << "] received" << endl;
		}
		if(cmd=="q" || cmd == "quit" || cmd == "e" || cmd == "exit")
		{
			// loop exit
			LoopAlive = false;
		}



		LOG_DATA car_state;

		// �ԗ������Ԃ̎擾
		car.GetLogDataFromCar( car_state );

		// get u and vel from shared memory @okuda
		u = car_state.u;
		vel = car_state.speed;
		y_e = car_state.v;
		tire_angle = car_state.steer * 0.058917;
		p = get_p(u, map, p_zero, 1, ctrl_prm.course_length, p_zero); 

		//���񐔍X�V///////////////////////////////////////////
		if (u < 20 && pre_u > ctrl_prm.course_length-20)
		{
			n_lap += 1;
			p_zero = 0;
		}
		yaw = car_state.yaw + 2 * pi *(n_lap-1); //for Carsim
		// yaw = car_state.yaw; //for COMS
		pre_u = u;
		///////////////////////////////////////////////////////



		//Rho,Vref�̎擾///////////////////////////////////////
		// VectorXd Rho = VectorXd::Zero(ltrl.n+1);
		// VectorXd Delta_Rho = VectorXd::Zero(ltrl.n);
		double u_f = u; //k���_�ł�u���W
		double rhoo;


		//////������/////////////////////////////////////////////
		MEASURE_BLOCK_TIME

			
		//��Ԃ̎擾�C�v�Z
		p = get_p(u, map, p_zero, 0, ctrl_prm.course_length, p_zero);
		if (u > ctrl_prm.course_length) u -= ctrl_prm.course_length;
		theta_d = get_theta_d(u, map, p_zero);//���t�@�����X�̃��[�p���}�b�v�����������

		double lx = cos(yaw);
		double ly = sin(yaw);
		double yaw_m = atan2(ly, lx);
		theta_e = diff_rad(yaw_m, theta_d);

		if(theta_e > 3.00 / 2 || theta_e < -3.00 / 2)
		{
			printf("************ Not tracking! *********\r\n");
		}

		y_e_dot = (y_e - pre_y_e)/ ctrl_prm.delta;
	

		double diff_theta_e = diff_rad(theta_e, pre_theta_e);

		//	theta_e_dot =  (theta_e - pre_theta_e)/ ctrl_prm.delta;
		theta_e_dot =  (diff_theta_e)/ ctrl_prm.delta;

		// //////////////////////////////////////////////////////////
		// ���[���Z���^�[����̌덷�̌v�Z
		// �����̌v�Z�́C���Ƃ��Ƃ̃��[����������̌덷���ő���������
		double lane_u = 0;
		double lane_v = 0;
		double lane_w = 0;
		// �܂��̓��[�����uvw�ɕϊ�����
		lane_course->XYZ2UVW(car_state.x, car_state.y, 0, &lane_u, &lane_v, &lane_w);
		static int p_lane = 0; 
		static int p_lane_zero = 0;
		p_lane = get_p(lane_u, lane_map, p_lane_zero, 1, 
			ctrl_prm.course_length, p_lane_zero);
		if (lane_u > ctrl_prm.course_length) lane_u -= ctrl_prm.course_length;
		double theta_d_lane = get_theta_d(lane_u, lane_map, p_lane_zero);//���t�@�����X�̃��[�p���}�b�v�����������
		double theta_e_lane = diff_rad(car_state.yaw, theta_d_lane);
		
		// //////////////////////////////////////////////////////////
		// Look ahead control ; ��ǂ݂̎���
		// cout << "L_d" << ssbParam.l_lookahead << std::endl;
		double ahead_len = ssbParam.l_lookahead;
		double ahead_x = car_state.x + ahead_len * cos(car_state.yaw);
		double ahead_y = car_state.y + ahead_len * sin(car_state.yaw);
		double ahead_u = 0;
		double ahead_v = 0;
		double ahead_w = 0;
		ref_course->XYZ2UVW(ahead_x, ahead_y, 0, &ahead_u, &ahead_v, &ahead_w);

		//��Ԃ̎擾�C�v�Z
		int p_ahead = 0, p_ahead_zero = 0;
		p_ahead = get_p(ahead_u, map, p_ahead_zero, 1, 
			ctrl_prm.course_length, p_ahead_zero);
		if (ahead_u > ctrl_prm.course_length) ahead_u -= ctrl_prm.course_length;
		double ahead_theta_d = 
			get_theta_d(ahead_u, map, p_ahead_zero);//���t�@�����X�̃��[�p���}�b�v�����������

		double ahead_theta_e = 0;
		{
			double lx = cos(yaw);
			double ly = sin(yaw);
			double yaw_m = atan2(ly, lx);
			ahead_theta_e = diff_rad(yaw_m, ahead_theta_d);
		}

		////////////////////////////////////////////////////////////
		// generate noise with given sigma
		double generated_noise = nd(re)*ssbParam.sigma;
		// reset moving average size
		noise_smoother.size = ssbParam.n_mov_avg;
		// compute moving average
		double input_noise = noise_smoother.input(generated_noise);

		// current index in course is tested
		//cout << "p:" << p << ", p_zero, " << p_zero
		//	<< ", p_lane:" << p_lane << ", p_lane_zero, " << p_lane_zero
		//	<< ", p_ahead:" << p_ahead << ", p_ahead_zero. " << p_ahead_zero
		//	<< std::endl;


		// Parameter setting is moved to before the 'while'
		// �����ŋ��L����������㏑�����邩�ǂ����c
		shared_param.UpdateParamFromSharedMemory(ssbParam);

		// ��ԃx�N�g�����쐬
		double state[4];

		if (ssbParam.IsLookAheadEnabled == 0)
		{
			state[0] = y_e;
			state[1] = y_e_dot;
			state[2] = theta_e;
			state[3] = theta_e_dot;
		}
		else {
			state[0] = ahead_v;
			state[1] = y_e_dot;
			state[2] = ahead_theta_e;
			state[3] = theta_e_dot;
		}

		// 
		if(ssbParam.IsSteeringOutputIsDelta==0)
		{	// ���͂̓X�e�A�p�̔����n
			double u_delta_dot =
				+ state[0] * ssbParam.dd_K_y_e
				+ state[1] * ssbParam.dd_K_y_e_dot
				+ state[2] * ssbParam.dd_K_th_e
				+ state[3] * ssbParam.dd_K_th_e_dot;

			// �f�b�h�]�[������
			if(ssbParam.IsDeadZoneEnabled
				&& (dead_zone_minus[0] < y_e && y_e < dead_zone_plus[0])
				&& (dead_zone_minus[1] < theta_e && theta_e < dead_zone_plus[1])
				)
			{	// �f�b�h�]�[������
				u_delta_dot = 0;	// ��������Ȃ�(0�㏑���ŃL�����Z��)
			}

			// �ϕ����ăX�e�A�p�����߂�
			input_delta += u_delta_dot * ltrl.tf;

		}else
		{	// ���͂̓X�e�A�p���̂���
			input_delta =
				+ state[0] * ssbParam.K_y_e
				+ state[1] * ssbParam.K_y_e_dot
				+ state[2] * ssbParam.K_th_e
				+ state[3] * ssbParam.K_y_e_dot;

			// �f�b�h�]�[������
			if (ssbParam.IsDeadZoneEnabled
				&& (dead_zone_minus[0] < y_e && y_e < dead_zone_plus[0])
				&& (dead_zone_minus[1] < theta_e && theta_e < dead_zone_plus[1])
				)
			{	// �f�b�h�]�[������
				input_delta = 0;	// ��������Ȃ�(0�㏑���ŃL�����Z��)
			}

		}

		input_delta = input_delta + input_noise;
		input_delta = limit(input_delta, -1, 1);

		//static double U_delta_tmp = 0;

		// ���x�w�ߒl
		double U_velRef = 60.0 / 3.6;


		/////////////////////////////////////////////////////////
		//lgtd.u= hold3 + acc * delta;	//�ڕW���x
		//lgtd.acc = acc;
		//lgtd.U(0) = get_throttle(lgtd.u/3.6,acc/3.6); //�ڕW�X���b�g��


		//���X�e�b�v�̒l��ۑ�

		pre_y_e = y_e;
		pre_theta_e = theta_e;
		pre_vel = vel;


		//����
		car.SetOutput(ctrl_cnt, 1, input_delta, U_velRef);
		bool do_wait = car.Update(ltrl.tf);

		MEASURE_BLOCK_TIME

		// ----------- log data -------------
		// �ۑ��������f�[�^�𑝂₵�����ꍇ�C
		// optdata_st.h�̒���LOG_DATA�̃����o�𑝂₵�C
		// registermember���œo�^���Ă����C������LOG_DATA�Ƀf�[�^��������D
		// ���e����������LOG_DATA�\���̂��Clogger�ɒǉ�(push_back)����D
		// ���Ƃ�logger.print�ŕۑ������D
		LOG_DATA tmp = LOG_DATA();

		// �ԗ��̂͏ꍇ����������̂ňȉ��̒��ŏ������݂��܂���
		// CarDynamicsDelegator.cpp�̒��Ɏ��̂��A����
		car.GetLogDataFromCar( tmp );
		car.GetLogDataForCar( tmp );

		// ����ϐ��̕ۑ�����U��.
		// �����Ȃ�悤�Ȃ��p�̊֐���p�ӂ��Ă��傤�����D
		tmp.ctrl_u = input_delta;// ltrl.u;
		tmp.v_ref = vel_ref;
		tmp.v_command = vel_ref;
		tmp.n_lap = n_lap;
		//tmp.p_zero = p_zero;
		//tmp.a_ref = acc;
		//tmp.th_ref = lgtd.U(0);
#ifdef LATERAL
		tmp.yaw_modi = yaw;
		tmp.theta_e = theta_e;
		tmp.U_log0 = input_delta;
		//tmp.U_log0 =ltrl.U(0);
		//tmp.U_log1 =ltrl.U(1);
		//tmp.U_log2 =ltrl.U(2);
		//tmp.U_log3 =ltrl.U(3);
		//tmp.U_log4 =ltrl.U(4);
		tmp.theta_d = theta_d;
		tmp.y_e_dot = y_e_dot;
		tmp.theta_e_dot = theta_e_dot;

		double l1 = 1.0, l3 = 0.5, h1 = 0.3, h2 = 1.0;
		double L = (h1+h2)/h1*l1;
		tmp.rho = (1/L)*tmp.v+(1+l3/L)*tmp.theta_e;
		tmp.rho_modi = l1*tmp.rho;
#endif
		tmp.lane_u = lane_u;
		tmp.lane_y_e = lane_v;
		tmp.lane_theta_d = theta_d_lane;
		tmp.lane_theta_e = theta_e_lane;

		// ��ǂݏ��
		tmp.ahead_u = ahead_u;
		tmp.ahead_v = ahead_v;
		tmp.ahead_x = ahead_x;
		tmp.ahead_y = ahead_y;
		tmp.ahead_theta_d = ahead_theta_d;
		tmp.ahead_theta_e = ahead_theta_e;

		//tmp.rho = rhoo;
		//tmp.rho_modi = ltrl.Rho(0);

		// ����U�����f�[�^�\�����C�X�g���[�W�Ɋi�[
		logger.push_back( tmp );

		MEASURE_BLOCK_TIME

		// cout << msr.lap();


		// ---------- debug print here -------------
		static int debug_print_skip_count = 0;
		const int decimate_number = 20;
		if( debug_print_skip_count++ % decimate_number == 0)
		{

						// computation for debug;
			double interval = keisoku.lap()/decimate_number;


			//Time
			printf("Time=%f, ",
				//shr_dsctrl_info.p_shared_data->nCtrlCnt);  // @ okuda
				car_state.t);
			printf("Average Itvl�F%.5f�b\n",interval);

			//Car state
			printf("Car state (x,y,  u,v)= (%.2f,%.2f,  %.2f,%.2f)\n", 
				car_state.x, car_state.y, car_state.u, car_state.v );
			printf("Control state (y_e,  y_e_dot; theta_e, theta_e_dot)= (%.2f, %.2f; %.2f, %.2f)\n",
				y_e, y_e_dot, theta_e, theta_e_dot);
			printf("Car state (yaw,speed,steer(rad))= (%.2f,%.2f,%.2f)\n", 
				car_state.yaw, car_state.speed,car_state.steer);
			cout << "laps:" << n_lap << endl << endl;

			

			// --------------------------------------------
		}

		// ---------- debug print here -------------
		static int debug_unity_skip_count = 0;
		const int decimate_unity = 1;
		if (debug_unity_skip_count++ % decimate_unity == 0)
		{
#ifdef ENABLE_UNITY
			DataTranslationToUnity(ctrl_cnt, stp.split(), tmp, udp_data);
#endif
		}

		if(do_wait)
		{
			//�������100ms
			while( stp.split()  < ltrl.tf )
			{ 
				Sleep(0);	
			}
		}

		stp.reset();


		ctrl_cnt++;


		// if (n_lap == 2 && u > 100) break;
		if (n_lap == 11) break; //n_lap���ɓ������u�ԂɏI��
	}

	// �ۑ������D�t�@�C���N���[�Y�̂��߁C���̒��J�b�R�͏����Ȃ���
	{
		time_t now = time(NULL);
		tm* now_local = localtime(&now);

		char str[256];
		char str2[256];
		strftime(str, sizeof(str)-1, "%Y%m%d_%H%M%S", now_local);
		strftime(str2, sizeof(str)-1, "%Y%m%d", now_local);

		string date_str(str);
		string date_str2(str2);
		//string filename = string("mpcdata")
		//	+ "_" + date_str 
		//	+ "_n" + boost::lexical_cast<string>(ltrl.n)
		//	+ "_r" + boost::lexical_cast<string>(ltrl.r)
		//	+ "_q" + boost::lexical_cast<string>(ltrl.Qd(0))
		//	+ "_in" + (car.mode==CAR_DYN_CARSIM ? "SIM" : "DS")
		//	+ ".csv";
		//ofstream data_out_stream(filename); 
		

#ifdef DATA_TARGET_DIRECTRY
		std::string dire_name = std::string(DATA_TARGET_DIRECTRY) + str2;
		_mkdir( dire_name.c_str() );
#else
		_mkdir(str2);
#endif

		string filename = dire_name
			+ "/mpcdata"
			+ "_" + date_str + ".csv";
		ofstream data_out_stream(filename + ".csv"); 

		// ���O�f�[�^���X�g���[���ɏ����o���C�N���A
		logger.print(data_out_stream);
		logger.clear();

		string prm_filename = dire_name
			+ "/mpcparam"
			+ "_" + date_str + ".txt";
		
		write_param(prm, prm_filename);
	}

	
	// ----------------------------------------
	// Close DataViewProcess
	HWND target;
	target = FindWindowA(NULL, "COMSMPCDataView");
	SendMessage( target, WM_SYSCOMMAND, SC_CLOSE, 0);

	// ---------------------------------------

	return 0;




}








void DataTranslationToUnity(
	int ctrl_cnt, double time_elap, 
	LOG_DATA car_state, UDPSender &udp_data) {
	PacketToDSViz packet;
	CAR_UNIT_INFO car_info;

	car_info.x = car_state.x;
	car_info.y = car_state.y;
	car_info.z = 0;
	car_info.yaw = car_state.yaw;
	car_info.id = 0;
	car_info.mesh_id = 0;
	car_info.pitch = 0;
	car_info.roll = 0;
	car_info.steering = car_state.steer;
	car_info.accelpedal = car_state.throttle;
	car_info.brakepedal = car_state.brake;
	car_info.eng_rpm = 0;
	car_info.speed = car_state.speed;

	CAR_UNIT_INFO ahead_pt;

	ahead_pt.x = car_state.ahead_x;
	ahead_pt.y = car_state.ahead_y;
	ahead_pt.z = 0+1;
	ahead_pt.yaw = car_state.yaw;
	ahead_pt.id = 1;
	ahead_pt.mesh_id = 1;
	ahead_pt.pitch = 0;
	ahead_pt.roll = 0;
	ahead_pt.steering = car_state.steer;
	ahead_pt.accelpedal = car_state.throttle;
	ahead_pt.brakepedal = car_state.brake;
	ahead_pt.eng_rpm = 0;
	ahead_pt.speed = car_state.speed;

	std::memcpy(packet.header_str, "NUDSDATA", 8);

	int NCar = 1;
	int NCamera = 1;

	packet.time_info.frame_cnt = ctrl_cnt;
	packet.time_info.deltaT = 0.01;
	packet.time_info.simulated_time = ctrl_cnt * 0.01;
	packet.time_info.measured_time = (float)time_elap;

	packet.camera_info[0].camera_attached_car_id = 0;
	packet.camera_info[0].camera_type = 0;

	packet.num_of_cars = 2;

	//for (int i = 0; i < packet.num_of_cars; i++)
	{
		packet.car_states[0] = car_info;
		packet.car_states[1] = ahead_pt;
		//packet.car_states[0].id = i;
		//packet.car_states[0].mesh_id = carManager->cars[i]->m_carTypeID;
		//packet.car_states[0].x = carManager->cars[i]->m_x;
		//packet.car_states[0].y = carManager->cars[i]->m_y;
		//packet.car_states[0].z = carManager->cars[i]->m_z;
		//packet.car_states[0].yaw = carManager->cars[i]->m_yaw;
		//packet.car_states[0].pitch = carManager->cars[i]->m_pitch;
		//packet.car_states[0].roll = carManager->cars[i]->m_roll;
		//packet.car_states[0].anim_flag = carManager->cars[i]->m_flg;
		//packet.car_states[0].speed = carManager->cars[i]->m_speed;
		//packet.car_states[0].winkers = (carManager->cars[i]->m_winker_R == 1 ? 1 : 0) + (carManager->cars[i]->m_winker_L == 1 ? 2 : 0);

	}
	// own car is invisible 
	//packet.car_states[0].id = 0;
	//packet.car_states[0].eng_rpm = carManager->myCar->m_rpm;

	// static objects;
	packet.static_objects.count = 0;
	//for (int i = 0; i < objManager->trafficlights.size(); i++) {
	//	packet.static_objects.id[i] = objManager->trafficlights[i].parameter.id;
	//	packet.static_objects.state[i] = objManager->trafficlights[i].state;
	//}
	   
	if (udp_data.isOpen() && ctrl_cnt % 3 == 0)
		udp_data.SendBytes(&packet, sizeof(PacketToDSViz));

	
}

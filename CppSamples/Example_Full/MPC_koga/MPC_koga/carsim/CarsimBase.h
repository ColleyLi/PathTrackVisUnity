/*!
 * CarsimBase class 
 * 
 * Copyright (c) 2013 by Hiroyuki Okuda, Suzlab. Nagoya Univ.
 */

///
/// CarsimBase�̎g���� How to use CarsimBase
///
/// 1. CarsimBase���p�����܂��B
/// class YourClass : public CarsimBase{}
///
/// 2. YourClass �̃R���X�g���N�^�ł�
/// CarsimBase�̃R���X�g���N�^��simfile����n���܂��B
/// // constructor of your class
/// YourClass(string simfile_name) : CarsimBase(simfile_name) {
///
/// 3. Callback functions�����܂��B
/// �ʏ�YourClass��static�Ƃ��Ă����ƕ֗��ł��B
/// 
/// static void calc_callback(vs_real t, vs_ext_loc loc){...}
/// static vs_bool scan_callback(char *keyword, char *buffer){...}
/// static void setdef_callback(void){...}
/// static void echo_callback(vs_ext_loc loc){...}
///
/// (Tips) - �����̊֐��Q�����ł́C�����o�֐��R�[���̌`�ɂ���ƕ֗��ł��D
/// example:
/// YourClass...
/// { ... 
///		static YourClass* instance;
///		void calc(vs_real, vs_ext_loc);
///	... }
/// static void calc_callback(vs_real t, vs_ext_loc loc){ 
///    YourClass::instance->calc(t, loc);
/// }
///
/// 4. Callback ��register���܂��D
///   A. �ʂɓo�^����
///    	vs_install_calc_function (&calc_callback);
///		vs_install_echo_function (&echo_callback);
///		vs_install_scan_function (&scan_callback);
///		vs_install_setdef_function (&setdef_callback); 
///
///   B. CarsimBase�̃w���p�֐����g��
/// 	CarsimBase::register_callbacks( 
///     	&YourClass::calc_callback, 
///     	&YourClass::echo_callback, 
///     	&YourClass::setdef_callback, 
///     	&YourClass::scan_callback
///     );
/// 
/// 5. Callback�̒��C���邢�͊O�ŁC�K�؂ȏ����������܂��D
/// 
/// * please reffer to sample program : CarsimNenpi.h and CarsimNenpi.cpp


#pragma once

#include <Windows.h>
#include <string>

#include "vs_deftypes.h" // VS types and definitions

class CarsimBase
{
public:
	CarsimBase(std::string fn);
	virtual ~CarsimBase(void);

	// open dll and load function pointers
	void open_dll(std::string simfile_name);

	// free dlls
	void free_dll();

	// run simulation all W/O stop
	virtual int run_all();

	virtual int run_init_step();
	virtual int run_integrate();
	virtual int run_terminate();

	virtual bool is_continuing();

	// typedef of function callback 
	typedef void (*calc_func_type) (vs_real time, vs_ext_loc loc) ;
	typedef void (*echo_func_type) (vs_ext_loc where);
	typedef void (*setdef_func_type) (void);
	typedef vs_bool (*scan_func_type) (char *, char *);
	typedef void (*free_func_type) (void);

	// register_static_callbacks
	virtual void register_callbacks(
		calc_func_type _calc, 
		echo_func_type _echo, 
		setdef_func_type _setdef, 
		scan_func_type _scan_
		);

protected:
	HMODULE vsDLL; // DLL with VS API
public:
	std::string dll_path;
	std::string simfile;

	// simulation time in carsim
	double carsim_t;

#include "vs_api.hpp"  // VS API functions
	// utility function from vs_get_api.c
	int vss_dll_problem (char *dll, char *func, int code);
	
};


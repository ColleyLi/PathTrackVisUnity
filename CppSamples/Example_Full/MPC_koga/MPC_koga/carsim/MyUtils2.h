
#pragma once
#define INDEBUG(x) x
//#define INDEBUG(x)

namespace myutil2
{
// -----------------------
// Our utilities for DS
// �V���v���Ȍv�Z�̂��߂̊ȒP�ȃ��[�e�B���e�B�֐���錾�B
// �K�v������Ύ��R�ɒǉ����Ă悢�ł����A
// �V���v���Ȃ��̂����ɂ��܂��傤�B

double Deg2Rad(double deg);
double Rad2Deg(double rad);


// safe release macro & template
//#define SAFE_RELEASE(x) {if(x!=NULL){(x)->Release();x=NULL;}}

template <class T> 
void SafeRelease(T x)
{
	if(x!=NULL){
		(x)->Release();
		x=NULL;
	}
};

template <class T> 
void SafeDelete(T x)
{
	if(x!=NULL){
		delete (x);
		x=NULL;
	}
};

template <class T>
void SafeDeleteArray(T x)
{
    if(x!=NULL){
        delete[] (x);
        x=NULL;
    }
};

template<class T>
T interp_1(T st_val, T ed_val, T t, T st_t, T ed_t)
{
	if(t <= st_t)return st_val;
	if(t >= ed_t)return ed_val;

	if( ed_t - st_t == 0)
	{
		return ((st_val + ed_val) / 2.0);
	}
	
	T p;
	p = (t - st_t) / (ed_t - st_t);
	return ((ed_val - st_val)*p + st_val);
};





}

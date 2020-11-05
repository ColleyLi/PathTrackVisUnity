#pragma once

#include <Eigen/Core>
#include "optdata_st.h"

// lateral control MPC
class LongitudinalControl
{
public:

	// ???
	//double v;
	//double pi;

	// �œK���p�����[�^�ƕϐ��̎���
	// main���ł͂���̎Q�Ƃ��g����
	opt_slt lgtd;

	LongitudinalControl(){}

	// Create optimization parameters
	opt_slt& CreateParameter(RTCLib::CSVLoader &prm, int n)
	{
		
		lgtd.u=0;
		
		lgtd.Ad.resize(1,1);
		lgtd.Bd.resize(1,1);
		lgtd.Qd.resize(1,1);
		lgtd.Sf.resize(1,1);
		
		lgtd.n = prm[n][prm.GetColOf("N")];     //�X�e�b�v��
		lgtd.r = prm[n][prm.GetColOf("R_lon")];      //���͂̏d��
		lgtd.tf = prm[n][prm.GetColOf("period")];
		lgtd.Qd << prm[n][prm.GetColOf("Q_lon")];
		lgtd.Sf << prm[n][prm.GetColOf("Sf_lon")]; 
		
		lgtd.Ad <<	1;    //A�s��
		lgtd.Bd <<	lgtd.tf;

		lgtd.A.resize(lgtd.n+1, 1);							  //A�s��̊g��n
		lgtd.B.resize(lgtd.n+1, lgtd.n);
		lgtd.Q.resize(lgtd.n+1, lgtd.n+1);
		lgtd.R = lgtd.r * MatrixXd::Identity(lgtd.n,lgtd.n);      //R�s��̊g��n
		lgtd.Vref.resize(lgtd.n+1);
		lgtd.Xp.resize(lgtd.n+1);

		lgtd.X.resize(1);
	
		for (int m = 0; m < lgtd.n; m++)
		{	
			for (int l = 0; l < lgtd.n+1; l++)
			{	
				if (l > m)
				{
					lgtd.B.block(l,m,1,1) = lgtd.Bd;
				}else
				{
					lgtd.B(l,m) = 0;
				}
			}
		}

		
		for(int l=0; l < lgtd.n+1; l++)
		{
			for (int m = 0; m < lgtd.n+1; m++){
				if (l == m && m != lgtd.n){
					lgtd.Q.block(l,m,1,1) = lgtd.Qd;
				}else if ( l == lgtd.n && m == lgtd.n){
					lgtd.Q.block(l,m,1,1) = lgtd.Sf;
				}else{
					lgtd.Q(l,m) = 0;
				}
			}
		}

		for (int i=0; i< lgtd.n+1 ;i ++){
			lgtd.A(i,0) = lgtd.Ad(0,0);
		}

		MatrixXd tmp_mat1 = (lgtd.B.transpose() * lgtd.Q * lgtd.B + lgtd.R).inverse();	
		lgtd.K = -( tmp_mat1 * lgtd.B.transpose() * lgtd.Q ); 

		return lgtd;
	}





};

#ifndef __COURSE_H__
#define __COURSE_H__

#pragma once

#include <vector>
#include <string>
//#include "CarManager.h"

namespace CarManagement{

	using namespace std;

	//class Car;
	//class CarManager;

	// �R�[�X�������N���X
	class Course
	{
	public:
		// Vec3�\���̂������`
		struct CheckPoint
		{
			double pos[3]; // �O���[�o�����W�ɑ΂���ʒu
			double u; // ���̓_�܂ł̃R�[�X�ɉ���������
			double tangent[3]; // �R�[�X�̐ڐ�����(���ۂ͎��̓_�̕�������v�Z��������)
			double normal[3]; // �R�[�X�̕��ʓ��ŁCtangent�ɐ�������(tangent��upvector���v�Z��������)
			double upvector[3]; // �R�[�X�̖ʂɐ����ȕ���(�f�[�^�Ƃ��ė^����)

			// �`�F�b�N�|�C���g�̍��W�n����O���[�o���ɑ΂���p��roll,pitch,yaw���v�Z
			// ���܂̂Ƃ��덇���Ă邩�ǂ����s��
			void getRollPitchYaw(double *roll, double *pitch, double *yaw) const;
		};

		// �ǂݍ��񂾃R�[�X�t�@�C��
		std::string coursefile;

		// �R�[�X��
		double length;

		// �`�F�b�N�|�C���g�̐�
		int num_of_checkpoints;
		// �`�F�b�N�|�C���g�̔z��
		CheckPoint *checkPoint;

		// Course����CarManager���Q�Ƃ��邽�߂̃|�C���^
		//static CarManager *cm;

		// �R���X�g���N�^
		Course( std::string file);

		// �f�X�g���N�^
		~Course();

		// ���񐔂�Ԃ��֐�
		int getCurrentLap(double u) const;
		// ���񐔂��J�b�g���C���̎���ɂ����鑖�s�ʒu��Ԃ��֐�
		double getCurrentLapPosition(double u) const;

		// ��ԋ߂��`�F�b�N�|�C���g�̃C���f�b�N�X��Ԃ��֐�
		int getNearestCheckPointIndex(double x, double y, double z) const;
		//int getNearestCheckPointIndex(const Car &car) const;
		// 1�X�e�b�v�O�̈�ԋ߂������C���f�b�N�X��n���Ă������猟�����邱�Ƃō�������}��o�[�W����
		int getNearestCheckPointIndex(double x, double y, double z, int prev) const;
		//int getNearestCheckPointIndex(const Car &car, int prev) const;

		// ��Ԗڂɋ߂��`�F�b�N�|�C���g�̃C���f�b�N�X��Ԃ��֐�
		// �����Ɍ����ƁC�n���ꂽ�`�F�b�N�|�C���g�̑O��̂����̋߂�����Ԃ��֐�
		int getSecondNearestCheckPointIndex(double x, double y, double z) const;
		//int getSecondNearestCheckPointIndex(const Car &car) const;
		// ��Ԗڂɋ߂��`�F�b�N�|�C���g�̃C���f�b�N�X��Ԃ��֐�(��ԋ߂��`�F�b�N�|�C���g��n�����Ƃō�����)
		// �����Ɍ����ƁC�n���ꂽ�`�F�b�N�|�C���g�̑O��̂����̋߂�����Ԃ��֐�
		int getSecondNearestCheckPointIndex(double x, double y, double z, int nearest) const;
		//int getSecondNearestCheckPointIndex(const Car &car, int nearest) const;

		// �����Ă����ԑ傫�ȃ`�F�b�N�|�C���g�̃C���f�b�N�X��Ԃ��֐�
		int getPrevCheckPointIndex(double u, double v, double w) const;

		// �ʒu���W�����߂�
		double getX(double u, double v, double w) const;
		//double getX(const Car &car) const;
		double getY(double u, double v, double w) const;
		//double getY(const Car &car) const;
		double getZ(double u, double v, double w) const;
		//double getZ(const Car &car) const;

		// �R�[�X�ɉ��������W�����߂�
		// ������u�Ɋւ��Ă͎��񐔂܂Ŋ܂߂������͂킩��Ȃ�
		double getU(double x, double y, double z) const;
		//double getU(const Car &car) const;
		double getV(double x, double y, double z) const;
		//double getV(const Car &car) const;
		double getW(double x, double y, double z) const;
		//double getW(const Car &car) const;

		// �ʒu���W����R�[�X�ɉ��������W�����߂�֐�
		// �������S�Ă̓_�ƌ��݈ʒu���r����Ǝ��Ԃ������肷����̂ŁC1�X�e�b�v�O��u���W��n�����Ƃō�����
		void XYZ2UVW(double x, double y, double z, double *u, double *v, double *w) const;

		// �R�[�X�ɉ��������W����ʒu���W�����߂�֐�
		void UVW2XYZ(double u, double v, double w, double *x, double *y, double *z) const;
	};


	// �֗��n�֐���

	// ��拗��
	double dist2(double x1, double y1, double z1, double x2=0, double y2=0, double z2=0);

	// ����(dist2���d��)
	double dist(double x1, double y1, double z1, double x2=0, double y2=0, double z2=0);

	// ���K��
	void normalize(double *x, double *y, double *z);

	// �O��(�͈̓`�F�b�N�H�����ꂨ�������́H)
	void outer_product(double a[], double b[], double res[]);

	// �ȈՊO��
	double crossprod2d(const double &ax, const double &ay, const double &bx, const double &by);

}


#endif // __COURSE_H__
// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"

#include "UDPTrans.h"
#include <Windows.h>

#include <stdio.h>
#include <tchar.h>
#include <time.h>

#include <conio.h>
#include <string>
//#include <iostream>
//#include <fstream>

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B
// data structure for shared memory
#include "../../Common/Shrmem_struct.h"
#include "../../Common/AssistShare.h"

// shared memory manager
#include "../../Common/RTCLib/cpp/header/SharedMemoryManager.h"
#include "../../Common/RTCLib/cpp/header/CSVLoader.h"
#include "../../Common/RTCLib/cpp/header/StopWatch.h"
#include "../../Common/RTCLib/cpp/header/DataStorage.h"
#include "setting.h"

#include "ShrMemManager.h"


#include <math.h>
#include <Eigen/Dense>
#include <stdlib.h>

// for carsim
#include "carsim/CarsimCar2014.h"
#include "setting.h"

#pragma once


#define VEL_CONTROL	//���x�Ő���
//#define PEDAL_CONTROL	//�y�_���Ő���

#define LONGITUDINAL	//�c��������
#define LATERAL			//����������

//#define DELAY		//�x�ꂠ��
//#define RHO		//�ȗ��̃t�B���^

#define BICYCLE	//bicycle model�inot define ���@���~���񃂃f���j

// course info to compute UV coodinate
//#define DRIVING_COURSE ("fujioka_course.txt")

#define DRIVING_COURSE ("ref_path/maruyama_AvgPath_Okuda2.txt")
//#define DRIVING_LANE_CENTER ("ref_path/maruyamaR100_forExp.txt")
#define DRIVING_LANE_CENTER ("ref_path/maruyama_AvgPath_Okuda2.txt")


// course info for Koga-program
//#define DRIVING_MAP ("fujioka_map.csv")
#define DRIVING_MAP ("ref_path/maru_mapR100_AvgPath_Okuda2.csv")
//#define DRIVING_LANE_MAP  ("ref_path/maru_mapR100_LaneCenter_Okuda2.csv")
#define DRIVING_LANE_MAP ("ref_path/maru_mapR100_AvgPath_Okuda2.csv")

#define LENGTH 0.28

//#define ENABLE_RESULT_VISUALIZER
#define ENABLE_UNITY

#define DATA_TARGET_DIRECTRY ("data/")
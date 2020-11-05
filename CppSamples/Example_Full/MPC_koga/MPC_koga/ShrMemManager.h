#pragma once

// ���L�������}�l�[�W���I  by Okuda
//  
//  ���z�֐��������Ȃ��^�Ȃ�n�j�I���Ԃ�
//  ShrMemManager<Hogehoge> shr("SOME_UNIQUE_STRING") ��Hogehoge�^�p���L�������̈�쐬
//  shr.Write( hoge_instance ); ��hoge_instance����������
//  shr.Read( hoge_instance ); ��hoge_instance�Ƀf�[�^��ǂݍ���
//  shr.LockMem(); �Ŕ��Α��v���Z�X�Ƃ̔r������̂��߂̃~���[�e�b�N�X���擾
//  she.UnlockMem(); �Ń~���[�e�b�N�X�����

#include <windows.h>


template <class T>
class ShrMemManager
{
	friend class ScopedLock;
private:
	HANDLE hMap; /* �t�@�C���}�b�s���O�I�u�W�F�N�g�̃n���h�� */
	//SHARED_DATA	CurrentData, *pData; /* ���L����f�[�^ */
	BOOL bAlreadyExists; /* ���Ƀt�@�C���}�b�s���O�I�u�W�F�N�g���쐬����Ă��邩�ǂ�����\���^�U�l */
	//char msg[1000];
	HANDLE hMutex; /* ���L�f�[�^�ւ̔r���A�N�Z�X�p�~���[�e�b�N�X�I�u�W�F�N�g�̃n���h�� */

public:
	T* pData;
	
	void LockMem();
	void UnlockMem();
public:
	ShrMemManager();
	ShrMemManager(LPCTSTR pSharePageName, LPCTSTR pMutexName=NULL);
	LRESULT Open(LPCTSTR pSharePageName, LPCTSTR pMutexName=NULL);

	~ShrMemManager(void);

	LRESULT Read(T &out);
	LRESULT Write(const T &in);

};

template<class T>
ShrMemManager<T>::ShrMemManager() : hMap(NULL), hMutex(NULL), pData(NULL)
{

}

template<class T>
ShrMemManager<T>::ShrMemManager(LPCTSTR pSharePageName, LPCTSTR pMutexPostFix) : hMap(NULL), hMutex(NULL), pData(NULL)
{
	Open(pSharePageName, pMutexPostFix);
}

template<class T>
LRESULT ShrMemManager<T>::Open(LPCTSTR pSharePageName, LPCTSTR pMutexName)
{
	std::wstring str(pSharePageName);
	if(pMutexName != NULL)
	{
		str = pMutexName;
	}else{
		str = str + L"_MUTEX";
	}
	
	/* �r������p�~���[�e�b�N�X�I�u�W�F�N�g�쐬 */
	hMutex = CreateMutex(NULL, FALSE, str.c_str());

	/* �t�@�C���}�b�s���O�I�u�W�F�N�g�̍쐬 */
	hMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE | SEC_COMMIT,
		0, sizeof(T), 
		pSharePageName);
	if(hMap==NULL)
	{
		str = str + L"���L�������̃I�[�v���Ɏ��s���܂����I�I ���L�y�[�W��:";
		MessageBox(NULL, str.c_str(), L"Error!!", MB_OK);
		return S_FALSE;
	}

	/* ���ɍ쐬����Ă���? */
	bAlreadyExists = (GetLastError() == ERROR_ALREADY_EXISTS);

	/* �r���[�̍쐬 */
	pData = (T *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(T));
	return S_OK;
}

template <class T>
ShrMemManager<T>::~ShrMemManager(void)
{
	/* �㏈�� */
	CloseHandle(hMutex);
	UnmapViewOfFile(pData);
	CloseHandle(hMap);
}


template <class T>
LRESULT ShrMemManager<T>::Read(T &out)
{
	if(hMap==NULL)
	{
		OutputDebugString(L"���L���������J���Ă��܂��񁗓ǂݍ���");
		return S_FALSE;
	}
	/* ���L�������ɑO�y�[�W�ǂ� */
	LockMem();//WaitForSingleObject(hMutex, INFINITE);
	out = *pData;
	UnlockMem();//ReleaseMutex(hMutex);

	return S_OK;
};

template <class T>
LRESULT ShrMemManager<T>::Write(const T &in)
{
	if(hMap==NULL)
	{
		OutputDebugString(L"���L���������J���Ă��܂��񁗏�������");
		return S_FALSE;
	}
	/* ���L�������ɑO�y�[�W���� */

	LockMem();//WaitForSingleObject(hMutex, INFINITE);
	*pData = in;
	UnlockMem();//ReleaseMutex(hMutex);

	return S_OK;
};

template <class T>
void ShrMemManager<T>::LockMem()
{
	WaitForSingleObject(hMutex, INFINITE);
}

template <class T>
void ShrMemManager<T>::UnlockMem()
{
	ReleaseMutex(hMutex);
}

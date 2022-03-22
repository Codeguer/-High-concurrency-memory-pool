#pragma once
#include"Common.h"

#ifdef _WIN32
	#include<Windows.h>
#else
	#include<unistd.h>
#endif

inline static void*SyetemAlloc(size_t kpage) {
#ifdef _WIN32
	void*ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//MEM_COMMITΪָ����ַ�ռ��ύ�����ڴ�
	//MEM_RESERVE��ֹ�����ڴ���亯��malloc��LocalAlloc����ʹ���ѱ������ڴ淶Χ,ֱ���������ͷ�
	//���򲻿�ִ�д��룬Ӧ�ó�����Զ�д������
	//������óɹ�,���ط�����׵�ַ,����ʧ��, ����nullptr
#else
	void*ptr = sbrk(kpage << 13);
#endif
	if (ptr == nullptr) {
		throw std::bad_alloc();
	}
	return ptr;
}

template<class T>
class ObjectPool{
private:
	char* _memory = nullptr;//�������ڴ�
	size_t remainBytes = 0;//��¼_memory����Ĵ���ڴ������
	void* _freeList = nullptr;//�����ͷŻ������ڴ�

public:
	T*New(){
		T*obj = nullptr;//Ҫ���ص��ڴ�����
		if(_freeList){//���ȴ�_freeList��ȡ�ڴ�
			obj = (T*)_freeList;
			_freeList = NextObj(_freeList);
		}
		else {
			size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			if (remainBytes < objSize) {//�����ʣ�ڴ��С����һ��objSize��ô��Ҫ����һ����
				remainBytes = 128 * 1024;
				//��ҳΪ��λ��ȡ�ڴ�
				_memory =(char*) SyetemAlloc(remainBytes >> 13);//һҳΪ8kb
			}
			obj = (T*)_memory;
			remainBytes -= objSize;
			_memory += objSize;
		}

		new(obj)T;//��λnew���ʽ����ʾ����T��Ĺ��캯�����г�ʼ��

		return obj;
	}

	void Delete(T *obj) {//���ͷŵ�obj����_freeList
		obj->~T();//��ʾ����T��������������obj���������
		//ͷ��
		NextObj((void*)obj) = _freeList;//��ͱ���Ҫ��֤obj��С����Ҫsizeof(void*)�Ĵ�С
		_freeList = obj;
	}
};

struct TreeNode//�Զ�������
{
	int _val;
	TreeNode* _left;
	TreeNode* _right;

	TreeNode()
		:_val(0)
		, _left(nullptr)
		, _right(nullptr)
	{}
};

void TestObjectPool()
{
	// �����ͷŵ��ִ�
	const size_t Rounds = 5;

	// ÿ�������ͷŶ��ٴ�
	const size_t N = 100000;

	std::vector<TreeNode*> v1;
	v1.reserve(N);

	size_t begin1 = clock();
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v1.push_back(new TreeNode);
		}
		for (int i = 0; i < N; ++i)
		{
			delete v1[i];
		}
		v1.clear();
	}

	size_t end1 = clock();

	std::vector<TreeNode*> v2;
	v2.reserve(N);

	ObjectPool<TreeNode> TNPool;
	size_t begin2 = clock();
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v2.push_back(TNPool.New());
		}
		for (int i = 0; i < N; ++i)
		{
			TNPool.Delete(v2[i]);
		}
		v2.clear();
	}
	size_t end2 = clock();

	std::cout << "new cost time:" << end1 - begin1 << std::endl;
	std::cout << "object pool cost time:" << end2 - begin2 << std::endl;
}
#include"ThreadCache.h"
#include"CentralCache.h"
void* ThreadCache::Allocate(size_t size) {
	assert(size <= MAX_BYTES);
	size_t alignSize = SizeClass::RoundUp(size);//�����Ҫ��ȡ�ĵ���obj�Ĵ�С
	size_t index = SizeClass::Index(size);

	if (!_freeLists[index].Empty())
	{
		return _freeLists[index].Pop();
	}
	else//��central cacheҪ
	{
		return FetchFromCentralCache(index, alignSize);
	}
}

void ThreadCache::Deallocate(void*ptr, size_t size) {
	assert(ptr);
	assert(size <= MAX_BYTES);

	// �Ҷ�ӳ�����������Ͱ������������
	size_t index = SizeClass::Index(size);
	_freeLists[index].Push(ptr);
}

void* ThreadCache::FetchFromCentralCache(size_t index, size_t size) {
	// ��ȡһ����������ʹ����������ʽ
	//batch��������˼
	//ȡ����ֵ���ٵ��Ǹ���Ŀ�ģ�����������Ƚ�С��ôһ�ξͶ�����ڴ�飻
				//				����������Ƚϴ���ôһ�ξ��ٸ����ڴ�飻
	//��������Ŀ�ľ����ڷ�ֹ�ռ��˷�����ٷ���central cache����֮�����ƽ��
	//_freeLists[i].MaxSize()���һ��ʼ����Ϊ��1����ô��һ�η���1
	//���sizeΪ8�ֽڣ���Ϊ��_freeLists[i].MaxSize()�Ŀ��Ʋ���һ��ʼ�ͷ���512��
	//�����������ӵ�һ�η���1�����ڶ��η���2���������η���3��...��_freeLists[i].MaxSize()���ϼ�1��
	//��_freeLists[i].MaxSize()>512�Ͳ�����������
	//���sizeΪ8kb,��ô��_freeLists[i].MaxSize()>(64kb/8kb)��ʧȥ������
	size_t batchNum = std::min(_freeLists[index].MaxSize(), SizeClass::NumMoveSize(size));
	//����Ҫǿ������batchNum������Ϊ�����Ч�ʣ�ʵ�ʷ���Ķ����������ܲ���batchNum(����һ��span��û����ô�������)

	// ȥ���Ļ����ȡbatch_num������
	void* start = nullptr;
	void* end = nullptr;
	size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start, end, batchNum, size);
	//ʵ����ֻ��Ҫһ������batchNum������Ϊ�����Ч�ʣ�����batchNumΪ10��������Span��û��10�����еĿռ�
	//ֻ��4���ˣ���Ҳû�й�ϵ��ͨ������ֵ�Ϳ���֪��ʵ�ʷ����˶��ٸ��ռ�

	assert(actualNum > 0);

	if (_freeLists[index].MaxSize() == batchNum) {//����_freeLists[i].MaxSize()<=SizeClass::NumMoveSize(size)ʱ
		//SizeClass::NumMoveSize(size)����Ϊ[2,512]�����_freeLists[i].MaxSize()�������������ô
		//_freeLists[i].MaxSize()��ʧȥ�����ã���˵�_freeLists[i].MaxSize() != batchNumʱ
		//�Ͳ����ٹ���_freeLists[i].MaxSize()��
		_freeLists[index].MaxSize()+=1;
	}
	
	if (actualNum == 1){
		assert(start == end);
		return start;
	}
	else{
		// >1������һ����ʣ�¹ҵ���������
		_freeLists[index].PushRange(NextObj(start), end);
		return start;
	}	
	
}
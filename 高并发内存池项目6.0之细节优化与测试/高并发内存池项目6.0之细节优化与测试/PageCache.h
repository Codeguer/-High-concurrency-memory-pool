#pragma once

#include "Common.h"
#include"ObjectPool.h"
class PageCache{
public:
	static PageCache* GetInstance(){
		return &_sInst;
	}

	// ��ȡ�Ӷ���span��ӳ��
	Span* MapObjectToSpan(void* obj);

	// �ͷſ���span�ص�Pagecache�����ϲ����ڵ�span
	void ReleaseSpanToPageCache(Span* span);

	// ��ȡһ��Kҳ��span
	Span* NewSpan(size_t k);

	std::mutex _pageMtx;//ȫ��
private:
	SpanList _spanLists[NPAGES];
	ObjectPool<Span> _spanPool;//ʹ�ö����ڴ�ش���new

	std::unordered_map<PAGE_ID, Span*> _idSpanMap;//�������Ҫ��ҳ����span��ӳ�䣬��thread cache��һ���ֶ���
	//���ո�central cache����һȺ�������ͨ��ӳ���ϵ�ҵ���Ӧ��span
	//�˴����Ż���tcmallocʹ�õ��ǻ����� ��Ч�ʸ���

	PageCache()
	{}
	PageCache(const PageCache&) = delete;

	static PageCache _sInst;
};


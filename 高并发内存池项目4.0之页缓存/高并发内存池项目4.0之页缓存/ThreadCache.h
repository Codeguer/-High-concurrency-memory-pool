#include"Common.h"

//��Ҫ��STL�����ݽṹ����Ϊ�������ǵ��ڴ�ؾ�����������STL��
//�Լ�malloc����Ϊ���ǵ��ڴ�ؽ�������Ҫ���malloc��

//_freeLists��һ��ӳ����������Ĺ�ϣͰ����_freeLists[16]��ֻ��Ͱ���Ĳ������ݣ�����obj�ڴ��ĵ�ַ
//_freeLists�����ŵ���ͷ���_head,��_head������obj�ڴ��
//��������ĳ����С���ڴ�ֱ���ҵ���������_freeListsȡ�ڴ棬�ͷ����ڴ����_freeLists��Ӧ�±�ĵط�
class ThreadCache {
private:
	FreeList _freeLists[NFREELIST];
public:
	// ������ͷ��ڴ����
	//��Ҫͨ��size�Ĵ�С����ӳ���ҵ���Ӧ��С���ڴ�Ͱ����ȡһ��obj�ڴ��

	//size��ʾһ��obj�ڴ��Ĵ�С
	void* Allocate(size_t size);
	void Deallocate(void*ptr, size_t size);//����һ���ڴ�齫����� _freeLists��
	//����Ϊʲô��������New��Delete�أ���Ϊ��C++��new��delete���˿��ٿռ��ͷſռ���
	//���õ��ù��캯�����������������г�ʼ������������������ֻ�ǵ�����ֻ�ǽ����ڴ�ռ�ķ������ͷ�
	//���������ڴ�ض����Ƕ����

	// �����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index, size_t size);//index��ʾfreeLists�������±�
};

//��һ�ַ���������һ��ȫ�ֵ����ݽṹ����˵һ���������һ����ʽ�ṹ
//������һ��map��ÿ���̶߳����Լ����߳�id��Ȼ����thread cache����ӳ��
//һ���߳�����ͨ��ĳ�������Ϳ�֪�����߳�id��Ȼ��ͨ��map�ң��Ҳ���˵����һ�����߳�
//����һ��thread cache����������id���а󶨣�����̷߳���map����Ҫ������
// map<int, ThreadCache> idCache;//
//�ڶ��ַ���
// TLS  ȫ��thread local storage//�̱߳��ش洢����������ļ��ܵ��Ǹ���
//�߳��е�ջ�Ƕ����
/*Text  Segment������Σ���Data  Segment�����ݶΣ����Ѷ��ǹ����
�������һ������,�ڸ��߳��ж����Ե���,
�������һ��ȫ�ֱ���,�ڸ��߳��ж����Է��ʵ�*/
//�̱߳��ش洢Ҫ����������Ǹ�ÿ���߳̿���һ�������Լ����е�ȫ�ֱ���������

//windows�¼�__declspec(thread)���tls_threadcache����ÿ���̶߳��е�ȫ�ֵ�ָ���ˣ�Linux���Լ����÷�
#ifdef _WIN32
	static __declspec(thread) ThreadCache* pTLSThreadCache = nullptr;
#else
	static __thread ThreadCache* pTLSThreadCache = nullptr;
#endif

//TLS��Ϊ��̬�뾲̬�ģ���̬�ı��紴��ȫ�ֱ���
//��̬����malloc����

//���̵߳��������ڲ����ǰ������������̣�������߳���ǰ��������ô����ص�obj����Ҫ�ͷ�
//��˾����ƻص�����һ�������߳̿������ʱ����ûص����������ڴ��ͷ�
//ʹ�ö�̬��TLS�ô����ǿ���ע��һ�������ĺ������������ͷ�obj
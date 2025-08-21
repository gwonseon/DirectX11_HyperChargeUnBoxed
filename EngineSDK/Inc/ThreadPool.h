#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "Base.h"

BEGIN(Engine)

class CThreadPool final: public CBase
{
public:
	CThreadPool(size_t);
	template<class F,class... Args>
	auto enqueue(F&& f,Args&&... args)-> future<typename result_of<F(Args...)>::type>;

	virtual ~CThreadPool();
private:
	vector< thread > workers; // 워커 스레드 보관하는 벡터

	queue< function<void()> > tasks; // 공유 버퍼, 실행 대기중인 작업 저장하는 큐

	mutex queue_mutex; // 큐 접근 보호 
	condition_variable condition; // 조건 변수, 신호를 주고 받는 기능만 제공
	bool stop; // 스레드풀 종료 플래그
};


// 작업 추가, 큐에 작업 넣고 future 반환
template<class F,class... Args> // F : 호출할 함수 타입, Args : 함수 인자들 
auto CThreadPool::enqueue(F&& f,Args&&... args)
-> future<typename result_of<F(Args...)>::type>
{
	using return_type = typename result_of<F(Args...)>::type;// 반환 타입 정의

	// 전달 받은 함수와 인자를 packaged_task 로 묶는다
	auto task = make_shared< packaged_task<return_type()> >(
		bind(forward<F>(f),forward<Args>(args)...)
	);
	// 작업 결과를 받을 future 객체
	future<return_type> res = task->get_future();
	{
		// 큐에 작업을 추가하기 전에 mutex로 보호
		unique_lock<mutex> lock(queue_mutex); 

		// 종료 상태라면 예외 처리
		if(stop)
			throw runtime_error("enqueue on stopped ThreadPool");
		// 큐에 작업 추가
		tasks.emplace([task]() { (*task)(); });
	}
	// 대기 중인 스레드 하나를 깨워 작업 하도록 함
	condition.notify_one();  // 조건 변수를 기다리고 있는 스레드들 중 하나를 깨운다
	return res;
}


END
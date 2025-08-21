#include "ThreadPool.h"

CThreadPool::CThreadPool(size_t threads)
	: stop(false)
{
	for(size_t i = 0; i < threads; ++i) 
	{
		// threads 수만큼 스레드 생성해 workers 에 저장
		workers.emplace_back(
			[this]
			{
				while(true)
				{
					function<void()> task;
					{
						// 해당 스레드에 lock 걸어서 큐 접근 보호
						// unique_lock : 뮤텍스를 잠그거나 해제하는 시점을 개발자가 제어할 수 있다.
						unique_lock<mutex> lock(this->queue_mutex);

						// wait 호출하는 스레드는 락 객체 점유 상태여야 함, 
						// wait 호출되면 해당 락 객체의 unlock 호출되고 스레드 blocking 됨
						// 작업이 들어오거나 stop 플래그가 true가 될 때까지 대기한다.
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						// 종료 요청이 오거나 남은 작업이 없다면 스레드 종료해라
						if(this->stop && this->tasks.empty())
							return;
						// tasks 큐에서 대기 중인 작업 하나 꺼내서 task에 이동시켜라
						task = move(this->tasks.front());
						this->tasks.pop();
					}
					// 락 해제한 후 실제 작업 실행해라
					task();
				}
			}
		);
	}
}


CThreadPool::~CThreadPool()
{
	{
		// stop 플래그를 true 로 해서 더이상 작업 받지 않도록 설정
		unique_lock<mutex> lock(queue_mutex);
		stop = true;
	}
	// 대기중인 모든 스레드 깨워 종료 루틴 실행하게 함
	condition.notify_all();
	// 모든 스레드 종료될 때까지 대기
	for(thread& worker : workers)
		worker.join();
}
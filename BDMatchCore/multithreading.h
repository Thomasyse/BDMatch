#pragma once
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

class fixed_thread_pool {
public:
	explicit fixed_thread_pool(size_t thread_count, std::atomic_flag *keep_processing0 = nullptr)
		: data_(std::make_shared<data>()) {
		data_->keep_processing = keep_processing0;
		for (size_t i = 0; i < thread_count; ++i) {
			std::thread([data = data_] {
				std::unique_lock<std::mutex> lk(data->mtx_);
				for (;;) {
					if (!data->tasks_.empty()) {
						if (data->keep_processing && !data->keep_processing->test_and_set()) {
							while (!data->tasks_.empty()) {
								data->tasks_.pop();
								data->task_num_--;
							}
							data->keep_processing->clear();
							if (data->task_num_ == 0)data->fin_.notify_one();
							lk.unlock();
							lk.lock();
						}
						else {
							auto current = std::move(data->tasks_.front());
							data->tasks_.pop();
							lk.unlock();
							current();
							lk.lock();
							data->task_num_--;
							if (data->task_num_ == 0) {
								data->fin_.notify_one();
							}
						}
					}
					else if (data->is_shutdown_) {
						break;
					}
					else {
						data->cond_.wait(lk);
					}
				}
			}).detach();
		}
	}

	fixed_thread_pool() = default;
	fixed_thread_pool(fixed_thread_pool&&) = default;

	~fixed_thread_pool() {
		if ((bool)data_) {
			{
				std::lock_guard<std::mutex> lk(data_->mtx_);
				data_->is_shutdown_ = true;
			}
			data_->cond_.notify_all();
		}
	}
	
	template <class F>
	void execute(F&& task) {
		{
			std::lock_guard<std::mutex> lk(data_->mtx_);
			data_->tasks_.emplace(std::forward<F>(task));
		}
		data_->task_num_++;
		data_->cond_.notify_one();
	}

	template <class F>
	void execute_batch(std::vector<F> &batch) {
		std::lock_guard<std::mutex> lk(data_->mtx_);
		for(auto &task : batch)
		{
			data_->tasks_.emplace(task);
		}
		data_->task_num_ += batch.size();
		data_->cond_.notify_all();
	}

	void wait() {
		std::unique_lock<std::mutex> lk(data_->mtx_);
		if (data_->task_num_ > 0) data_->fin_.wait(lk);
		lk.unlock();
	}

private:
	struct data {
		std::mutex mtx_;
		std::condition_variable cond_;
		std::condition_variable fin_;
		std::atomic<int> task_num_ = 0;
		std::atomic_flag *keep_processing = nullptr;
		bool is_shutdown_ = false;
		std::queue<std::function<void()>> tasks_;
	};
	std::shared_ptr<data> data_;
};
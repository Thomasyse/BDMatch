#pragma once
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <stop_token>
#include <chrono>

#include "targetver.h"

class fixed_thread_pool {
public:
	explicit fixed_thread_pool(size_t thread_count, std::stop_source& stop_src)
		: data_(std::make_shared<data>()) {
		for (size_t i = 0; i < thread_count; ++i) {
			std::jthread([stop_tk = stop_src.get_token(), data = data_] {
				std::unique_lock<std::mutex> lk(data->mtx_);
				for (;;) {
					if (!data->tasks_.empty()) {
						if (stop_tk.stop_requested()) {
							data->task_num_ -= data->tasks_.size();
							data->tasks_.clear();
							if (data->task_num_ == 0) {
								data->fin_.notify_one();
							}
						}
						else {
							auto current = std::move(data->tasks_.front());
							data->tasks_.pop_front();
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
			data_->tasks_.emplace_back(std::forward<F>(task));
			data_->task_num_++;
		}
		data_->cond_.notify_one();
	}

	template <class F>
	void execute_batch(const std::vector<F>& batch) {
		{
			std::lock_guard<std::mutex> lk(data_->mtx_);
			data_->tasks_.insert(data_->tasks_.end(), batch.begin(), batch.end());
			data_->task_num_ += batch.size();
		}
		data_->cond_.notify_all();
	}

	void wait() {
		std::unique_lock<std::mutex> lk(data_->mtx_);
		if (data_->task_num_ > 0)data_->fin_.wait(lk);
		lk.unlock();
	}

private:
	struct data {
		std::mutex mtx_;
		std::condition_variable cond_;
		std::condition_variable fin_;
		size_t task_num_ = 0;
		bool is_shutdown_ = false;
		std::deque<std::function<void()>> tasks_;
	};
	std::shared_ptr<data> data_;
};
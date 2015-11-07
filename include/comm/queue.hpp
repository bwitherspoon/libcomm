/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef COMM_QUEUE_HPP_
#define COMM_QUEUE_HPP_

#include <condition_variable> // for std::condition_variable
#include <mutex>              // for std::mutex
#include <utility>            // for std::move
#include <queue>              // for std::queue

namespace comm {

template<typename T>
class queue {
public:
  T pop()
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    while (m_queue.empty()) m_condition.wait(lock);

    auto item = m_queue.front();
    m_queue.pop();

    return std:move(item);
  }

  void push(T item)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

  }

private:
  std::queue<T> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_condition;
};

}

#endif /* COMM_QUEUE_HPP_ */

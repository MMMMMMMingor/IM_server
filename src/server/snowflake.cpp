//
// Created by Mingor on 2020/4/5.
//
#include <server/snowflake.h>
#include <sys/time.h>

inline uint64_t now_in_millis() {
  timeval tv;
  gettimeofday(&tv, 0);
  return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

inline uint64_t wait_until_next_millis(uint64_t last_timestamp) {
  uint64_t timestamp = now_in_millis();
  while (timestamp <= last_timestamp) {
    timestamp = now_in_millis();
  }
  return timestamp;
}

// twitter 雪花算法 ——> 用于生成session_id
IdWorker::IdWorker(uint16_t worker_id, uint16_t data_center_id)
    : m_worker_id(worker_id), m_data_center_id(data_center_id) {}

uint64_t IdWorker::get_next_id() {
  std::lock_guard<std::mutex> g(m_mutex);
  uint64_t timestamp = now_in_millis();

  // 在当前秒内
  if (m_last_timestamp == timestamp) {
    m_sequence = (m_sequence + 1) & 0xFFF;
    if (m_sequence == 0) {
      timestamp = wait_until_next_millis(m_last_timestamp);
    }
  } else {
    m_sequence = 0;
  }

  m_last_timestamp = timestamp;
  return ((timestamp & 0x1FFFFFF) << 22 | (m_data_center_id & 0x1F) << 17 |
          (m_worker_id & 0x1F) << 12 | (m_sequence & 0xFFF));
}

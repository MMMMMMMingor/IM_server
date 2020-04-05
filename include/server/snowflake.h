#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include <mutex>

// twitter 雪花算法 ——> 用于生成session_id
class IdWorker {
public:
  IdWorker(uint16_t worker_id, uint16_t data_center_id);

  uint64_t get_next_id();

protected:
  std::mutex m_mutex;
  uint16_t m_worker_id{0};
  uint16_t m_data_center_id{0};
  uint64_t m_last_timestamp{0};
  uint32_t m_sequence{0};
};

#endif
#ifndef SNOWFLAKE_HPP
#define SNOWFLAKE_HPP

#include <mutex>
#include <sys/time.h>

inline uint64_t now_in_millis() {
  timeval tv;
  gettimeofday(&tv, 0);
  return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

uint64_t wait_until_next_millis(uint64_t last_timestamp) {
  uint64_t timestamp = now_in_millis();
  while (timestamp <= last_timestamp) {
    timestamp = now_in_millis();
  }
  return timestamp;
}

// 结构为：
//
// 0---0000000000 0000000000 0000000000 0000000000 0 --- 00000 ---00000
// ---0000000000 00
// 在上面的字符串中，第一位为未使用（实际上也可作为long的符号位），接下来的41位为毫秒级时间，然后5位datacenter标识位，
// 5位机器ID（并不算标识符，实际是为线程标识），然后12位该毫秒内的当前毫秒内的计数，加起来刚好64位，为一个Long型。
//
// 这样的好处是，整体上按照时间自增排序，并且整个分布式系统内不会产生ID碰撞（由datacenter和机器ID作区分），
// 并且效率较高，经测试，snowflake每秒能够产生26万ID左右，完全满足需要。
//
class IdWorker {
public:
  IdWorker(uint16_t worker_id, uint16_t data_center_id)
      : m_worker_id(worker_id), m_data_center_id(data_center_id) {}

  uint64_t get_next_id() {
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

protected:
  std::mutex m_mutex;
  uint16_t m_worker_id{0};
  uint16_t m_data_center_id{0};
  uint64_t m_last_timestamp{0};
  uint32_t m_sequence{0};
};

#endif
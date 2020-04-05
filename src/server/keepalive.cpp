//
// Created by Mingor on 2020/4/5.
//
#include <server/common_util.hpp>
#include <server/keepalive.h>

/**
 * 开始心跳检测
 * @param ctx
 */
void KeepAlive::init(SessionPool *session_pool) {
  LOG_F(INFO, "开始心跳检测~~~");
  auto func = [this, session_pool]() {
    while (!m_finished) {
      std::this_thread::sleep_for(m_check_interval);
      keepalive_check(session_pool);
    }
  };
  std::thread t{func};

  t.detach();
}

/**
 * 心跳检测：
 * 过期的session：清除
 * 未过期的session：发送心跳
 * @param ctx
 */
void KeepAlive::keepalive_check(SessionPool *session_pool) {
  im_message::Message message;
  message.set_type(im_message::HeadType::KEEPALIVE_REQUEST);

  auto keepalive = [this, &message, session_pool](uint64_t session_id,
                                                  Session *session) {
    Session::Time now = std::chrono::system_clock::now();

    Munites diff = std::chrono::duration_cast<Munites>(
        now - session->get_last_keepalive());

    // 如果超时，则移除session
    if (diff.count() > m_invalid_interval.count()) {
      LOG_F(INFO, "%s 心跳超时，服务器主动断开连接",
            session->get_username().c_str());
      session_pool->remove_session(session_id);
    } else {
      LOG_F(INFO, "发送心跳包 ---> %s ", session->get_username().c_str());
      send_message_safe(session, message);
    }
  };

  session_pool->for_each(keepalive);
}

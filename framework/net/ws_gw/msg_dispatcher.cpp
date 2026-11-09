#include "msg_dispatcher.h"

#include "tcp_listener.h"
#include "ws_session.h"

WsGwContext::WsGwContext()  = default;
WsGwContext::~WsGwContext() = default;

void MsgDispatcher::add_session(const std::shared_ptr<WsSession> &session)
{
    if (!session) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    const std::string           id = session->get_id();

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        if ((*it)->get_id() == id)
        {
            m_sessions.erase(it);
            break;
        }
    }

    m_sessions.push_back(session);
}

void MsgDispatcher::remove_session(const std::shared_ptr<WsSession> &session)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        if (it->get() == session.get())
        {
            m_sessions.erase(it);
            break;
        }
    }
}

WsGwError MsgDispatcher::send_notify(const std::string &id, wsgw::Json &value)
{
    std::shared_ptr<WsSession> target;
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto &session : m_sessions)
        {
            if (session->get_id() == id)
            {
                target = session;
                break;
            }
        }
    }

    if (!target) return WSGW_NOT_FOUND_CLIENT;

    target->send_msg(value);
    return WSGW_OK;
}

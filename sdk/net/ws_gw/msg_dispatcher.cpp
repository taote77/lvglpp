
#include "msg_dispatcher.h"
#include <boost/log/trivial.hpp>
#include <boost/version.hpp>
#include <mutex>

WsGwContext::WsGwContext()
{}
WsGwContext::~WsGwContext()
{}

MsgDispatcher::MsgDispatcher(net::io_context &ioc) : m_ioc(ioc)
{}

WsGwError MsgDispatcher::send_notify(const std::string &id, boost::json::object &v)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        if (id == (*it)->get_id())
        {
            (*it)->send_msg(v);
            return WSGW_OK;
        }
    }

    return WSGW_NOT_FOUND_CLIENT;
}

bool MsgDispatcher::check_req(const tcp::endpoint &remote_ep, const websocket::request_type &req, std::string &id)
{
    if (remote_ep.address().to_string() == "127.0.0.1")
    {
        id = "127.0.0.1";
        return true;
    }

    auto it = req.find("id");
    if (it != req.end())
    {
#if BOOST_VERSION < 108000
        id = it->value().to_string();
#else
        id = it->value();
#endif
        return true;
    }
    return false;
}

void MsgDispatcher::add_session(std::shared_ptr<WsSession> &ses)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    std::string                 id = ses->get_id();
    auto                        it = m_sessions.begin();
    for (; it != m_sessions.end(); ++it)
    {
        if (id == (*it)->get_id())
        {
            m_sessions.erase(it);
            break;
        }
    }
    m_sessions.push_back(ses);
}

void MsgDispatcher::remove_session(std::shared_ptr<WsSession> &ses)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    auto                        it = m_sessions.begin();
    for (; it != m_sessions.end(); ++it)
    {
        if (ses.get() == it->get())
        {
            m_sessions.erase(it);
            break;
        }
    }
}

#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class session;
class msg_item
{
    object m_msg;
    std::weak_ptr<session> m_session;

public:
    msg_item(const object &msg, std::shared_ptr<session> session)
        : m_msg(msg), m_session(session)
    {}

    const object& get_msg()const{ return m_msg; }
    std::shared_ptr<session> get_session()const{ return m_session.lock(); }
};


class received_msg_queue
{
    std::list<std::shared_ptr<msg_item>> m_queue;
    boost::mutex m_mutex;
public:
    void enqueue(std::shared_ptr<msg_item> item)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_queue.push_back(item);
    }

    std::shared_ptr<msg_item> dequeue()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        if(m_queue.empty()){
            return std::shared_ptr<msg_item>();
        }
        else{
            auto item=m_queue.front();
            m_queue.pop_front();
            return item;
        }
    }
};


}}}

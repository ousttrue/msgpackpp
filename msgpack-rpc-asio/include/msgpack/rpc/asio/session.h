#pragma once

namespace msgpack {
namespace rpc {
namespace asio {


class request_factory
{
    ::msgpack::rpc::msgid_t m_next_msgid;
public:
    request_factory()
        : m_next_msgid(1)
    {
    }

    ::msgpack::rpc::msgid_t next_msgid()
    {
        return m_next_msgid++;
    }

    // 0
    ::msgpack::rpc::msg_request<std::string, std::tuple<>> 
        create(const std::string &method)
        {
            ::msgpack::rpc::msgid_t msgid = next_msgid();
            typedef std::tuple<> Parameter;
            return ::msgpack::rpc::msg_request<std::string, Parameter>(
                    method, Parameter(), msgid);
        }
    // 1
    template<typename A1>
        ::msgpack::rpc::msg_request<std::string, std::tuple<A1>> 
        create(const std::string &method, A1 a1)
        {
            ::msgpack::rpc::msgid_t msgid = next_msgid();
            typedef std::tuple<A1> Parameter;
            return ::msgpack::rpc::msg_request<std::string, Parameter>(
                    method, Parameter(a1), msgid);
        }
    // 2
    template<typename A1, typename A2>
        ::msgpack::rpc::msg_request<std::string, std::tuple<A1, A2>> 
        create(const std::string &method, A1 a1, A2 a2)
        {
            ::msgpack::rpc::msgid_t msgid = next_msgid();
            typedef std::tuple<A1, A2> Parameter;
            return ::msgpack::rpc::msg_request<std::string, Parameter>(
                    method, Parameter(a1, a2), msgid);
        }
};


class func_call
{
public:
    enum STATUS_TYPE
    {
    STATUS_WAIT,
    STATUS_RECEIVED,
    STATUS_ERROR,
    };
private:
    STATUS_TYPE m_status;
    ::msgpack::object m_result;
    ::msgpack::object m_error;
    std::string m_request;
    boost::mutex m_mutex;
    boost::condition_variable_any m_cond;
public:
    func_call(const std::string &s)
        : m_status(STATUS_WAIT), m_request(s)
    {
    }

    void set_result(const ::msgpack::object &result)
    {
        if(m_status!=STATUS_WAIT){
            throw rpc_error("already finishded");
        }
        boost::mutex::scoped_lock lock(m_mutex);
        m_result=result;
        m_status=STATUS_RECEIVED;
        m_cond.notify_all();
    }

    void set_error(const ::msgpack::object &error)
    {
        if(m_status!=STATUS_WAIT){
            throw rpc_error("already finishded");
        }
        boost::mutex::scoped_lock lock(m_mutex);
        m_error=error;
        m_status=STATUS_ERROR;
        m_cond.notify_all();
    }

    // blocking
    func_call& sync()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        if(m_status==STATUS_WAIT){
            m_cond.wait(m_mutex);
        }
        return *this;
    }

    template<typename R>
        R& convert(R *value)const
        {
            if(m_status==STATUS_RECEIVED){
                m_result.convert(value);
                return *value;
            }
            else{
                throw rpc_error("not ready");
            }
        }

    std::string string()const
    {
        std::stringstream ss;
        ss << m_request << " = ";
        switch(m_status)
        {
            case func_call::STATUS_WAIT:
                ss << "?";
                break;
            case func_call::STATUS_RECEIVED:
                ss << m_result;
                break;
            case func_call::STATUS_ERROR:
                ss << "!";
                break;
            default:
                ss << "!?";
                break;
        }

        return ss.str();
    }
};
std::ostream &operator<<(std::ostream &os, const func_call &request)
{
    os << request.string();
    return os;
}

class session: public std::enable_shared_from_this<session>
{
    request_factory m_request_factory;
    boost::asio::ip::tcp::socket m_socket;
    enum { max_length = 1024 };
    char m_data[max_length];
    unpacker m_pac;
    // server queue
    std::weak_ptr<server_request_queue> m_server_queue;
    // write queue
    bool m_writing;
    std::list<std::shared_ptr<msgpack::sbuffer>> m_write_queue;
    std::map<msgpack::rpc::msgid_t, std::shared_ptr<func_call>> m_requestMap;
    // must shard_ptr
    session(boost::asio::io_service& io_service, std::shared_ptr<server_request_queue> server_queue)
        : m_socket(io_service), m_pac(1024), m_server_queue(server_queue),
        m_writing(false)
    {
    }
public:

    ~session()
    {
    }

    static std::shared_ptr<session> create(boost::asio::io_service &io_service,
            std::shared_ptr<server_request_queue> server_queue=std::shared_ptr<server_request_queue>())
    {
        return std::shared_ptr<session>(new session(io_service, server_queue));
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    void connect(boost::asio::ip::tcp::endpoint endpoint)
    {
        auto shared=shared_from_this();
        m_socket.async_connect(endpoint, [shared](const boost::system::error_code &error){
                if(error){
                std::cerr << "error !" << std::endl;
                }
                else{
                shared->startRead();
                }

                }); 
    }

    // 0
    std::shared_ptr<func_call> call_async(const std::string &method)
    {
        auto request=m_request_factory.create(method);
        return send_async(request);
    }
    // 1
    template<typename A1>
        std::shared_ptr<func_call> call_async(const std::string &method, A1 a1)
        {
            auto request=m_request_factory.create(method, a1);
            return send_async(request);
        }
    // 2
    template<typename A1, typename A2>
        std::shared_ptr<func_call> call_async(const std::string &method, A1 a1, A2 a2)
        {
            auto request=m_request_factory.create(method, a1, a2);
            return send_async(request);
        }

    // 0
    template<typename R>
        R &call_sync(R *value, const std::string &method)
        {
            auto request=m_request_factory.create(method);
            auto call=send_async(request);
            call->sync().convert(value);
            return *value;
        }
    // 1
    template<typename R, typename A1>
        R &call_sync(R *value, const std::string &method, A1 a1)
        {
            auto request=m_request_factory.create(method, a1);
            auto call=send_async(request);
            call->sync().convert(value);
            return *value;
        }
    // 2
    template<typename R, typename A1, typename A2>
        R &call_sync(R *value, const std::string &method, A1 a1, A2 a2)
        {
            auto request=m_request_factory.create(method, a1, a2);
            auto call=send_async(request);
            call->sync().convert(value);
            return *value;
        }

    // read connection
    void startRead()
    {
        auto pac=&m_pac;
        auto shared=shared_from_this();
        m_socket.async_read_some(
                boost::asio::buffer(pac->buffer(), pac->buffer_capacity()),
                [shared, pac](const boost::system::error_code &error,
                    size_t bytes_transferred)
                {
                auto server_queue=shared->m_server_queue.lock();
                if (error) {
                int a=0;
                // todo
                }
                else {

                char *p=pac->buffer();
                pac->buffer_consumed(bytes_transferred);
                msgpack::unpacked result;

                // extract object
                while(pac->execute()) {
                size_t size=pac->parsed_size();
                // msgpack message
                ::msgpack::object msg = pac->data();
                ::msgpack::rpc::msgid_t msgid=0;
                try{
                    ::msgpack::rpc::msg_rpc rpc;
                    msg.convert(&rpc);

                    switch(rpc.type) {
                        case ::msgpack::rpc::REQUEST: 
                            server_queue->enqueue(std::make_shared<queue_item>(msg, shared));
                            break;

                        case ::msgpack::rpc::RESPONSE: 
                            {
                                ::msgpack::rpc::msg_response<object, object> res;
                                msg.convert(&res);
                                auto found=shared->m_requestMap.find(res.msgid);
                                if(found!=shared->m_requestMap.end()){
                                    // ToDo: error check
                                    found->second->set_result(res.result);
                                }
                                else{
                                    // ToDo:error
                                    int a=0;
                                }
                            }
                            break;

                        case ::msgpack::rpc::NOTIFY: 
                            {
                                ::msgpack::rpc::msg_notify<object, object> req;
                                msg.convert(&req);
                                //req.method, req.param);
                            }
                            break;

                        default:
                            throw msgpack::rpc::rpc_error("rpc type error");
                    }

                }
                catch(::msgpack::rpc::rpc_error ex){
                    //shared->enqueue(shared->create_error_message(msgid, ex));
                }
                catch(...){
                    //shared->enqueue(shared->create_error_message(msgid, ::msgpack::rpc::rpc_error("unknown error")));
                }

                pac->reset();
                p+=size;
                }

                // read loop
                shared->startRead();
                }
                });
    }

    // write
    void enqueue(std::shared_ptr<msgpack::sbuffer> msg)
    {
        // lock
        if(m_writing){
            // queueing...
            m_write_queue.push_back(msg);
        }
        else{
            // start async write
            m_writing=true;
            startWrite(msg);
        }
    }

private:
    template<typename Parameter>
        std::shared_ptr<func_call> send_async(const ::msgpack::rpc::msg_request<std::string, Parameter> &msgreq)
        {
    auto sbuf=std::make_shared<msgpack::sbuffer>();
    ::msgpack::pack(*sbuf, msgreq);

    std::stringstream ss;
    ss << msgreq.method << msgreq.param;
    auto req=std::make_shared<func_call>(ss.str());
    m_requestMap.insert(std::make_pair(msgreq.msgid, req));

    enqueue(sbuf);

    return req;
        }

    void startWrite(std::shared_ptr<msgpack::sbuffer> msg)
    {
        // for vc
        auto shared=shared_from_this();
        shared->socket().async_write_some(
                boost::asio::buffer(msg->data(), msg->size()),
                [shared, msg](
                    const boost::system::error_code& error, 
                    size_t bytes_transferred)
                {
                if(error){
                std::cerr << "write error" << std::endl;
                }

                // lock
                if(shared->m_write_queue.empty()){
                shared->m_writing=false;
                }
                else{
                shared->m_writing=true;
                auto next_msg=shared->m_write_queue.front();
                shared->m_write_queue.pop_front();
                shared->startWrite(next_msg);
                }
                });
    }

    template<typename E>
        std::shared_ptr<msgpack::sbuffer> create_error_message(msgpack::rpc::msgid_t msgid, const E &error)
        {
            // error type
            ::msgpack::rpc::msg_response<msgpack::type::nil, E> msgres(
                    ::msgpack::type::nil(), 
                    error, 
                    msgid);
            // result
            auto sbuf=std::make_shared<msgpack::sbuffer>();
            msgpack::pack(*sbuf, msgres);
            return sbuf;
        }
};

}}}

#ifndef QGL_CORE_SIGNAL_H
#define QGL_CORE_SIGNAL_H

#include <functional>
#include <vector>
#include <memory>
#include <algorithm>

namespace QGL {

// 前向声明
class Object;

// 连接类型
enum class ConnectionType {
    AutoConnection,    // 自动选择连接类型
    DirectConnection,  // 直接连接（同步）
    QueuedConnection   // 队列连接（异步）
};

// 连接句柄，用于断开连接
class Connection {
public:
    Connection() = default;
    explicit Connection(std::weak_ptr<void> connection) : m_connection(connection) {}
    
    bool isValid() const { return !m_connection.expired(); }
    void disconnect();
    
private:
    std::weak_ptr<void> m_connection;
};

// 基础信号接口
class SignalBase {
public:
    virtual ~SignalBase() = default;
    virtual void disconnectAll() = 0;
};

// 模板化信号类
template<typename... Args>
class Signal : public SignalBase {
private:
    struct ConnectionData {
        std::function<void(Args...)> slot;
        std::weak_ptr<Object> receiver;
        ConnectionType type;
        bool valid = true;
        
        ConnectionData(std::function<void(Args...)> s, 
                      std::weak_ptr<Object> r = std::weak_ptr<Object>(),
                      ConnectionType t = ConnectionType::DirectConnection)
            : slot(std::move(s)), receiver(r), type(t) {}
    };
    
    using ConnectionPtr = std::shared_ptr<ConnectionData>;
    std::vector<ConnectionPtr> m_connections;
    
public:
    Signal() = default;
    ~Signal() { disconnectAll(); }
    
    // 禁用拷贝，允许移动
    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&&) = default;
    Signal& operator=(Signal&&) = default;
    
    // 连接函数对象/lambda
    Connection connect(std::function<void(Args...)> slot, 
                      ConnectionType type = ConnectionType::DirectConnection) {
        auto connection = std::make_shared<ConnectionData>(std::move(slot), std::weak_ptr<Object>(), type);
        m_connections.push_back(connection);
        return Connection(connection);
    }
    
    // 连接成员函数
    template<typename T>
    Connection connect(T* receiver, void (T::*method)(Args...), 
                      ConnectionType type = ConnectionType::DirectConnection) {
        static_assert(std::is_base_of_v<Object, T>, "Receiver must inherit from Object");
        
        auto slot = [receiver, method](Args... args) {
            (receiver->*method)(args...);
        };
        
        auto connection = std::make_shared<ConnectionData>(
            std::move(slot), 
            std::static_pointer_cast<Object>(receiver->shared_from_this()),
            type
        );
        m_connections.push_back(connection);
        return Connection(connection);
    }
    
    // 连接 const 成员函数
    template<typename T>
    Connection connect(T* receiver, void (T::*method)(Args...) const, 
                      ConnectionType type = ConnectionType::DirectConnection) {
        static_assert(std::is_base_of_v<Object, T>, "Receiver must inherit from Object");
        
        auto slot = [receiver, method](Args... args) {
            (receiver->*method)(args...);
        };
        
        auto connection = std::make_shared<ConnectionData>(
            std::move(slot), 
            std::static_pointer_cast<Object>(receiver->shared_from_this()),
            type
        );
        m_connections.push_back(connection);
        return Connection(connection);
    }
    
    // 发射信号
    void emit(Args... args) {
        // 清理无效连接
        cleanupConnections();
        
        // 调用所有有效连接
        for (auto& connection : m_connections) {
            if (connection->valid) {
                // 检查接收者是否仍然有效
                if (!connection->receiver.expired() || connection->receiver.lock() == nullptr) {
                    try {
                        connection->slot(args...);
                    } catch (...) {
                        // 忽略槽函数中的异常，避免影响其他连接
                    }
                }
            }
        }
    }
    
    // operator() 重载，支持函数调用语法
    void operator()(Args... args) {
        emit(args...);
    }
    
    // 断开所有连接
    void disconnectAll() override {
        for (auto& connection : m_connections) {
            connection->valid = false;
        }
        m_connections.clear();
    }
    
    // 断开特定接收者的连接
    template<typename T>
    void disconnect(T* receiver) {
        static_assert(std::is_base_of_v<Object, T>, "Receiver must inherit from Object");
        
        auto receiverPtr = std::static_pointer_cast<Object>(receiver->shared_from_this());
        for (auto& connection : m_connections) {
            if (auto locked = connection->receiver.lock()) {
                if (locked == receiverPtr) {
                    connection->valid = false;
                }
            }
        }
        cleanupConnections();
    }
    
    // 获取连接数量
    size_t connectionCount() const {
        return std::count_if(m_connections.begin(), m_connections.end(),
                           [](const ConnectionPtr& conn) { return conn->valid; });
    }
    
    // 检查是否有连接
    bool isConnected() const {
        return connectionCount() > 0;
    }
    
private:
    void cleanupConnections() {
        m_connections.erase(
            std::remove_if(m_connections.begin(), m_connections.end(),
                          [](const ConnectionPtr& conn) { 
                              return !conn->valid || conn->receiver.expired(); 
                          }),
            m_connections.end()
        );
    }
    
    friend class Connection;
    
    void disconnectConnection(ConnectionPtr connection) {
        if (connection) {
            connection->valid = false;
        }
        cleanupConnections();
    }
};

// 便捷宏定义
#define SIGNAL(name, ...) \
    QGL::Signal<__VA_ARGS__> name

#define SLOT(receiver, method) \
    receiver, method

#define CONNECT(sender, signal, receiver, slot) \
    sender->signal.connect(receiver, slot)

#define CONNECT_LAMBDA(sender, signal, lambda) \
    sender->signal.connect(lambda)

} // namespace QGL

#endif // QGL_CORE_SIGNAL_H
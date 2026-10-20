#ifndef LVGLPP_CONNECTION_H
#define LVGLPP_CONNECTION_H

#include <functional>
#include <memory>
#include <utility>

namespace lvglpp::core {

namespace detail {

/**
 * Shared state behind a Connection token.
 *
 * The owner (Object/BaseItem) marks the state disconnected when the slot is
 * removed or the sender is destroyed. Calling disconnect() is idempotent and
 * safe even after the sender has gone away.
 */
struct ConnectionState {
    bool                  connected = true;
    std::function<void()> on_disconnect;

    void disconnect()
    {
        if (!connected)
        {
            return;
        }
        connected     = false;
        auto fn       = std::move(on_disconnect);
        on_disconnect = nullptr;
        if (fn)
        {
            fn();
        }
    }
};

} // namespace detail

/**
 * Non-owning disconnect token returned by connect()/onXxx().
 *
 * Copyable. Destruction does NOT disconnect, so ignoring the return value of
 * onXxx() keeps the handler connected (the common fire-and-forget usage).
 * Use ScopedConnection for deterministic RAII disconnection.
 */
class Connection
{
public:
    Connection() = default;

    explicit Connection(std::shared_ptr<detail::ConnectionState> state) : state_(std::move(state))
    {}

    void disconnect()
    {
        if (state_)
        {
            state_->disconnect();
        }
    }

    bool connected() const
    {
        return state_ && state_->connected;
    }

private:
    std::shared_ptr<detail::ConnectionState> state_;
};

/**
 * Move-only RAII wrapper: disconnects on destruction.
 */
class ScopedConnection
{
public:
    ScopedConnection() = default;

    explicit ScopedConnection(Connection connection) : connection_(std::move(connection))
    {}

    ~ScopedConnection()
    {
        connection_.disconnect();
    }

    ScopedConnection(const ScopedConnection &)            = delete;
    ScopedConnection &operator=(const ScopedConnection &) = delete;

    ScopedConnection(ScopedConnection &&other) noexcept : connection_(std::move(other.connection_))
    {
        other.connection_ = Connection();
    }

    ScopedConnection &operator=(ScopedConnection &&other) noexcept
    {
        if (this != &other)
        {
            connection_.disconnect();
            connection_       = std::move(other.connection_);
            other.connection_ = Connection();
        }
        return *this;
    }

    /// Stop managing the connection without disconnecting it.
    void release()
    {
        connection_ = Connection();
    }

    Connection &connection()
    {
        return connection_;
    }

    const Connection &connection() const
    {
        return connection_;
    }

    bool connected() const
    {
        return connection_.connected();
    }

private:
    Connection connection_;
};

} // namespace lvglpp::core

#endif // LVGLPP_CONNECTION_H

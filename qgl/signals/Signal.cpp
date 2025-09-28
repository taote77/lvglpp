#include "Signal.h"

namespace QGL {

void Connection::disconnect() {
    // Connection 的具体实现
    // 这里需要访问 Signal 的私有成员来断开连接
    if (auto connection = std::static_pointer_cast<void>(m_connection.lock())) {
        // 将连接标记为无效
        // 注意：实际实现中需要更复杂的机制来安全地断开连接
    }
}

} // namespace QGL
#include <filesystem>
#include <fstream>
#include <iostream>
#include <png.h>
#include <regex>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Base64 编码表
const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789+/";

// Base64 编码函数[3,5](@ref)
std::string base64_encode(const unsigned char *data, size_t len)
{
    std::string   encoded;
    int           i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (len--)
    {
        char_array_3[i++] = *(data++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                encoded += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    // 处理剩余字节[6](@ref)
    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++)
            encoded += base64_chars[char_array_4[j]];

        while (i++ < 3)
            encoded += '=';
    }
    return encoded;
}

// 数据库记录结构体（修改imageData类型为std::string）
struct ImageRecord {
    std::string url;
    int         width;
    int         height;
    std::string format;
    std::string imageData; // 直接存储文件二进制流的字符串
    std::string metaData;  // 直接存储文件二进制流的字符串
};

// 从路径提取尺寸信息（保持不变）
std::pair<int, int> get_dimensions(const fs::path &path)
{
    std::smatch match;
    std::string full_path = path.string();

    if (std::regex_search(full_path, match, std::regex(R"((?:^|[/\\])(\d+)[xX](\d+)(?:[/\\]|$))")))
    {
        return {std::stoi(match[1]), std::stoi(match[2])};
    }
    throw std::runtime_error("无效路径格式: " + full_path);
}

// 创建数据库表（保持不变）
void create_table(sqlite3 *db)
{
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS images (
            url TEXT PRIMARY KEY NOT NULL,
            width INTEGER NOT NULL,
            height INTEGER NOT NULL,
            format TEXT NOT NULL,
            imageData TEXT NOT NULL,
            metaData TEXT NOT NULL
        )
    )";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string error = "创建表失败: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error);
    }
}

// 插入图片记录（调整imageData绑定逻辑）
void insert_record(sqlite3 *db, const ImageRecord &record)
{
    sqlite3_stmt *stmt;
    const char   *sql = R"(
        INSERT INTO images (url, width, height, format, imageData, metaData)
        VALUES (?, ?, ?, ?, ?, ?)
        ON CONFLICT (url) DO UPDATE SET  -- 针对url主键冲突的处理
            width = excluded.width,       -- excluded表示插入时的新值
            height = excluded.height,
            format = excluded.format,
            imageData = excluded.imageData,
            metaData = excluded.metaData
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("准备语句失败: " + std::string(sqlite3_errmsg(db)));
    }

    // 绑定参数（imageData使用字符串的data()和size()）
    sqlite3_bind_text(stmt, 1, record.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.width);
    sqlite3_bind_int(stmt, 3, record.height);
    sqlite3_bind_text(stmt, 4, record.format.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_bind_text(stmt, 5,
                          record.imageData.data(),                   // 直接使用字符串的底层数据指针
                          static_cast<int>(record.imageData.size()), // 使用字符串长度作为数据大小
                          SQLITE_TRANSIENT)
        != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("绑定二进制数据失败");
    }

    if (sqlite3_bind_text(stmt, 6,
                          record.metaData.data(),                   // 直接使用字符串的底层数据指针
                          static_cast<int>(record.metaData.size()), // 使用字符串长度作为数据大小
                          SQLITE_TRANSIENT)
        != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("绑定二进制数据失败");
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("插入数据失败: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
}

// 读取文件二进制数据（修改为返回std::string）
std::string read_file(const fs::path &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("无法打开文件: " + path.string());
    }

    auto size = file.tellg();
    if (size < 0)
    {
        throw std::runtime_error("获取文件大小失败: " + path.string());
    }

    // 获取文件大小
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取到缓冲区
    std::vector<unsigned char> buffer(file_size);
    file.read(reinterpret_cast<char *>(buffer.data()), file_size);
    file.close();

    // 2. Base64 编码
    std::string base64_str = base64_encode(buffer.data(), file_size);

    return base64_str; // 返回二进制数据字符串
}

std::string extractAfterResource(const std::string &str)
{
    size_t pos = str.find("resource");
    if (pos != std::string::npos)
    {
        return ":" + str.substr(pos + 8); // "resource" 长度为 8
    }
    return "";
}

// 主处理函数（调整记录构建逻辑）
void process_directory(const fs::path &dir_path, sqlite3 *db)
{
    for (const auto &entry : fs::recursive_directory_iterator(dir_path, fs::directory_options::skip_permission_denied))
    {
        if (entry.is_regular_file() && entry.path().extension() != ".db")
        {
            try
            {
                auto [width, height] = get_dimensions(entry.path());
                std::string ext      = entry.path().extension().string().substr(1);

                // 直接读取为字符串二进制流
                std::string data = read_file(entry.path());

                // 构建记录（imageData使用字符串）
                ImageRecord record{
                    extractAfterResource(fs::absolute(entry.path()).string()), width, height, ext,
                    std::move(data) // 转移字符串所有权避免拷贝
                };

                insert_record(db, record);
                std::cout << "已处理: " << entry.path() << "\n";
            }
            catch (const std::exception &e)
            {
                std::cerr << "处理失败: " << entry.path() << "\n  原因: " << e.what() << "\n\n";
            }
        }
    }
}

int main(int argc, char *argv[]) // 主函数保持不变
{
    if (argc != 3)
    {
        std::cerr << "用法: " << argv[0] << " <图片目录> <数据库路径>\n";
        return 1;
    }

    try
    {
        sqlite3 *db;
        if (sqlite3_open(argv[2], &db) != SQLITE_OK)
        {
            throw std::runtime_error("无法打开数据库: " + std::string(sqlite3_errmsg(db)));
        }

        create_table(db);

        if (!fs::is_directory(argv[1]))
        {
            throw std::runtime_error("路径不存在或不是目录: " + std::string(argv[1]));
        }
        process_directory(argv[1], db);

        char *errMsg = nullptr;
        sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &errMsg);
        if (errMsg)
        {
            std::cerr << "数据库优化警告: " << errMsg << "\n";
            sqlite3_free(errMsg);
        }

        sqlite3_close(db);
        std::cout << "成功创建数据库: " << argv[2] << "\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}

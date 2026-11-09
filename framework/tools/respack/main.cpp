#include "core/tools/assets/AssetPack.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printUsage(const char *program)
{
    std::cerr << "Usage:\n"
              << "  " << program << " pack <input-dir> <output.pak> [--exclude <glob>]...\n"
              << "  " << program << " list <input.pak>\n"
              << "  " << program << " verify <input.pak>\n";
}

int fail(const std::string &message)
{
    std::cerr << "respack: " << message << "\n";
    return 1;
}

} // namespace

int main(int argc, char *argv[])
{
    using namespace lvglpp::tools;

    if (argc < 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    const std::string command = argv[1];
    std::string       error;

    if (command == "pack")
    {
        if (argc < 4)
        {
            printUsage(argv[0]);
            return 1;
        }

        const std::string inputDirectory = argv[2];
        const std::string outputPack     = argv[3];

        std::vector<std::string> excludes = pack::defaultExcludes();
        for (int i = 4; i < argc; ++i)
        {
            const std::string option = argv[i];
            if (option == "--exclude" && i + 1 < argc)
            {
                excludes.push_back(argv[++i]);
            } else
            {
                return fail("unknown option: " + option);
            }
        }

        pack::Writer writer;
        if (!writer.addDirectory(inputDirectory, excludes, &error))
        {
            return fail(error);
        }
        if (!writer.writeFile(outputPack, &error))
        {
            return fail(error);
        }

        std::cout << "packed " << writer.entryCount() << " file(s) into " << outputPack << "\n";
        return 0;
    }

    if (command == "list" || command == "verify")
    {
        pack::Reader reader;
        if (!reader.openFile(argv[2], &error))
        {
            return fail(error);
        }

        std::uint64_t total = 0;
        for (const auto &entry : reader.entries())
        {
            total += entry.size;
            std::cout << entry.size << "\t" << entry.path << "\n";
        }

        if (command == "verify")
        {
            if (!reader.verifyAll(&error))
            {
                return fail(error);
            }
            std::cout << "verified " << reader.entries().size() << " file(s), " << total << " byte(s)\n";
        }
        return 0;
    }

    printUsage(argv[0]);
    return 1;
}

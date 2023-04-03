#include "ArgumentParser.h"
#include <regex>
#include <iostream>

uintmax_t SizeQueryParser::parse(int argc, char *argv[]) {
    int index = 1;
    bool print_exact_byte_size = false;
    bool count_only_directories = true;

    for (; index < argc; index++) {
        if (argv[index] == nullptr) {
            std::cerr << "Got broken argument" << std::endl;
            std::terminate();
        }

        bool parsed_all_flags = false;
        auto argument = std::string(argv[index]);
        switch (argument[1]) {
            case 'b':
                print_exact_byte_size = true;
                break;
            case 'a':
                count_only_directories = false;
                break;
            case 'c':
                m_printTotalCount = true;
                break;
            case 's':
                m_printOnlyTotalCount = true;
                break;
            case '-':
                processFileFlag(argument);
                break;
            default:
                parsed_all_flags = true;
        }
        if (parsed_all_flags) {
            break;
        }
    }

    if (index < argc && m_countInFile) {
        std::cerr << "Can't read in file and in directory an the same time" << std::endl;
        std::terminate();
    }

    std::uintmax_t total_size = 0;
    m_sizeCounter = std::make_unique<SizeCounter>(count_only_directories, print_exact_byte_size);

    bool got_path_args = false;
    for (; index < argc; index++) {
        if (argv[index] == nullptr) {
            std::cerr << "Got broken argument" << std::endl;
            std::terminate();
        }
        got_path_args = true;
        total_size += m_sizeCounter->countFileName(std::string(argv[index]));
    }

    if (m_filesFromFlagValue.has_value() && !got_path_args) {
        total_size += m_sizeCounter->countFromFile(m_filesFromFlagValue.value());
    }

    if (m_printOnlyTotalCount && count_only_directories) {
        std::cerr << "Flags -a and -s are incompatible" << std::endl;
        std::terminate();
    }
    return total_size;
}

void SizeQueryParser::processFileFlag(const std::string &argument) {
    if (argument.substr(0, 13) != "--files-from=" || argument.size() == 13) {
        std::cerr << "Unknown argument: " << argument << std::endl;
        std::terminate();
    }
    m_countInFile = true;
    m_filesFromFlagValue = argument.substr(13);
}

bool SizeQueryParser::printTotalCount() const noexcept {
    return m_printTotalCount;
}

bool SizeQueryParser::printOnlyTotalCount() const noexcept {
    return m_printOnlyTotalCount;
}

std::shared_ptr<std::map<std::string, uintmax_t>> SizeQueryParser::getFilesToSizeMap() const noexcept {
    return m_sizeCounter->getSizeMap();
}

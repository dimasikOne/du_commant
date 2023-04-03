#include "SizeCounter.h"
#include <filesystem>
#include <memory>
#include <regex>
#include <iostream>
#include <fstream>

uintmax_t SizeCounter::toFormat(std::uintmax_t byte_size) const {
    if (m_printExactSize) {
        return byte_size;
    }
    auto size_in_blocks = byte_size / 512;
    if (byte_size % 512 != 0 || byte_size == 0) {
        size_in_blocks++;
    }
    return size_in_blocks;
}

uintmax_t SizeCounter::countFileNameWithMask(const std::string &string_path_with_mask) {
    auto mask_start = string_path_with_mask.find('*');
    std::uintmax_t total_size = 0;
    if (mask_start == std::string::npos) {
        std::cerr << "Expected path with mask while got without it: " << string_path_with_mask << std::endl;
        std::terminate();
    }

    auto base_path_end = string_path_with_mask.substr(0, mask_start).find_last_of('/');
    auto base_path_string = string_path_with_mask.substr(0, base_path_end);
    auto filesystem_base_path = std::filesystem::path(base_path_string);
    if (!exists(filesystem_base_path)) {
        std::cerr << "No such path: " << string_path_with_mask << std::endl;
        std::terminate();
    }

    std::for_each(begin(std::filesystem::directory_iterator{filesystem_base_path}),
                  end(std::filesystem::directory_iterator{filesystem_base_path}),
                  [&](const std::filesystem::path &filesystem_base_file) {
                      auto fixed_mask_path = std::regex_replace(string_path_with_mask, std::regex("\\*"), ".*");
                      if (std::regex_match(filesystem_base_file.string(), std::regex(fixed_mask_path))) {
                          total_size += countFileName(filesystem_base_file);
                      } else {
                          if (filesystem_base_file.string().substr(mask_start + 1).find('*') != std::string::npos) {
                              auto after_mask_path_start = filesystem_base_file.string().find('/', mask_start + 1);
                              auto after_mask_path = filesystem_base_file.string().substr(after_mask_path_start);
                              total_size += countFileNameWithMask(filesystem_base_file.string() + after_mask_path);
                          }
                      }
                  });
    return total_size == 0 && !m_printExactSize ? 1 : total_size;
}

bool SizeCounter::hasMask(const std::string &path) {
    return path.find('*') != std::string::npos;
}

uintmax_t SizeCounter::countFromFile(const std::string &path) {
    std::fstream stream;
    stream.open(path);
    std::string temp_filepath_string;
    uintmax_t total_size = 0;

    while (stream >> temp_filepath_string) {
        std::filesystem::path temp_filepath(temp_filepath_string);
        if (hasMask(temp_filepath_string)) {
            countFileNameWithMask(path);
        }
        if (!exists(temp_filepath)) {
            std::cerr << "No such path: " << temp_filepath_string << std::endl;
            std::terminate();
        }
        std::uintmax_t temp_file_size = 0;
        if (is_directory(temp_filepath)) {
            temp_file_size = countFileName(temp_filepath);
        } else {
            if (m_countOnlyDirectories) {
                continue;
            }
            temp_file_size = toFormat(std::filesystem::file_size(temp_filepath));
        }
        total_size += temp_file_size;
        m_filesToSize->insert_or_assign(absolute(temp_filepath), temp_file_size);
    }

    return total_size;
}

uintmax_t SizeCounter::countFileName(const std::filesystem::path &path) {
    if (hasMask(path.string())) {
        return countFileNameWithMask(path.string());
    }
    if (!exists(path)) {
        std::cerr << "No such path: " << path.string() << std::endl;
        std::terminate();
    }
    uintmax_t file_size = 0;
    if (is_directory(path)) {
        std::for_each(begin(std::filesystem::directory_iterator{path}),
                      end(std::filesystem::directory_iterator{path}), [&](const auto &directory) {
                    file_size += countFileName(directory);
                });
    } else {
        file_size = toFormat(std::filesystem::file_size(path));
        if (m_countOnlyDirectories) {
            return file_size;
        }
    }

    auto final_size = file_size == 0 && !m_printExactSize ? 1 : file_size;
    m_filesToSize->insert_or_assign(absolute(path), final_size);
    return final_size;
}

SizeCounter::SizeCounter(bool countOnlyDirectories, bool printExactFileSize) : m_countOnlyDirectories{
        countOnlyDirectories}, m_printExactSize{printExactFileSize}, m_filesToSize{
        std::make_shared<std::map<std::string, uintmax_t>>()} {}

std::shared_ptr<std::map<std::string, uintmax_t>> SizeCounter::getSizeMap() noexcept {
    return m_filesToSize;
}

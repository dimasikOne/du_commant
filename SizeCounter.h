#ifndef DU_COMMAND_SIZECOUNTER_H
#define DU_COMMAND_SIZECOUNTER_H

#include <memory>
#include <string>
#include <filesystem>
#include <map>

class SizeCounter {
public:
    SizeCounter(bool countOnlyDirectories, bool printExactSize);
    uintmax_t countFromFile(const std::string &path);
    uintmax_t countFileName(const std::filesystem::path &path);
    std::shared_ptr<std::map<std::string, uintmax_t>> getSizeMap() noexcept;

private:
    uintmax_t countFileNameWithMask(const std::string &string_path_with_mask);
    static bool hasMask(const std::string& path);
    [[nodiscard]] uintmax_t toFormat(std::uintmax_t byte_size) const;

    std::shared_ptr<std::map<std::string, uintmax_t>> m_filesToSize{nullptr};
    bool m_countOnlyDirectories = false;
    bool m_printExactSize = false;
};
#endif //DU_COMMAND_SIZECOUNTER_H

#ifndef DU_COMMAND_ARGUMENTPARSER_H
#define DU_COMMAND_ARGUMENTPARSER_H

#include "SizeCounter.h"
#include <memory>
#include <string>
#include <set>
#include <filesystem>

class SizeQueryParser {
public:
    uintmax_t parse(int argc, char *argv[]);
    [[nodiscard]] bool printTotalCount() const noexcept;
    [[nodiscard]] bool printOnlyTotalCount() const noexcept;
    [[nodiscard]] std::shared_ptr<std::map<std::string, uintmax_t>> getFilesToSizeMap() const noexcept;
private:
    void processFileFlag(const std::string& argument);

    std::unique_ptr<SizeCounter> m_sizeCounter {nullptr};
    bool m_countInFile = false;
    std::optional<std::string> m_filesFromFlagValue = std::nullopt;
    bool m_printTotalCount = false;
    bool m_printOnlyTotalCount = false;
};

#endif //DU_COMMAND_ARGUMENTPARSER_H

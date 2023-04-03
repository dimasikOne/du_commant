#include <set>
#include <iostream>
#include "ArgumentParser.h"

int main(int argc, char *argv[]) {
    SizeQueryParser parser;
    std::uintmax_t total_size = parser.parse(argc, argv);
    const auto path_to_size = parser.getFilesToSizeMap();

    std::for_each(path_to_size->begin(), path_to_size->end(),
                  [&](const std::pair<std::string, uintmax_t> &path_to_size_pair) {
                      if (!parser.printOnlyTotalCount()) {
                          std::cout << path_to_size_pair.second << '\t' << path_to_size_pair.first
                                    << std::endl;
                      }
                  });

    if (parser.printTotalCount() || parser.printOnlyTotalCount()) {
        std::cout << "total: " << total_size << std::endl;
    }

    return 0;
}

//Вопросы:
//2. поведение, если заданы флаги -a и -s одновременно - ошибка
//3. если указан флаг --files-from и кроме него даны еще пути, считать это ошибкой ввода или выводить размер по всему?

// DB関連の実装を記載する
#include <iostream>
#include <sqlite3.h>

int main() {
  sqlite3 *db;
  int exit = sqlite3_open("test.db", &db);

  if (exit) {
    std::cerr << "Error open DB " << sqlite3_errmsg(db) << std::endl;
    return (-1);
  } else {
    std::cout << "Opened Database Successfully!" << std::endl;
  }

  sqlite3_close(db);
  return 0;
}
/**
 * Classe de definição do gerenciador do banco de dados
 *
 * Otávio Goes
 * 4/11/2019
 */

#include <sqlite3.h>

#include <iostream>
#include <mutex>
#include <string>
#include <vector>


#if LOG
  #define logging(x) std::cout << x << std::endl;
#else
  #define logging(x)
#endif


class Database {
public:
  static bool open ( const char* /*filename*/ );
  static bool exec ( const char* sql );
  static std::vector< std::vector< std::string > > query ( const char* sql );
  static void                                      close ();

private:
  Database ();
  ~Database ();

private:
  // singleton
  static Database* instance;

  std::mutex mutex;

  sqlite3* db = NULL;
  bool     opened;
};

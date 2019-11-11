/**
 * Classe de definição do gerenciador do banco de dados
 *
 * Otávio Goes
 * 4/11/2019
 */

#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

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

  sqlite3* db = NULL;
  bool     opened;
};

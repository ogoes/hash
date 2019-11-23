
#include <future>
#include <iostream>
#include <string>

#include "database/database.h"

namespace models {
typedef std::vector< std::string > db_data_t;

static std::string PLAYER_CREATION =
    "PRAGMA foreign_keys = ON;"
    "CREATE TABLE IF NOT EXISTS [Player] ("
    "[id] INTEGER PRIMARY KEY,"
    "[username] VARCHAR(50) NOT NULL,"
    "[password] VARCHAR(255) NOT NULL,"
    "[wins] INTEGER NOT NULL,"
    "[tieds] INTEGER NOT NULL,"
    "[lost] INTEGER NOT NULL"
    ");";

class Player {
public:
  Player ();
  ~Player ();

  //
  static Player* from_db ( db_data_t );

  // getters
  uint        get_id ();        //
  std::string get_username ();  //
  uint        get_wins ();      //
  uint        get_tieds ();     //
  uint        get_lost ();      //

  // compare passwords in login
  bool check_password ( std::string );  //

  //
  std::string insert_into ();  //

  std::string query_builder ( const char* );  //

  std::string update ();  //

  std::string delete_from ();  //

  // setters
  Player* set_id ( uint );               //
  Player* set_username ( std::string );  //
  Player* set_password ( std::string );  //
  Player* set_wins ( uint );             //
  Player* set_tieds ( uint );            //
  Player* set_lost ( uint );             //

private:
  bool        has_id;
  uint        id;
  bool        has_username;
  std::string username;
  bool        has_password;
  std::string password;
  bool        has_wins;
  uint        wins;
  bool        has_tieds;
  uint        tieds;
  bool        has_lost;
  uint        lost;
};

typedef enum { TIED = 1, HAS_WINNER } Result;
typedef enum { PLAYER1 = 1, PLAYER2 } Winner;

static std::string GAME_CREATION =
    "PRAGMA foreign_keys = ON;"
    "CREATE TABLE IF NOT EXISTS [Game] ("
    "[id] INTEGER PRIMARY KEY,"
    "[player1] INTEGER NOT NULL,"
    "[player2] INTEGER NOT NULL,"
    "[result] INTEGER NOT NULL,"
    "[winner] INTEGER,"
    "FOREIGN KEY (player1) REFERENCES Player (id)"
    " ON DELETE CASCADE ON UPDATE CASCADE,"
    "FOREIGN KEY (player2) REFERENCES Player (id)"
    " ON DELETE CASCADE ON UPDATE CASCADE"
    ");";

class Game {
public:
  Game ();
  ~Game ();

  // from database
  static Game* from_db ( db_data_t );

  // getters
  uint    get_id ();
  Player* get_first_player ();
  Player* get_second_player ();

  Result  get_result ();
  Player* get_winner ();

  std::string insert_into ();  //

  std::string query_builder ( const char* );  //

  std::string update ();  //

  std::string delete_from ();  //

  // setters
  Game* set_id ( uint );
  Game* set_first_player ( Player* );
  Game* set_second_player ( Player* );

  Game* set_result ( Result );
  Game* set_winner ( Winner );

private:
  uint    id;
  bool    has_id;
  Player* player1;
  bool    has_player1;
  Player* player2;
  bool    has_player2;

  Result result;
  bool   has_result;
  Winner winner;
  bool   has_winner;
};
}  // namespace models

/**
 *  Codigo responsavel por gerenciar as operações relacionadas a persistencia no
 * banco de dados
 *
 * Otávio Goes
 * 4/11/2019
 */

#include "database/database.h"

#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

Database* Database::instance = nullptr;

/**
 * [Database::open description] Abre a conexão com o banco de dados,
 *                               assim como efetua a criação das tabelas caso
 * ainda não enham side criadas
 * @param  db_name [description] recebe o nome do arquivo do banco de dados
 * @return         [description] returna o status da operação (true, false)
 */
bool Database::open ( const char* db_name ) {
  if ( instance == nullptr ) instance = new Database ();
  if ( instance->opened ) return instance->opened;

  if ( sqlite3_open ( db_name, &( instance->db ) ) ) {
    delete instance;
    instance = nullptr;
    throw "Erro ao abrir o banco de dados especificado";
  }

  instance->opened = !instance->opened;

  return true;
}

/**
 * [Database::exec description] executa determinado comando no banco
 * @param  sql [description] string que será executada no banco
 * @return     [description] returna o status da operação (true, false)
 */
bool Database::exec ( const char* sql ) {
  if ( !instance ) instance = new Database ();
  if ( !instance->opened ) return !instance->opened;

  char* errMsg = 0;

  if ( sqlite3_exec ( instance->db, sql, 0, 0, &errMsg ) != SQLITE_OK ) {
    std::cout << errMsg << std::endl;
    return false;
  }
  return true;
}

/**
 * [Database::query description] efetua uma consulta no banco
 * @param  sql [description] codigo da consulta a ser efetuada
 * @return     [description] retorno da query, um vetor que armazena cada linha
 * da resposta para cada linha, as suas determinadas colunas, e para cada coluna
 * seu valor
 */
std::vector< std::vector< std::string > > Database::query ( const char* sql ) {
  if ( !instance ) instance = new Database ();

  std::vector< std::vector< std::string > > response;
  if ( !instance->opened ) return response;

  sqlite3_stmt* statement;

  if ( sqlite3_prepare_v2 ( instance->db, sql, -1, &statement, 0 ) == 0 ) {
    int colNumber = sqlite3_column_count ( statement );
    int retval;
    while ( ( retval = sqlite3_step ( statement ) ) != SQLITE_DONE ) {
      if ( retval == SQLITE_ROW ) {
        std::vector< std::string > row;
        for ( int col = 0; col < colNumber; ++col ) {
          row.push_back ( std::string (
              ( const char* )sqlite3_column_text ( statement, col ) ) );
        }
        response.push_back ( row );
      }
    }
  }

  return response;
}

/**
 * [Database::close description] fecha a conexão com banco de dados
 */
void Database::close () {
  if ( instance ) sqlite3_close ( instance->db );
}

Database::Database () {}
Database::~Database () {}

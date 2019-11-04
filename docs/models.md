
## Comunicação entre cliente e servidor fora de uma partida

  #### Player
  ```protobuf
    message Player {
      
      uint64 id = 1;
      string username = 2;
      uint32 wins = 3;
      uint32 tieds = 4;
      uint32 lost = 5;

    }
  ```


  #### Operações possíveis para um jogador
  ```protobuf
    message Operation {

      enum Request {
        RANKING = 0; // show ranking
        PLAY = 1; // play a game
        ONLINE = 2; // online users
        VIEW = 3; // view yourself information
        CHANGE = 4; // request updates in your acount
      }

      Request operation = 1;

    }
  ```

  #### Modelo de resposta para um requisição
  ```protobuf
    message ResponseStatus {
      int32 status_code = 1;
      string message = 2;
    }
  ```


  #### Login e Signin
  ```protobuf
    message LogInSignUp {
      string username = 1;
      string password = 2;

      enum Type {
        SIGNUP = 0;
        LOGIN = 1;
      }

      Type request_type = 3;
    }
  ```

  #### Resposta ao login
  ```protobuf
    message LogInSignUpResponse {
      ResponseStatus status = 1;
      Player player = 2;
    }
  ```

  #### Resposta de Ranking
  ```protobuf
    message Ranking {
      ResponseStatus status = 1;
      repeated Player players = 2;
    }
  ```

  #### Jogar
  ```protobuf
    message Play {

      enum Type {
        WAIT = 0;
        CHALLANGE = 1;
      }

      Type request_type = 1;


      Player challanded_player = 2;
    }
  ```

  #### Desafiar
  ```protobuf
    // Challange
    message ChallangeRequest {
      Player challander_player = 1;
    }
  ```

  #### Resposta ao desafio
  ```protobuf
    message ChallangeResponse {
      enum Answer {
        ACCEPT = 0;
        DENY = 1;
      }

      Answer answer = 1;
    }
  ```

  #### Players Online
  ```protobuf
    message OnlinePlayers {
      ResponseStatus status = 1;
      repeated Player pleyers = 2;
    }
  ```

  #### Gerenciamento de conta
  ```protobuf
    message Change {
      string username = 1;
      string password = 2;
    }
  ```

  #### Resposta à mudança
  ```protobuf
    message ChangesResponse {
      ResponseStatus status = 1;
    }
  ```


## Comunicação entre cliente e servidor em partida

  #### Player

  ```protobuf
    message Player {
      
      uint64 id = 1;
      string username = 2;

    }
  ```

  #### Position

  ```protobuf
    message Position {
      int32 line = 1;
      int32 column = 2;
    }
  ```

  #### Jogada


  ```protobuf
    message Hit {
      Position position = 1;
      Player player = 2;
    }
  ```

  #### Resposta a Jogada

  ```protobuf
    message Response {
      int32 status_code = 1;
      string message = 2;
    }
  ```

  #### Requisição para sair da partida

  ```protobuf
    message Exit {
      Player player = 1;
    }
  ```

  #### Alerta de saída para o outro usuário

  ```protobuf
    message ExitAlert { // send to other player
      string message = 1;
      Player player = 2;
    }
  ```

## Comunicação entre o servidor e o Banco de Dados

  #### Modelo de Responsta a requisição

  ```protobuf

    message Response {

      int32 status_code = 1;
      string message = 2;
    }
  ```

  #### Player

  ```protobuf

    message Player {
        
      uint64 id = 1;
      string username = 2;
      uint32 wins = 3;
      uint32 tieds = 4;
      uint32 lost = 5;
    }
  ```

  #### Game

  ```protobuf
    message Game {

      uint64 id = 1;
      Player player1 = 2;
      Player player2 = 3;

      enum Result {
        TIED = 0;
        HAS_WINNER = 1;
      }

      Result result = 4;

      enum Winner {
        PLAYER1 = 0;
        PLAYER2 = 1;
      }

      Winner winner = 5;
    }
  ```


  #### Operações
  
  ```protobuf
    enum Operation {

      CREATE = 0;
      RETRIEVE = 1;
      UPDATE = 2;
      DELETE = 3;

    }
  ```

  #### CRUD Player
  ```protobuf
    message PlayerOperation {

      Operation operation = 1;
      Player player_data = 2;
    }
  ```

  #### Resposta CRUD Player
  ```protobuf
    message PlayerResponse {
      
      Response status = 1;
      repeated Player player = 2;
    }
  ```


  #### CRUD Game

  ```protobuf
    message GameOperation {

      Operation operation = 1;
      Player player_data = 2;
    }

  ```
  #### Resposta CRUD Game
  ```protobuf
    message GameResponse {

      Response status = 1;
      repeated Game game = 2;
    }
  ```

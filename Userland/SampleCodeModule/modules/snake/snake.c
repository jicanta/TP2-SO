#include <snake.h>

// Prototipos
static void fillCell(int x, int y, uint32_t color);
static void drawCenteredString(char *string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color);
static void displayMenu();
static void gameOverScreen();
static void displaySidebarInfo();
static void changeDirection(uint8_t c, Snake *snake);
static void render();
static void moveSnake(Snake *snake);
static void growSnake(Snake *snake);
static void clearMap();
static void setInitialValues();
static void generateFoodPos();
static int validateFoodPosition();
static void handleCollisions();
static int checkCollisions(Snake *snake, Snake *other_snake);
static int pointEquals(Point p1, Point p2);
static void gameloop();
static void food_sound();
static void lose_sound();
static void start_game_sound();
static void clearSidebar();

static Snake snake1;
static Snake snake2;
static Food food;

// Variables del Juego
static int score1, score2;
static Bool multiplayer; // FALSE = singleplayer, TRUE = multiplayer
static uint64_t difficulty;

static uint8_t buffer[1];

uint32_t play_snake() {
  // Inicia el programa
  uint8_t initial_scale = lib_get_scale();
  lib_set_scale(1);

  clearScreen();

  displayMenu();

  gameloop();

  // Termina el programa
  clearScreen();
  lib_set_scale(initial_scale); // Restaura el scale original
  return 0;
}

static void gameloop() {
  int quit = FALSE;
  while (!quit) {
    setInitialValues();

    clearMap();
    clearSidebar();
    displaySidebarInfo();

    start_game_sound();
    while (1) {
      lib_sleep(difficulty);

      lib_read_char(buffer);

      if (buffer[0] != 0) {
        if (!multiplayer) {
          changeDirection(buffer[0], &snake1);
        } else {
          changeDirection(buffer[0], &snake1);
          changeDirection(buffer[0], &snake2);
        }
      }

      if (!multiplayer) {
        // Singleplayer
        moveSnake(&snake1);
      } else {
        // Multiplayer
        moveSnake(&snake1);
        moveSnake(&snake2); 
      }

      handleCollisions();

      if (!snake1.alive && !snake2.alive) {
        break;
      }

      render();
    }

    lose_sound();
    gameOverScreen();

    while (buffer[0] != 'r') {
      if (buffer[0] == '\n') {
        quit = TRUE;
        break;
      }
      lib_read_char(buffer);
    }
  }
}

static void render() {
  // Dibuja snake 1
  fillCell(snake1.lastTail.x, snake1.lastTail.y, BACKGROUND_COLOR);
  if (snake1.alive) {
    for (int i = 0; i < snake1.length; i++) {
      fillCell(snake1.body[i].x, snake1.body[i].y, SNAKE_1_COLOR);
    }
  } else if (!snake1.dieAnimationFired){
    snake1.dieAnimationFired = TRUE;
    lib_sleep(1000);
    for (int i = 0; i < snake1.length; i++) {
      fillCell(snake1.body[i].x, snake1.body[i].y, BACKGROUND_COLOR);
    }
  }

  // Dibuja snake 2
  fillCell(snake2.lastTail.x, snake2.lastTail.y, BACKGROUND_COLOR);
  if (multiplayer && snake2.alive) {
    for (int i = 0; i < snake2.length; i++) {
      fillCell(snake2.body[i].x, snake2.body[i].y, SNAKE_2_COLOR);
    }
  } else if (multiplayer && !snake2.dieAnimationFired) {
    snake2.dieAnimationFired = TRUE;
    lib_sleep(1000);
    for (int i = 0; i < snake2.length; i++) {
      fillCell(snake2.body[i].x, snake2.body[i].y, BACKGROUND_COLOR);
    }

  }


  // Dibuja comida
  fillCell(food.position.x, food.position.y, FOOD_COLOR);

  // Dibuja puntaje del primer jugador
  char scoreString[10];
  itoa(score1, scoreString);
  lib_draw_string("Puntaje 1: ", SCREEN_WIDTH + SIDEBAR_MARGIN, 200, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);
  lib_draw_string(scoreString, SCREEN_WIDTH + SIDEBAR_MARGIN + 8 * 13, 200, SNAKE_1_COLOR, SIDEBAR_BG_COLOR);

  if (!multiplayer) return;

  // Dibuja puntaje del segundo jugador
  itoa(score2, scoreString);
  lib_draw_string("Puntaje 2: ", SCREEN_WIDTH + SIDEBAR_MARGIN, 240, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);
  lib_draw_string(scoreString, SCREEN_WIDTH + SIDEBAR_MARGIN + 8 * 13, 240, SNAKE_2_COLOR, SIDEBAR_BG_COLOR);
}


static void setInitialValues() {
  srand(getSecMinSeed());
  score1 = score2 = 0;

  snake1.alive = TRUE;
  snake1.length = INITIAL_SNAKE_LENGTH;
  snake1.direction = INITIAL_SNAKE_DIRECTION;

  for (int i = 0; i < snake1.length; i++) {
    snake1.body[i].x = INITIAL_SNAKE_1_X;
    snake1.body[i].y = INITIAL_SNAKE_1_Y + i;
  }

  if (multiplayer) {
    snake2.length = INITIAL_SNAKE_LENGTH;
    snake2.direction = INITIAL_SNAKE_DIRECTION;
    snake2.alive = TRUE;

    for (int i = 0; i < snake2.length; i++) {
      snake2.body[i].x = INITIAL_SNAKE_2_X;
      snake2.body[i].y = INITIAL_SNAKE_2_Y + i;
    }
  }

  generateFoodPos();
}


// GAME LOGIC FUNCTIONS


static void changeDirection(uint8_t c, Snake *snake) {
  Direction newDirection;

  if (snake == &snake1) {
    switch (buffer[0]) {
      case 'w':
        newDirection = UP;
        break;
      case 'a':
        newDirection = LEFT;
        break;
      case 's':
        newDirection = DOWN;
        break;
      case 'd':
        newDirection = RIGHT;
        break;
      default:
        return;
    }
  } else {
    switch (buffer[0]) {
      case 'i':
        newDirection = UP;
        break;
      case 'j':
        newDirection = LEFT;
        break;
      case 'k':
        newDirection = DOWN;
        break;
      case 'l':
        newDirection = RIGHT;
        break;
      default:
        return;
    }
  }

  if (snake->direction % 2 == newDirection % 2) {
    return;
  }

  snake->direction = newDirection;
}

static void generateFoodPos() {
  do {
    food.position.x = rand() % (MAP_WIDTH);
    food.position.y = rand() % (MAP_HEIGHT);
  } while (validateFoodPosition());
}

static int validateFoodPosition() {
  for (int i = 0; i < snake1.length; i++) {
    if (pointEquals(snake1.body[i], food.position)) {
      return 1;
    }
  }

  if (multiplayer) {
    for (int i = 0; i < snake2.length; i++) {
      if (pointEquals(snake2.body[i], food.position)) {
        return 1;
      }
    }
  }

  return 0;
}

static void handleCollisions() {
  if (!multiplayer) {
    // Singleplayer
    if (checkCollisions(&snake1, &snake2)) {
      growSnake(&snake1);
      score1++;
      food_sound();
      generateFoodPos();
    }
  } else {
    // Multiplayer
    if (checkCollisions(&snake1, &snake2)) {
      growSnake(&snake1);
      score1++;
      food_sound();
      generateFoodPos();
    }
    if (checkCollisions(&snake2, &snake1)) {
      growSnake(&snake2);
      score2++;
      food_sound();
      generateFoodPos();
    }
  }
}

static int checkCollisions(Snake *snake, Snake *other_snake) {
  if (snake->body[0].x < 0 || snake->body[0].x >= MAP_WIDTH || snake->body[0].y < 0 || snake->body[0].y >= MAP_HEIGHT) {
    snake->alive = FALSE;
    return 0;
  }
    

  for (int i = 1; i < snake->length; i++) {
    if (pointEquals(snake->body[0], snake->body[i])) {
      snake->alive = FALSE;
      return 0;
    }
  }

  if (multiplayer && other_snake->alive) {
    for (int i = 0; i < other_snake->length; i++) {
      if (pointEquals(snake->body[0], other_snake->body[i]))  {
        snake->alive = FALSE;
        return 0;
      }
    }
  }

  if (pointEquals(snake->body[0], food.position)) 
    return 1;

  return 0;
}

static void moveSnake(Snake *snake) {
  snake->lastTail = snake->body[snake->length - 1];
  for (int i = snake->length - 1; i > 0; i--) {
    snake->body[i] = snake->body[i - 1];
  }

  switch (snake->direction) {
    case UP:
      snake->body[0].y--;
      break;
    case DOWN:
      snake->body[0].y++;
      break;
    case LEFT:
      snake->body[0].x--;
      break;
    case RIGHT:
      snake->body[0].x++;
      break;
  }
}

static void growSnake(Snake *snake) {
  snake->body[snake->length] = snake->body[snake->length - 1];
  snake->length++;
}


// HELPER FUNCTIONS


static void clearMap() {
  lib_draw_rectangle(0, 0, MAP_WIDTH * CELL_SIZE, MAP_HEIGHT * CELL_SIZE, BACKGROUND_COLOR);
}

static void clearSidebar() {
  lib_draw_rectangle(SCREEN_WIDTH, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, SIDEBAR_BG_COLOR);
}

static int pointEquals(Point p1, Point p2) {
  return p1.x == p2.x && p1.y == p2.y;
}

static void drawCenteredString(char *string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color) {
  uint64_t string_width = strlen(string) * (FONT_WIDTH + SPACE_BETWEEN) - SPACE_BETWEEN;

  uint64_t start_x = x - (string_width / 2);

  lib_draw_string(string, start_x, y, color, bg_color);
}

static void fillCell(int x, int y, uint32_t color) {
  lib_draw_rectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

static char getChar() {
  lib_read_char(buffer);
  return buffer[0];
}

static void food_sound() {
  lib_play_sound(523, 10);
  lib_play_sound(554, 30);
  lib_play_sound(880, 40);
}
static void lose_sound() {
  lib_play_sound(880, 40);
  lib_play_sound(554, 30);
  lib_play_sound(523, 10);
}
static void start_game_sound() {
  lib_play_sound(523, 10);
  lib_play_sound(554, 30);
  lib_play_sound(880, 40);
}


// DISPLAYS


static void displayMenu() {
  clearMap();
  drawCenteredString("Bienvenido a Snake!", ((MAP_WIDTH) * CELL_SIZE) / 2 , 100, TEXT_COLOR, BACKGROUND_COLOR);
  drawCenteredString("Elija su Modo de Juego (presione [1] o [2])", ((MAP_WIDTH) * CELL_SIZE) / 2, 120, TEXT_COLOR, BACKGROUND_COLOR);

  drawCenteredString("1. Singleplayer", ((MAP_WIDTH) * CELL_SIZE) / 2, 160, TEXT_COLOR, BACKGROUND_COLOR);
  drawCenteredString("2. Multiplayer", ((MAP_WIDTH) * CELL_SIZE) / 2, 180, TEXT_COLOR, BACKGROUND_COLOR);

  char c;
  while ((c = getChar()) != '1' && c != '2') {}

  multiplayer = c == '2';

  clearMap();

  drawCenteredString("Elija la Dificultad (presione [1], [2] o [3])", ((MAP_WIDTH) * CELL_SIZE) / 2, 100, TEXT_COLOR, BACKGROUND_COLOR);

  drawCenteredString("1. Easy", ((MAP_WIDTH) * CELL_SIZE) / 2, 140, TEXT_COLOR, BACKGROUND_COLOR);
  drawCenteredString("2. Medium", ((MAP_WIDTH) * CELL_SIZE) / 2, 160, TEXT_COLOR, BACKGROUND_COLOR);
  drawCenteredString("3. Hard", ((MAP_WIDTH) * CELL_SIZE) / 2, 180, TEXT_COLOR, BACKGROUND_COLOR);

  while ((c = getChar()) != '1' && c != '2' && c != '3') {}

  switch (c) {
    case '1':
      difficulty = EASY_DIFFICULTY;
      break;
    case '2':
      difficulty = MEDIUM_DIFFICULTY;
      break;
    case '3':
      difficulty = HARD_DIFFICULTY;
      break;
  }

  clearMap();

  drawCenteredString("Presione cualquier tecla para comenzar", ((MAP_WIDTH) * CELL_SIZE) / 2, 100, TEXT_COLOR, BACKGROUND_COLOR);

  while ((c = getChar()) == 0) {}
}

static void gameOverScreen() {
  drawCenteredString("Game Over!", ((MAP_WIDTH) * CELL_SIZE) / 2 , 100, TEXT_COLOR, BACKGROUND_COLOR);

  int retMsgOffset = 0;
  if (multiplayer) {
    retMsgOffset = 20;
    int whoWon = score1 > score2 ? 1 : (score2 > score1 ? 2 : 0);
    char *winnerMsg = whoWon == 1 ? "Gana el jugador 1!" : (whoWon == 2 ? "Gana el jugador 2!" : "Empate!");
    drawCenteredString(winnerMsg, ((MAP_WIDTH) * CELL_SIZE) / 2, 120, whoWon == 1 ? SNAKE_1_COLOR : whoWon == 2 ? SNAKE_2_COLOR : TEXT_COLOR, BACKGROUND_COLOR);
  }

  drawCenteredString("Presione la tecla ENTER para regresar", ((MAP_WIDTH) * CELL_SIZE) / 2, 120 + retMsgOffset, TEXT_COLOR, BACKGROUND_COLOR);
  drawCenteredString("Presione R para volver a jugar", ((MAP_WIDTH) * CELL_SIZE) / 2, 140 + retMsgOffset, TEXT_COLOR, BACKGROUND_COLOR);
}

static void displaySidebarInfo() {
  // Write game mode
  lib_draw_string("Game Mode: ", SCREEN_WIDTH + SIDEBAR_MARGIN, 20, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);
  lib_draw_string(multiplayer ? "Multiplayer" : "Singleplayer", SCREEN_WIDTH + SIDEBAR_MARGIN, 40, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);

  // Write difficulty
  lib_draw_string("Difficulty: ", SCREEN_WIDTH + SIDEBAR_MARGIN, 80, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);
  lib_draw_string(difficulty == HARD_DIFFICULTY ? "Hard" : (difficulty == MEDIUM_DIFFICULTY ? "Medium" : "Easy"), SCREEN_WIDTH + SIDEBAR_MARGIN, 100, SIDEBAR_TEXT_COLOR, SIDEBAR_BG_COLOR);
}




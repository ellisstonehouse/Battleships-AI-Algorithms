
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "game.h"
#include "playGame.h"
#include "initGame.h"
#include "aiMoves.h"

#define TESTING_RANDOM 0
#define RANDOM_SHOOTING 1
#define HUNT 2
#define HUNT_PARITY 3
#define PROB_DENSITY 4


int* aiMove(Board* board, int mode) {

  switch (mode) {

  case TESTING_RANDOM:
    return randomShooting(board);

  case RANDOM_SHOOTING:
    return randomShooting(board);
  
  case HUNT:
    return hunt(board);
  
  case HUNT_PARITY:
    return huntParity(board);
  
  case PROB_DENSITY:
    return probabilityDensity(board);
  
  default:
    return randomShooting(board);
  }
}

int* randomShooting(Board* board) {
  static int move[2];
  int x, y;

  while(true) {

    srand((unsigned int)clock()); // only use clock when testing
    // srand(time(0));

    x = rand() % board->boardSize;
    y = rand() % board->boardSize;

    if (board->grid[x][y] == '*' || board->grid[x][y] == 'x' || board->grid[x][y] == '#') {
      continue;
    }

    break;
  }

  move[0] = x;
  move[1] = y;

  return move;
}



int* hunt(Board* board) {

  static int move[2];

  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {

        if (board->grid[x][y] == '*') {
          // Above HIT
          if (0 <= x-1 && board->grid[x-1][y] != '*' && board->grid[x-1][y] != 'x' && board->grid[x-1][y] != '#') {
            move[0] = x-1; move[1] = y;
            return move;
          }
          // Below HIT
          if (x+1 < board->boardSize && board->grid[x+1][y] != '*' && board->grid[x+1][y] != 'x' && board->grid[x+1][y] != '#') {
            move[0] = x+1; move[1] = y;
            return move;
          }
          // Right of HIT
          if (y+1 < board->boardSize && board->grid[x][y+1] != '*' && board->grid[x][y+1] != 'x' && board->grid[x][y+1] != '#') {
            move[0] = x; move[1] = y+1;
            return move;
          }
          // Left of HIT
          if (0 <= y-1 && board->grid[x][y-1] != '*' && board->grid[x][y-1] != 'x' && board->grid[x][y-1] != '#') {
            move[0] = x; move[1] = y-1;
            return move;
          }
        }

    }
  }

  return randomShooting(board);
}

int* huntParity(Board* board) {

  static int move[2];

  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {

      if (board->grid[x][y] == '*') {
        // Above HIT
        if (0 <= x-1 && board->grid[x-1][y] != '*' && board->grid[x-1][y] != 'x' && board->grid[x-1][y] != '#') {
          move[0] = x-1; move[1] = y;
          return move;
        }
        // Below HIT
        if (x+1 < board->boardSize && board->grid[x+1][y] != '*' && board->grid[x+1][y] != 'x' && board->grid[x+1][y] != '#') {
          move[0] = x+1; move[1] = y;
          return move;
        }
        // Right of HIT
        if (y+1 < board->boardSize && board->grid[x][y+1] != '*' && board->grid[x][y+1] != 'x' && board->grid[x][y+1] != '#') {
          move[0] = x; move[1] = y+1;
          return move;
        }
        // Left of HIT
        if (0 <= y-1 && board->grid[x][y-1] != '*' && board->grid[x][y-1] != 'x' && board->grid[x][y-1] != '#') {
          move[0] = x; move[1] = y-1;
          return move;
        }
      }

    }
  }

  int* rMove = randomShooting(board);

  while (rMove[0]%2 == rMove[1]%2) {
    rMove = randomShooting(board);
  }

  return rMove;
}

int* probabilityDensity(Board* board) {

  //static int move[2];

  int target = false;

  // fix this selection loop
  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {
      if (board->grid[x][y] == '*') {
        target = true;
        break;
      }
    }
  }

  if (target) {
    return pD_Target(board);
  }
  else {
    return pD_Hunt(board);
  }
}

int* pD_Hunt(Board* board) {

  static int move[2];

  // allocate grid dynamically
  int** densityGrid = malloc(sizeof(int* ) * board->boardSize);
  for (int i = 0; i < board->boardSize; i++) {
    densityGrid[i] = malloc(sizeof(int ) * board->boardSize);
  }

  // set grid to 0's
  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {
      densityGrid[x][y] = 0;
    }
  }


  for (int i=0; i<board->fleetSize; i++) {

    Ship* ship = board->fleet[i];

    // only search for ships still afloat
    if (!ship->afloat) {
      continue;
    }

    for (int x = 0; x < board->boardSize; x++) {
      for (int y = 0; y < board->boardSize; y++) {

        int x_possible = true;
        int y_possible = true;

        for (int k = 0; k < ship->size; k++) {
          if (board->boardSize < x+ship->size || board->grid[x+k][y] == '*' || board->grid[x+k][y] == 'x' || board->grid[x+k][y] == '#') {
            x_possible = false;
            break;
          }
        }
        for (int k = 0; k < ship->size; k++) {
          if (board->boardSize < y+ship->size || board->grid[x][y+k] == '*' || board->grid[x][y+k] == 'x' || board->grid[x][y+k] == '#') {
            y_possible = false;
            break;
          }
        }

        if (x_possible) {
          for (int k = 0; k < ship->size; k++) {
            densityGrid[x+k][y]++;
          }
        }
        if (y_possible) {
          for (int k = 0; k < ship->size; k++) {
            densityGrid[x][y+k]++;
          }
        }
      }
    }
  }


  int max = 0;

  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {
      if (max < densityGrid[x][y]) {
        max = densityGrid[x][y];
        move[0] = x;
        move[1] = y;
      }
      printf("%d ", densityGrid[x][y]);
    }
    printf("\n");
  }

  
  for (int i = 0; i < board->boardSize; i++) {
    free(densityGrid[i]);
  }
  free(densityGrid);

  return move;
}

int* pD_Target(Board* board) {

  static int move[2];

  // allocate grid dynamically
  int** densityGrid = malloc(sizeof(int* ) * board->boardSize);
  for (int i = 0; i < board->boardSize; i++) {
    densityGrid[i] = malloc(sizeof(int ) * board->boardSize);
  }

  // set grid to 0's
  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {
      densityGrid[x][y] = 0;
    }
  }

  for (int i=0; i<board->fleetSize; i++) {

    Ship* ship = board->fleet[i];

    // only search for ships still afloat
    if (!ship->afloat) {
      continue;
    }

    for (int x = 0; x < board->boardSize; x++) {
      for (int y = 0; y < board->boardSize; y++) {

        int x_possible = false;
        int y_possible = false;

        for (int k = 0; k < ship->size; k++) {
          if (board->grid[x+k][y] == '*') {
            x_possible = true;
          }
          if (board->boardSize < x+ship->size || board->grid[x+k][y] == 'x' || board->grid[x+k][y] == '#') {
            x_possible = false;
            break;
          }
        }
        for (int k = 0; k < ship->size; k++) {
          if (board->grid[x][y+k] == '*') {
            y_possible = true;
          }
          if (board->boardSize < y+ship->size || board->grid[x][y+k] == 'x' || board->grid[x][y+k] == '#') {
            y_possible = false;
            break;
          }
        }


        if (x_possible) {
          for (int k = 0; k < ship->size; k++) {
            if (board->grid[x+k][y] != '*') {
              densityGrid[x+k][y]++;
            }
          }
        }
        if (y_possible) {
          for (int k = 0; k < ship->size; k++) {
            if (board->grid[x][y+k] != '*') {
              densityGrid[x][y+k]++;
            }
          }
        }
      }
    }
  }

  int max = 0;

  for (int x = 0; x < board->boardSize; x++) {
    for (int y = 0; y < board->boardSize; y++) {
      if (max < densityGrid[x][y]) {
        max = densityGrid[x][y];
        move[0] = x;
        move[1] = y;
      }
      printf("%d ", densityGrid[x][y]);
    }
    printf("\n");
  }

  return move;
}
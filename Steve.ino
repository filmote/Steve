#include <Arduboy2.h>
#include <EEPROM.h>
#include "Images.h"

#define NUMBER_OF_OBSTACLES 5
#define GROUND_LEVEL 32
#define JUMP_TOP_HEIGHT 10

#define EEPROM_START_C1                 200
#define EEPROM_START_C2                 EEPROM_START_C1 + 1
#define EEPROM_SCORE                    EEPROM_START_C1 + 2

enum GameStatus {
  Introduction,
  PlayGame,
  GameOver,
};

enum Stance {
  Standing,
  Running1,
  Running2,
  Ducking1,
  Ducking2,
  Dead,
};

enum ObstacleType {
  Pterodactyl1,
  Pterodactyl2,
  SingleCactus,
  DoubleCactus,
  TripleCactus
};

struct Dinosaur {
  char x;
  char y;
  Stance stance;
  byte jumping;
  byte goingUp;
};

struct Obstacle {
  char x;
  char y;
  ObstacleType type;
  bool enabled;
  
};

Arduboy2 arduboy;
int frame = 0;

Obstacle obstacles[5] = {
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
};

Dinosaur steve = {0, GROUND_LEVEL, Standing, false, false};

unsigned int score = 0;
unsigned int highScore = 0;

GameStatus gameStatus = Introduction;

void setup() {

  initEEPROM();
  arduboy.begin();
  arduboy.setFrameRate(60);

}

void loop() {

  // Pause here until it's time for the next frame ..
  
  if (!(arduboy.nextFrame()))
    return;

  switch (gameStatus) {

    case GameStatus::Introduction:
      introduction();
      break;

    case GameStatus::PlayGame:
      playGame();
      break;

    case GameStatus::GameOver:
      gameOver();
      break;
      
  }
    
}

void introduction() {

  highScore = EEPROMReadInt(EEPROM_SCORE);
  arduboy.clear();

  steve.x = 0;
  steve.y = GROUND_LEVEL;
  steve.jumping = false;
  steve.stance = Stance::Dead;

  score = 0;

  arduboy.setCursor(44, 10);
  arduboy.print(F("Press A to begin"));
  arduboy.drawLine(20, 107, 7, WHITE);
  arduboy.drawLine(20, 107, 19, WHITE);

  drawSteve();
  drawScoreboard(false);
  arduboy.display();
    
  if (arduboy.justPressed(A_BUTTON))                            { gameStatus = GameStatus::PlayGame; }
  
}

void gameOver() {

  if (score > highScore) {
    EEPROMWriteInt(EEPROM_SCORE, score);
    highScore = score;
  }

  arduboy.clear();

  arduboy.setCursor(44, 10);
  arduboy.print(F("Game Over"));
  arduboy.drawLine(20, 107, 7, WHITE);
  arduboy.drawLine(20, 107, 19, WHITE);

  drawSteve();
  drawScoreboard(true);
  arduboy.display();
    
  if (arduboy.justPressed(A_BUTTON))                            { gameStatus = GameStatus::PlayGame; }
 
}

void playGame() {

  arduboy.clear();
  
  if (steve.y == GROUND_LEVEL) {

    if (arduboy.justPressed(A_BUTTON))                          { steve.jumping = true; steve.goingUp = true; }
    if (arduboy.justPressed(B_BUTTON))                          { steve.stance = Stance::Ducking1; }
    if (arduboy.justPressed(LEFT_BUTTON) && steve.x > 0)        { steve.x--; }
    if (arduboy.justPressed(RIGHT_BUTTON) && steve.x < 32)      { steve.x++; }
    
  }

  // Has Steve collided with anything?

  if (collision()) {

    steve.stance = Stance::Dead;
    gameStatus = GameStatus::GameOver;

  }
  else {
    
    updateSteve();
    updateObstacles();
  
    drawSteve();
    drawObstacles();
    drawScoreboard(true);

    arduboy.display();
    
  }
  
}

byte getImageWidth(const byte *image) {
  return pgm_read_byte(image);
}

byte getImageHeight(const byte *image) {
  return pgm_read_byte(image + 1);
}

bool collision () {
    
  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    Obstacle thisObstacle = obstacles[i];

    if (thisObstacle.enabled == true) {

      if (arduboy.collide(getSteveRect(), getObstacleRect(i))) {
        
        return true;
          
      }
      
    }
    
  }

  return false;
 
}

void updateSteve() {

  // Is Steve jumping ?

  if (steve.jumping) {

    if (steve.goingUp) {
    
      steve.y--;
      if (steve.y == JUMP_TOP_HEIGHT) {
        steve.goingUp = false;
      }
    
    }
    else {
    
      steve.y++;
      if (steve.y == GROUND_LEVEL) {
        steve.jumping = false;
      }

    }
    
  }


  // Swap the image to simulate running ..

  switch (steve.stance) {

    case Stance::Running1:
      steve.stance = Stance::Running2;
      break;
    
    case Stance::Running2:
      steve.stance = Stance::Running1;
      break;
    
    case Stance::Ducking1:
      steve.stance = Stance::Ducking1;
      break;
    
    case Stance::Ducking2:
      steve.stance = Stance::Ducking2;
      break;

    default:
      break;
    
   }

}

void drawSteve() {

  switch (steve.stance) {

    case Stance::Standing:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_still), dinosaur_still, dinosaur_still_mask, frame, frame);
      break;

    case Stance::Running1:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_running_1), dinosaur_running_1, dinosaur_running_1_mask, frame, frame);
      break;

    case Stance::Running2:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_running_2), dinosaur_running_2, dinosaur_running_2_mask, frame, frame);
      break;

    case Stance::Ducking1:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_ducking_1), dinosaur_ducking_1, dinosaur_ducking_1_mask, frame, frame);
      break;

    case Stance::Ducking2:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_ducking_2), dinosaur_ducking_2, dinosaur_ducking_2_mask, frame, frame);
      break;

    case Stance::Dead:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_dead), dinosaur_dead, dinosaur_still_mask, frame, frame);
      break;
       
  }
  
}

Rect getSteveRect() {

  switch (steve.stance) {

    case Stance::Standing:
    case Stance::Running1:
    case Stance::Running2:
    case Stance::Dead:
      return Rect { steve.x, steve.y - getImageHeight(dinosaur_still), getImageWidth(dinosaur_still), getImageHeight(dinosaur_still) };

    case Stance::Ducking1:
    case Stance::Ducking2:
      return Rect { steve.x, steve.y - getImageHeight(dinosaur_ducking_2), getImageWidth(dinosaur_ducking_2), getImageHeight(dinosaur_ducking_2) };
       
  }
  
}

void updateObstacles() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    Obstacle thisObstacle = obstacles[i];
    
    if (thisObstacle.enabled == true) {

      switch (thisObstacle.type) {

        case ObstacleType::Pterodactyl1:
          thisObstacle.type = Pterodactyl2;
          thisObstacle.x--;
          if (thisObstacle.x < 0) {
            thisObstacle.enabled = false; 
          }
          break;

        case ObstacleType::Pterodactyl2:
          thisObstacle.type = Pterodactyl1;
          thisObstacle.x--;
          if (thisObstacle.x < 0) {
            thisObstacle.enabled = false; 
          }
          break;

        case ObstacleType::SingleCactus:
        case ObstacleType::DoubleCactus:
        case ObstacleType::TripleCactus:
          if (arduboy.everyXFrames(2)) {
            thisObstacle.x--;
            if (thisObstacle.x < 0) {
              thisObstacle.enabled = false; 
            }
          }
          break;

      }
      
    }
    
  }
  
}

void drawObstacles() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    Obstacle thisObstacle = obstacles[i];
    
    if (thisObstacle.enabled == true) {

      switch (thisObstacle.type) {

        case ObstacleType::Pterodactyl1:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, pterodactyl_1, frame);
          break;

        case ObstacleType::Pterodactyl2:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, pterodactyl_2, frame);
          break;

        case ObstacleType::SingleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_1, frame);
          break;

        case ObstacleType::DoubleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_2, frame);
          break;

        case ObstacleType::TripleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_3, frame);
          break;

      }
      
    }
    
  }
  
}

Rect getObstacleRect(byte index) {

  Obstacle thisObstacle = obstacles[index];
    
  switch (thisObstacle.type) {

    case ObstacleType::Pterodactyl1:
    case ObstacleType::Pterodactyl2:
      return Rect { thisObstacle.x, thisObstacle.y, getImageWidth(pterodactyl_1), getImageHeight(pterodactyl_1) };

    case ObstacleType::SingleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, getImageWidth(cactus_1), getImageHeight(cactus_1) };

    case ObstacleType::DoubleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, getImageWidth(cactus_2), getImageHeight(cactus_2) };

    case ObstacleType::TripleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, getImageWidth(cactus_3), getImageHeight(cactus_3) };

  }

}

void drawScoreboard(bool displayCurrentScore) {

  if (displayCurrentScore) { 
    
    arduboy.setCursor(0, 0);
    arduboy.print(F("Score: "));
    if (score < 1000) arduboy.print("0");
    if (score < 100) arduboy.print("0");
    if (score < 10)  arduboy.print("0");
    arduboy.print(score);
       
  }
  
  arduboy.setCursor(44, 0);
  arduboy.print(F("High Score: "));
  if (highScore < 1000) arduboy.print("0");
  if (highScore < 100) arduboy.print("0");
  if (highScore < 10)  arduboy.print("0");
  arduboy.print(highScore);
  
  arduboy.drawLine(0, WIDTH, 11, WHITE);

}

/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised? 
 *   
 *   Looks for the characters 'S' and 'T' in the first two bytes of the EEPROM
 *   memory range starting from byte EEPROM_STORAGE_SPACE_START.  If not found,
 *   it resets the settings ..
 * ----------------------------------------------------------------------------
 */
void initEEPROM() {

  uint8_t c1 = EEPROM.read(EEPROM_START_C1);
  uint8_t c2 = EEPROM.read(EEPROM_START_C2);

  if (c1 != 83 || c2 != 84) { 
  
    EEPROM.update(EEPROM_START_C1, 83);
    EEPROM.update(EEPROM_START_C2, 84);
    EEPROMWriteInt(EEPROM_SCORE, 0);
      
  }

}


/* ----------------------------------------------------------------------------
 * Write a 2 byte integer to the EEPROM at the specified address ..
 * ----------------------------------------------------------------------------
 */
void EEPROMWriteInt(int address, int value) {
  
  uint8_t lowByte = ((value >> 0) & 0xFF);
  uint8_t highByte = ((value >> 8) & 0xFF);
  
  EEPROM.write(address, lowByte);
  EEPROM.write(address + 1, highByte);

}


/* ----------------------------------------------------------------------------
 * Read a 2 byte integer from the EEPROM at the specified address ..
 * ----------------------------------------------------------------------------
 */
uint16_t EEPROMReadInt(int address) {
  
  uint8_t lowByte = EEPROM.read(address);
  uint8_t highByte = EEPROM.read(address + 1);
  
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);

}

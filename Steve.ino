#include <Arduboy2.h>
#include "Images.h"

#define NUMBER_OF_OBSTACLES 5
#define GROUND_LEVEL 32
#define JUMP_TOP_HEOGHT 10

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
  byte x;
  byte y;
  Stance stance;
  byte goingUp;
};

struct Obstacle {
  byte x;
  byte y;
  ObstacleType type;
  bool enabled;
  
};

Arduboy2 arduboy;
int frame = 0;

Obstacle obstacles[5] = {
  { 0, 0, Pterodactyl1, false },
  { 0, 0, Pterodactyl1, false },
  { 0, 0, Pterodactyl1, false },
  { 0, 0, Pterodactyl1, false },
  { 0, 0, Pterodactyl1, false },
};

Dinosaur steve = {0, GROUND_LEVEL, Standing, false};

unsigned int score = 0;

void setup() {

  arduboy.begin();
  arduboy.setFrameRate(60);

}

void loop() {

  // Pause here until it's time for the next frame ..
  
  if (!(arduboy.nextFrame()))
    return;

  if (steve.y == GROUND_LEVEL) {

    if (arduboy.justPressed(UP_BUTTON))       { steve.goingUp = true; }
    if (arduboy.justPressed(DOWN_BUTTON))     { steve.stance = Ducking1; }
    
  }

  updateSteve();
  updateObstacles();

  drawSteve();
  drawObstacles();
  
}

void updateSteve() {

  switch (steve.stance) {

    case Running1:
      steve.stance = Running2;
      break;
    
    case Running2:
      steve.stance = Running1;
      break;
    
    case Ducking1:
      steve.stance = Ducking1;
      break;
    
    case Ducking2:
      steve.stance = Ducking2;
      break;
    
   }

}

void drawSteve() {

  switch (steve.stance) {

    case Standing:
      Sprites::drawExternalMask(steve.x, steve.y - 21, dinosaur_still, dinosaur_still_mask, frame, frame);
      break;

    case Running1:
      Sprites::drawExternalMask(steve.x, steve.y - 21, dinosaur_running_1, dinosaur_running_1_mask, frame, frame);
      break;

    case Running2:
      Sprites::drawExternalMask(steve.x, steve.y - 21, dinosaur_running_2, dinosaur_running_2_mask, frame, frame);
      break;

    case Ducking1:
      Sprites::drawExternalMask(steve.x, steve.y - 13, dinosaur_ducking_1, dinosaur_ducking_1_mask, frame, frame);
      break;

    case Ducking2:
      Sprites::drawExternalMask(steve.x, steve.y - 13, dinosaur_ducking_2, dinosaur_ducking_2_mask, frame, frame);
      break;

    case Dead:
      Sprites::drawExternalMask(steve.x, steve.y - 21, dinosaur_dead, dinosaur_still_mask, frame, frame);
      break;
       
  }
  
}

Rect getSteveRect() {

  switch (steve.stance) {

    case Standing:
    case Running1:
    case Running2:
    case Dead:
      return Rect { steve.x, steve.y - 21, 18, 24 };

    case Ducking1:
    case Ducking2:
      return Rect { steve.x, steve.y - 21, 26, 16 };
       
  }
  
}

void updateObstacles() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    Obstacle thisObstacle = obstacles[i];
    
    if (thisObstacle.enabled == true) {

      switch (thisObstacle.type) {

        case Pterodactyl1:
          thisObstacle.type = Pterodactyl2;
          thisObstacle.x--;
          if (thisObstacle.x < 0) {
            thisObstacle.enabled = false; 
          }
          break;

        case Pterodactyl2:
          thisObstacle.type = Pterodactyl1;
          thisObstacle.x--;
          if (thisObstacle.x < 0) {
            thisObstacle.enabled = false; 
          }
          break;

        case SingleCactus:
        case DoubleCactus:
        case TripleCactus:
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

        case Pterodactyl1:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, pterodactyl_1, frame);
          break;

        case Pterodactyl2:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, pterodactyl_2, frame);
          break;

        case SingleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_1, frame);
          break;

        case DoubleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_2, frame);
          break;

        case TripleCactus:
          Sprites::drawOverwrite(thisObstacle.x, thisObstacle.y, cactus_3, frame);
          break;

      }
      
    }
    
  }
  
}

Rect getObstacleRect(byte index) {

  Obstacle thisObstacle = obstacles[index];
    
  switch (thisObstacle.type) {

    case Pterodactyl1:
    case Pterodactyl2:
      return Rect { thisObstacle.x, thisObstacle.y, 22, 16 };

    case SingleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, 11, 24 };

    case DoubleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, 23, 24 };

    case TripleCactus:
      return Rect { thisObstacle.x, thisObstacle.y, 35, 24 };

  }

}


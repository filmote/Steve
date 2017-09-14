#include <Arduboy2.h>
#include "Images.h"

enum Stance {
  Standing,
  Running1,
  Running2,
  Ducking1,
  Ducking2,
  Dead,
};

enum ObstacleType {
  Pterodactyl,
  SingleCactus,
  DoubleCactus,
  TrippleCactus
};

struct Dinosaur {
  byte x;
  byte y;
  Stance stance;
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
  { 0, 0, Pterodactyl, false },
  { 0, 0, Pterodactyl, false },
  { 0, 0, Pterodactyl, false },
  { 0, 0, Pterodactyl, false },
  { 0, 0, Pterodactyl, false },
};

Dinosaur steve = {0, 0, Standing};

unsigned int score = 0;

void setup() {

  arduboy.begin();
  arduboy.setFrameRate(60);

}

void loop() {

  // Pause here until it's time for the next frame ..
  
  if (!(arduboy.nextFrame()))
    return;



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


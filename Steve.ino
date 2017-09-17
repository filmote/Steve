#include <Arduboy2.h>
#include <EEPROM.h>
#include "EEPROMUtils.h"
#include "Images.h"

#define NUMBER_OF_OBSTACLES         5     
#define GROUND_LEVEL                32
#define JUMP_TOP_HEIGHT             10
#define SCORE_START_CACTUS          300
#define PTERODACTYL_UPPER_LIMIT     8
#define PTERODACTYL_LOWER_LIMIT     24
#define OBSTACLE_LAUNCH_DELAY_MIN   125
#define OBSTACLE_LAUNCH_DELAY_MAX   300

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
  Dead1,
  Dead2,
};

enum ObstacleType {
  SingleCactus,
  DoubleCactus,
  TripleCactus,
  Pterodactyl1,
  Pterodactyl2,
  Count_CactusOnly = 3,
  Count_AllObstacles = 4,
};

enum GroundType {
  Flat,
  Bump,
  Hole,
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
int groundX = 0;

Obstacle obstacles[5] = {
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
  { 0, 0, ObstacleType::Pterodactyl1, false },
};

GroundType ground[5] = {
  GroundType::Flat,
  GroundType::Flat,
  GroundType::Hole,
  GroundType::Flat,
  GroundType::Flat,
};

Dinosaur steve = {0, GROUND_LEVEL, Standing, false, false};

unsigned int score = 0;
unsigned int highScore = 0;
unsigned int obstacleLaunchCountdown = OBSTACLE_LAUNCH_DELAY_MIN;

GameStatus gameStatus = Introduction;


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Setup the environment ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void setup() {

  initEEPROM();
  arduboy.begin();
  arduboy.setFrameRate(60);

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Control the various states of the game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {

  // Pause here until it's time for the next frame ..
  
  if (!(arduboy.nextFrame())) return;

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


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Reset everything ready for a new game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void initialiseGame() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    obstacles[i].enabled = false;
  }

  score = 0;
  steve.x = 0;
  steve.y = GROUND_LEVEL;
  steve.jumping = false;
  steve.stance = Stance::Standing;

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the introduction ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void introduction() {

  highScore = EEPROMReadInt(EEPROM_SCORE);
  arduboy.clear();

  initialiseGame();

  arduboy.setCursor(44, 10);
  arduboy.print(F("Press A to begin"));
  arduboy.drawLine(20, 107, 7, WHITE);
  arduboy.drawLine(20, 107, 19, WHITE);

  drawGround();
  drawSteve();
  drawScoreboard(false);
  arduboy.display();
    
  if (arduboy.justPressed(A_BUTTON)) {
    
    gameStatus = GameStatus::PlayGame; 
  
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the 'Game Over' text if Steve has hit an obstacle ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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
  
  drawGround();
  drawSteve();
  drawObstacles();
  drawScoreboard(true);


  // Update Steve's image.  If he is dead and still standing, this will change him to lying on the ground ..

  updateSteve();

  arduboy.display();
    
  if (arduboy.justPressed(A_BUTTON)) { 
  
    initialiseGame();
    gameStatus = GameStatus::PlayGame; 
  
  }
 
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Run Steve, run.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void playGame() {

  arduboy.clear();
  

  // The player can only control Steve if he is running or ducking on the ground ..

  if (steve.y == GROUND_LEVEL) {

    if (arduboy.justPressed(A_BUTTON))                          { steve.jumping = true; steve.goingUp = true; }
    if (arduboy.justPressed(B_BUTTON))                          { steve.stance = Stance::Ducking1; }
    if (arduboy.justPressed(LEFT_BUTTON) && steve.x > 0)        { steve.x--; }
    if (arduboy.justPressed(RIGHT_BUTTON) && steve.x < 32)      { steve.x++; }


    // If the player has not pressed the B button (or continued to hold it down) 
    // and Steve is ducking, then return him to an upright position ..

    if (arduboy.notPressed(B_BUTTON) && (steve.stance == Stance::Ducking1 || steve.stance == Stance::Ducking2)) {
      steve.stance = Stance::Running1;
    }

  }


  // Should we launch another obstacle?
  
  --obstacleLaunchCountdown;
  
  if (obstacleLaunchCountdown == 0) {

    for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {
      
      Obstacle thisObstacle = obstacles[i];

      if (!thisObstacle.enabled) { 
        launchObstacle(i); 
        break;
      }

    }

    obstacleLaunchCountdown = random(OBSTACLE_LAUNCH_DELAY_MIN, OBSTACLE_LAUNCH_DELAY_MAX);
            
  }

    
  // Has Steve collided with anything?

  if (collision()) {

    if (steve.stance <= Stance::Running2) {
      steve.stance = Stance::Dead1;
    }
    else {
      steve.stance = Stance::Dead2;
    }
    gameStatus = GameStatus::GameOver;

  }
  else {
    

    // if not, move Steve and any visible obstacles and continue play ..

    updateSteve();
    updateObstacles();
  
    drawGround();
    drawSteve();
    drawObstacles();
    drawScoreboard(true);

    arduboy.display();
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Has Steve collided with any visible obstacle ?
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Update Steve's position and stance ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void updateSteve() {


  // Is Steve jumping ?

  if (steve.jumping) {

    if (steve.goingUp) {


      // Steve is on his way up.  If he has jumped has high as he can go, then he must come down ..

      steve.y--;
      if (steve.y == JUMP_TOP_HEIGHT) {
        steve.goingUp = false;
      }
    
    }
    else {
    

      // Steve is returning to earth, if he hits the ground then he is not jumping anymore ..

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
    
    case Stance::Dead1:
      steve.stance = Stance::Dead2;
      break;

    default:
      break;
    
   }

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render Steve.  
 * 
 *  The standing and ducking images are rendered relative to the ground, so the image height is subtracted from the current Y
 *  position to determine an upper top position.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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

    case Stance::Dead1:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_dead_1), dinosaur_dead_1, dinosaur_still_mask, frame, frame);
      break;

    case Stance::Dead2:
      Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(dinosaur_dead_2), dinosaur_dead_2, dinosaur_dead_2_mask, frame, frame);
      break;
       
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Return the rectangle that Steve occupies.  This differs depending on whether he is standing or ducking ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
Rect getSteveRect() {

  switch (steve.stance) {

    case Stance::Standing:
    case Stance::Running1:
    case Stance::Running2:
    case Stance::Dead1:
      return Rect { steve.x, steve.y - getImageHeight(dinosaur_still), getImageWidth(dinosaur_still), getImageHeight(dinosaur_still) };

    case Stance::Ducking1:
    case Stance::Ducking2:
    case Stance::Dead2:
      return Rect { steve.x, steve.y - getImageHeight(dinosaur_ducking_2), getImageWidth(dinosaur_ducking_2), getImageHeight(dinosaur_ducking_2) };
             
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Update the position of any visible obstacles ..
 *
 *  If the obstacle has completely off screen, then disable it.  Pterodactyls move 1 pixel per update whereas cacti move one
 *  pixel every second iteration.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void updateObstacles() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    Obstacle thisObstacle = obstacles[i];
    
    if (thisObstacle.enabled == true) {

      switch (thisObstacle.type) {

        case ObstacleType::Pterodactyl1:

          thisObstacle.type = Pterodactyl2;
          thisObstacle.x--;

          if (thisObstacle.x < -getImageWidth(pterodactyl_1)) {
            thisObstacle.enabled = false; 
          }

          break;

        case ObstacleType::Pterodactyl2:

          thisObstacle.type = Pterodactyl1;
          thisObstacle.x--;

          if (thisObstacle.x < -getImageWidth(pterodactyl_2)) {
            thisObstacle.enabled = false; 
          }

          break;

        case ObstacleType::SingleCactus:
        
          if (arduboy.everyXFrames(2)) {

            thisObstacle.x--;
            if (thisObstacle.x < -getImageWidth(cactus_1)) {
              thisObstacle.enabled = false; 
            }

          }
          break;

        case ObstacleType::DoubleCactus:
            
          if (arduboy.everyXFrames(2)) {

            thisObstacle.x--;
            if (thisObstacle.x < -getImageWidth(cactus_2)) {
              thisObstacle.enabled = false; 
            }

          }
          break;

        case ObstacleType::TripleCactus:

          if (arduboy.everyXFrames(2)) {

            thisObstacle.x--;
            if (thisObstacle.x < -getImageWidth(cactus_3)) {
              thisObstacle.enabled = false; 
            }

          }
          break;

      }
      
    }
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render any visible obstacles on the screen ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Return the rectangle that an obstacle occupies.  Each obstacle has a different size ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render the scoreboard at the top of the screen ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
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


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Launch a new obstacle ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void launchObstacle(byte obstacleNumber) {

  ObstacleType type;

  if (score < SCORE_START_CACTUS) {
    type = (ObstacleType)random(ObstacleType::SingleCactus, ObstacleType::Count_CactusOnly);
  }
  else {
    type = (ObstacleType)random(ObstacleType::SingleCactus, ObstacleType::Count_AllObstacles);
  }
 
  switch (type) {

    case ObstacleType::SingleCactus:
    case ObstacleType::DoubleCactus:
    case ObstacleType::TripleCactus:
      obstacles[obstacleNumber].type = type;
      obstacles[obstacleNumber].enabled = true;
      obstacles[obstacleNumber].x = WIDTH - 1;
      obstacles[obstacleNumber].y = GROUND_LEVEL;
      break;

    case ObstacleType::Pterodactyl1:  
      obstacles[obstacleNumber].type = ObstacleType::Pterodactyl1;
      obstacles[obstacleNumber].enabled = true;
      obstacles[obstacleNumber].x = WIDTH - 1;
      obstacles[obstacleNumber].y = random(PTERODACTYL_UPPER_LIMIT, PTERODACTYL_LOWER_LIMIT);
      break;

  }
 
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render the ground.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void drawGround() {

  if (groundX == 32) {
      
    groundX = 0;


    // Randomly select a new road type ..

    byte type = random(0, 8);

    switch (type) {

      case 0 ... 5:
        type = GroundType::Flat;
        break;

      case 6:
        type = GroundType::Bump;
        break;

      case 7:
        type = GroundType::Hole;
        break;
  
    }


    // Shuffle the road elements along and assign the randomly selected type to the last element ..

    ground[0] = ground[1];
    ground[1] = ground[2];
    ground[2] = ground[3];
    ground[3] = ground[4];
    ground[4] = (GroundType)type;

  }

  groundX++;


  // Render the road.  
  
  for (byte i = 0; i < 5; i++) {
  
    switch (ground[i]) {
      
      case GroundType::Flat:
        Sprites::drawOverwrite((i * 32) - groundX, GROUND_LEVEL, ground_flat, frame);   
        break;
        
      case GroundType::Bump:
        Sprites::drawOverwrite((i * 32) - groundX, GROUND_LEVEL, ground_bump, frame);   
        break;
        
      case GroundType::Hole:
        Sprites::drawOverwrite((i * 32) - groundX, GROUND_LEVEL, ground_hole, frame);   
        break;
        
    }

  }

}
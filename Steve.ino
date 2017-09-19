#include <Arduboy2.h>
//#include <EEPROM.h>
#include "EEPROMUtils.h"
#include "Images.h"

#define NUMBER_OF_OBSTACLES         5     
#define GROUND_LEVEL                55
#define STEVE_GROUND_LEVEL          GROUND_LEVEL + 7
#define CACTUS_GROUND_LEVEL         GROUND_LEVEL + 3
#define JUMP_TOP_HEIGHT             10
#define SCORE_START_CACTUS          300
#define PTERODACTYL_UPPER_LIMIT     27
#define PTERODACTYL_LOWER_LIMIT     48
#define OBSTACLE_LAUNCH_DELAY_MIN   90
#define OBSTACLE_LAUNCH_DELAY_MAX   200

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

struct Steve {
  int x;
  int y;
  Stance stance;
  byte jumping;
  byte goingUp;
  byte *image;
  byte *mask;
};

struct Obstacle {
  int x;
  int y;
  ObstacleType type;
  bool enabled;
  byte *image;
};

Arduboy2 arduboy;
int frame = 0;
int groundX = 0;

Obstacle obstacles[5] = {
  { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
  { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
  { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
  { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
  { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
};

GroundType ground[5] = {
  GroundType::Flat,
  GroundType::Flat,
  GroundType::Hole,
  GroundType::Flat,
  GroundType::Flat,
};

Steve steve = {0, STEVE_GROUND_LEVEL, Standing, false, false};

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
  arduboy.boot();
  arduboy.setFrameRate(75);

}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Control the various states of the game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {

  // Pause here until it's time for the next frame ..
  
  if (!(arduboy.nextFrame())) return;
  
  arduboy.pollButtons();

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
  steve.x = 2;
  steve.y = STEVE_GROUND_LEVEL;
  steve.jumping = false;
  steve.stance = Stance::Standing;

}

int r = 32;

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the introduction ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void introduction() {

//  highScore = EEPROMReadInt(EEPROM_SCORE);
  arduboy.clear();

  initialiseGame();

  arduboy.setCursor(17, 12);
  arduboy.print(F("Press A to begin"));

  drawGround(false);
  drawSteve();
  drawScoreboard(false);
  arduboy.display();
    
  if (arduboy.pressed(A_BUTTON)) {
    
    gameStatus = GameStatus::PlayGame; 
    steve.stance = Stance::Running1;
  
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the 'Game Over' text if Steve has hit an obstacle ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void gameOver() {

  if (score > highScore) {
//    EEPROMWriteInt(EEPROM_SCORE, score);
    highScore = score;
  }

  arduboy.clear();
  
  drawObstacles();
  drawGround(false);
  drawSteve();
  drawScoreboard(true);

  arduboy.setCursor(40, 12);
  arduboy.print(F("Game Over"));


  // Update Steve's image.  If he is dead and still standing, this will change him to lying on the ground ..

  updateSteve();

  arduboy.display();
    
  if (arduboy.justPressed(A_BUTTON)) { 
  
    initialiseGame();

    gameStatus = GameStatus::PlayGame; 
    steve.stance = Stance::Running1;

  }
 
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Run Steve, run.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void playGame() {

  arduboy.clear();
  

  // The player can only control Steve if he is running or ducking on the ground ..

  if (steve.y == STEVE_GROUND_LEVEL) {

    if (arduboy.justPressed(A_BUTTON))                          { steve.jumping = true; steve.goingUp = true; }
    if (arduboy.justPressed(B_BUTTON))                          { if (steve.stance != Stance::Ducking2) { steve.stance = Stance::Ducking1; }; } 
    if (arduboy.pressed(LEFT_BUTTON) && steve.x > 0)            { steve.x--; }
    if (arduboy.pressed(RIGHT_BUTTON) && steve.x < 100)         { steve.x++; }


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

      if (!obstacles[i].enabled) { 
        launchObstacle(i); 
        break;
      }

    }

    obstacleLaunchCountdown = random(OBSTACLE_LAUNCH_DELAY_MIN, OBSTACLE_LAUNCH_DELAY_MAX);
            
  }

    
  // Has Steve collided with anything?

  if (collision()) {

    steve.jumping = false;
    steve.goingUp = true;
    
    if (steve.stance <= Stance::Running2) {
      steve.stance = Stance::Dead1;
    }
    else {
      steve.y = STEVE_GROUND_LEVEL;
      steve.stance = Stance::Dead2;
    }
    gameStatus = GameStatus::GameOver;

  }
  else {
    

    // if not, move Steve and any visible obstacles and continue play ..

    updateSteve();
    updateObstacles();
  
    drawObstacles();
    drawGround(true);
    drawSteve();
    drawScoreboard(true);
    if (arduboy.everyXFrames(3)) { score++; }

    arduboy.display();
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Has Steve collided with any visible obstacle ?
 * -----------------------------------------------------------------------------------------------------------------------------
 */
bool collision () {
    
  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    if (obstacles[i].enabled == true) {

      if (collide(steve.x, steve.y - getImageHeight(steve.image), steve.image, obstacles[i].x, obstacles[i].y - getImageHeight(obstacles[i].image), obstacles[i].image)) {

        return true;
          
      }

//      if (arduboy.collide(getSteveRect(), getObstacleRect(i))) {
//        
//        return true;
//          
//      }
      
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
      if (steve.y == STEVE_GROUND_LEVEL) {
        steve.jumping = false;
      }

    }
    
  }


  // Swap the image to simulate running ..

  if (arduboy.everyXFrames(3)) {

    switch (steve.stance) {
  
      case Stance::Running1:
        steve.stance = Stance::Running2;
        break;
      
      case Stance::Running2:
        steve.stance = Stance::Running1;
        break;
      
      case Stance::Ducking1:
        steve.stance = Stance::Ducking2;
        break;
      
      case Stance::Ducking2:
        steve.stance = Stance::Ducking1;
        break;
      
      case Stance::Dead1:
        steve.stance = Stance::Dead2;
        break;
  
      default:
        break;
      
     }

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
      steve.image = dinosaur_still;
      steve.mask = dinosaur_still_mask;
      break;

    case Stance::Running1:
      steve.image = dinosaur_running_1;
      steve.mask = dinosaur_running_1_mask;
      break;

    case Stance::Running2:
      steve.image = dinosaur_running_2;
      steve.mask = dinosaur_running_2_mask;
      break;

    case Stance::Ducking1:
      steve.image = dinosaur_ducking_1;
      steve.mask = dinosaur_ducking_1_mask;
      break;

    case Stance::Ducking2:
      steve.image = dinosaur_ducking_2;
      steve.mask = dinosaur_ducking_2_mask;
      break;

    case Stance::Dead1:
      steve.image = dinosaur_dead_1;
      steve.mask = dinosaur_still_mask;
      break;

    case Stance::Dead2:
      steve.image = dinosaur_dead_2;
      steve.mask = dinosaur_dead_2_mask;
      break;
       
  }

  Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(steve.image), steve.image, steve.mask, frame, frame);
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Return the rectangle that Steve occupies.  This differs depending on whether he is standing or ducking ..
 * -----------------------------------------------------------------------------------------------------------------------------
 * /
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
*/

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Update the position of any visible obstacles ..
 *
 *  If the obstacle has completely off screen, then disable it.  Pterodactyls move 1 pixel per update whereas cacti move one
 *  pixel every second iteration.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void updateObstacles() {

  for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    
    if (obstacles[i].enabled == true) {
      
      switch (obstacles[i].type) {

        case ObstacleType::Pterodactyl1:

          if (arduboy.everyXFrames(2)) {

            obstacles[i].type = Pterodactyl2;

          }

          obstacles[i].x--;

          if (obstacles[i].x < -getImageWidth(pterodactyl_1)) {
            obstacles[i].enabled = false; 
          }

          break;

        case ObstacleType::Pterodactyl2:

          if (arduboy.everyXFrames(2)) {
            
            obstacles[i].type = Pterodactyl1;

          }
          
          obstacles[i].x--;

          if (obstacles[i].x < -getImageWidth(pterodactyl_2)) {
            obstacles[i].enabled = false; 
          }

          break;

        case ObstacleType::SingleCactus:

          obstacles[i].x--;
          if (obstacles[i].x < -getImageWidth(cactus_1)) {
            obstacles[i].enabled = false; 
          }
  
          break;

        case ObstacleType::DoubleCactus:

          obstacles[i].x--;
          if (obstacles[i].x < -getImageWidth(cactus_2)) {
            obstacles[i].enabled = false; 
          }

          break;

        case ObstacleType::TripleCactus:

          obstacles[i].x--;
          if (obstacles[i].x < -getImageWidth(cactus_3)) {
            obstacles[i].enabled = false; 
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
    
    if (obstacles[i].enabled == true) {

      switch (obstacles[i].type) {

        case ObstacleType::Pterodactyl1:
          obstacles[i].image = pterodactyl_1;
          break;

        case ObstacleType::Pterodactyl2:
          obstacles[i].image = pterodactyl_2;
          break;

        case ObstacleType::SingleCactus:
          obstacles[i].image = cactus_1;
          break;

        case ObstacleType::DoubleCactus:
          obstacles[i].image = cactus_2;
          break;

        case ObstacleType::TripleCactus:
          obstacles[i].image = cactus_3;
          break;

      }

      Sprites::drawOverwrite(obstacles[i].x, obstacles[i].y - getImageHeight(obstacles[i].image), obstacles[i].image, frame);      

    }
    
  }
  
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Return the rectangle that an obstacle occupies.  Each obstacle has a different size ..
 * -----------------------------------------------------------------------------------------------------------------------------
 * / 
Rect getObstacleRect(byte index) {

  Obstacle thisObstacle = obstacles[index];
    
  switch (thisObstacle.type) {

    case ObstacleType::Pterodactyl1:
    case ObstacleType::Pterodactyl2:
      return Rect { thisObstacle.x, thisObstacle.y, getImageWidth(pterodactyl_1), getImageHeight(pterodactyl_1) };

    case ObstacleType::SingleCactus:
      return Rect { thisObstacle.x, thisObstacle.y - getImageHeight(cactus_1), getImageWidth(cactus_1), getImageHeight(cactus_1) };

    case ObstacleType::DoubleCactus:
      return Rect { thisObstacle.x, thisObstacle.y - getImageHeight(cactus_2), getImageWidth(cactus_2), getImageHeight(cactus_2) };

    case ObstacleType::TripleCactus:
      return Rect { thisObstacle.x, thisObstacle.y - getImageHeight(cactus_3), getImageWidth(cactus_3), getImageHeight(cactus_3) };

  }

}
*/

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render the scoreboard at the top of the screen ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void drawScoreboard(bool displayCurrentScore) {

  arduboy.fillRect(0, 0, WIDTH, 10, BLACK);

  if (displayCurrentScore) { 
    
    arduboy.setCursor(1, 0);
    arduboy.print(F("Score: "));
    arduboy.setCursor(39, 0);
    if (score < 1000) arduboy.print("0");
    if (score < 100) arduboy.print("0");
    if (score < 10)  arduboy.print("0");
    arduboy.print(score);
       
  }
  
  arduboy.setCursor(72, 0);
  arduboy.print(F("High: "));
  arduboy.setCursor(104, 0);
  if (highScore < 1000) arduboy.print("0");
  if (highScore < 100) arduboy.print("0");
  if (highScore < 10)  arduboy.print("0");
  arduboy.print(highScore);
  
  arduboy.drawLine(0, 9, WIDTH, 9, WHITE);

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
      obstacles[obstacleNumber].y = CACTUS_GROUND_LEVEL;
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
void drawGround(bool moveGround) {

  if (moveGround) {

    if (groundX == 32) {
        
      groundX = 0;


      // Randomly select a new road type ..

      byte type = random(0, 6);

      switch (type) {

        case 0 ... 3:
          type = GroundType::Flat;
          break;

        case 4:
          type = GroundType::Bump;
          break;

        case 5:
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

  }


  // Render the road.  
  
  for (byte i = 0; i < 5; i++) {
  
    switch (ground[i]) {
      
      case GroundType::Flat:
        Sprites::drawSelfMasked((i * 32) - groundX, GROUND_LEVEL, ground_flat, frame);   
        break;
        
      case GroundType::Bump:
        Sprites::drawSelfMasked((i * 32) - groundX, GROUND_LEVEL, ground_bump, frame);   
        break;
        
      case GroundType::Hole:
        Sprites::drawSelfMasked((i * 32) - groundX, GROUND_LEVEL, ground_hole, frame);   
        break;
        
    }

  }

}

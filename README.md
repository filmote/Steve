# Steve
Steve - the Running Dinosaur for Arduboy




# Introduction

The following article describes how to build a simple game based on the Google browser game that is available if you are offline.  Other versions of this game are available on iOS and Android and are called *Steve the Jumping Dinosaur*.

This article assumes you have read Crait’s series of articles that describe the process of setting up an Arduboy environment, writing simple applications and culminating in the writing of your first game – the classic Pong!  

If you haven’t already read Crait’s articles, stop reading this and do that first.

[Make Your Own Arduboy Game: Part 1 - Setting Up Your Computer](https://community.arduboy.com/t/make-your-own-arduboy-game-part-1-setting-up-your-computer/2336)

[Make Your Own Arduboy Game: Part 2 - Printing Text](https://community.arduboy.com/t/make-your-own-arduboy-game-part-2-printing-text/2337/1)

[Make Your Own Arduboy Game: Part 3 - Storing Data & Loops](https://community.arduboy.com/t/make-your-own-arduboy-game-part-3-storing-data-loops/2338/4)

[Make Your Own Arduboy Game: Part 4 - Questions & Button Input](https://community.arduboy.com/t/make-your-own-arduboy-game-part-4-questions-button-input/2376/1)

[Make Your Own Arduboy Game: Part 5 - Your First Game!](https://community.arduboy.com/t/make-your-own-arduboy-game-part-5-your-first-game/2396/10)

[Make Your Own Arduboy Game: Part 6 - Graphics!](https://community.arduboy.com/t/make-your-own-arduboy-game-part-6-graphics/2440/49)

[Make Your Own Arduboy Game: Part 7 - Make Pong From Scratch!](https://community.arduboy.com/t/make-your-own-arduboy-game-part-7-make-pong-from-scratch/2615/47)


Crait’s articles do a great job of detailing the structure of an Arduboy program.  This article adds some additional concepts including enumerations, structures and arrays of images to build a more advanced game than Pong.

If I haven’t already said it, make sure you read Crait’s articles first.


# Creating Steve the Jumping Dinosaur

Steven the Jumping Dinosaur is an endless runner where you jump oncoming cacti and duck under low-flying pterodactyls.  The longer you run, the higher your score!  

Before we jump (all puns intended) in to writing some code, we need to look at a few new concepts:


## Structures

A structure is a way of grouping related variables together into a single construct.  A structure can be passed to a function or referred to as a single unit. 

An example of a structure is shown below. It represents a node and contains three variables that hold the coordinates and node value respectively.
```
struct Node {
    byte x;
    byte y;
    byte value;
};
```

Once defined, a new variable can be created using it as a datatype as shown below.  The members of the structure can be referenced directly to assign or retrieve their values.

```
Node aNode;
aNode.x = 3;
aNode.y = 4;

int x = calc(aNode);

int calc(Node theNode) {
    return theNode.x * theNode.y;
}
```
The declaration and initialization code above could have been expressed in a single line as shown below.  The members of the structure must be initialized in the sequence they were declared in the structure definition.  Not all members must be initialized and the second example is also valid and the ```value``` member would be initialized to 0.  

```
Node myNewNode = {3, 4, 5};
Node myNewNode = {3, 4};
```

## Enumerations

An enumerations is a data type consisting of a set of named values.  Under the covers, the values are all of type integer but the programmer may not ever need to know or care what the values are.  

Enumerations can be named or anonymous as shown below.  Note in the first example, I have nominated that the first element, ```Cat```, is assigned the numeric value 10.  ```Dog``` will automatically be assigned the value of 11 and ```Mouse```, 12.  In the second example where no starting number is specified, the items are numbered from zero onwards.

```
enum Pet {
    Cat = 10,
    Dog,
    Mouse
};

enum {
    Horse,
    Cow,
    Sheep
};
```

Once an enumeration is declared, it can be used as shown in the valid declarations below.

```
int aHorse = Horse;
Pet aCat = Cat;
Pet aDog = Pet::Dog;
```

Named enumerations can be used when defining parameters of a function.  As you can see from the sample calls, the declaration will not prevent you from passing any other enumeration type or even an integer.


```
void printPetDetails(Pet thePet) {

    switch (thePet) {

        case Pet::Cat:
            Serial.println("cat");
            break;

        case Pet::Dog:
            Serial.println("dog");
            break;

        case Pet::Mouse:
            Serial.println("mouse");
            break;

        default:
            Serial.println("other");
            break;
    }
}

printPetDetails(aCat);	< prints ‘cat’
printPetDetails(aDog); 	< prints ‘dog’
printPetDetails(aHorse); 	< prints ‘other’ but is illogical as it isn’t a Pet
printPetDetails(2); 	< prints ‘other’ but is illogical as it isn’t a Pet
```


## Sprites

A sprite is an image or graphic that represents a player, an enemy or even background elements of your game and can be drawn or moved around as a single object.  The Arduboy library provides a number of functions to render sprites to the screen.  In this tutorial, we will concentrate on two functions ```drawOverwrite()``` and ```drawExternalMask()```.

But before we get into those, let’s recap how we define a sprite.  The image below shows Steve in his upright, ready-to-run position.


![Artwork/Steve%203.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.001.png)


The array definition for this sprite is shown below.

```
const byte PROGMEM dinosaur_still[] = {

    18, 21,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xFE, 0xFF, 0xFB, 0xFF, 0xFF, 0xBF, 0xBF, 0x3F, 0x3E,

    0x3F, 0x7C, 0xF8, 0xF0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 
    0xFF, 0xFF, 0x7F, 0x3F, 0x04, 0x0C, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x01, 0x1F, 0x17, 0x03, 0x01, 0x03, 
    0x1F, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
```

I have formatted the array to make it a little more readable.  The first line contains the width and height of the array, in this case 18 pixels wide by 21 pixels.  The remaining rows contain the pixel data of the actual image.

I am not going to describe how the image data is calculated in this article – it has been described in detail in other articles in this magazine including this one on Page 23 of the [Arduboy Magazine (Vol 7)](https://issuu.com/arduboymag/docs/vol_7Arduboy%20Magazine%20Volume%207).  Team ARG have a great online tool that allows you to drop a black and white image on to the page and calculate the image data.  It can be found [here](https://teamarg.github.io/arduboy-image-converter/). 

The following code will render Steve at position x = 4, y = 5.

```
Sprites::drawOverwrite(3, 4, dinosaur_still, 0);
```

The last parameter – the zero – is the frame number.  We will cover that in a few moments.

When rendering an image using ```drawOverwrite()```, it overwrites whatever is on the screen with the image specified.  There is no concept of transparent pixels so the entire rectangle that the sprite covers is affected.


## Masks

The Arduboy library also provides some nice masking utilities that allow you to render a sprite over a background without clearing the entire rectangle that the sprite occupies.  In a sense it allows you to mark areas of a sprite as solid (black or white) and transparent.  

Consider the following image and mask:


![../Pipes_Article1_SpritesDemo/images/ball.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.002.png) ![../Pipes_Article1_SpritesDemo/images/mask.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.003.png)


As mentioned above, ```drawOverwrite()``` simply draws the sprite and overwrites what was already there. In the example below, the black corners of the ball are visible as the ball passes into the white area.


![../Pipes_Article1_SpritesDemo/images/DrawOverwrite_1.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.004.png)  ![../Pipes_Article1_SpritesDemo/images/DrawOverwrite_2.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.005.png)  ![../Pipes_Article1_SpritesDemo/images/DrawOverwrite_3.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.006.png)


As the name implies, the ```drawExternalMask()``` function allows the image and mask to be nominated when rendering.  Bits set to 1 in the mask indicate that the pixel will be set to the value of the corresponding image bit.  Bits set to 0 in the mask will be left unchanged.  This can be seen clearly as the ball moves into the right hand side of the background.  The top-left and bottom-right corners of the image are rendered as black as the mask is set to 1 in these areas which in turn ensures that the images pixels (both zeroes and ones) are rendered on the background.


![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_1.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.007.png)  ![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_2.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.008.png)  ![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_3.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.009.png)

![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_S_1.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.010.png)  ![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_S_2.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.011.png)  ![../Pipes_Article1_SpritesDemo/images/DrawExternalMask_S_3.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.012.png)


A second article on Page 33 of the [Arduboy Magazine (Vol 7)](https://issuu.com/arduboymag/docs/vol_7Arduboy%20Magazine%20Volume%207) describes the various other mask options available and contains a link to a project that demonstrates the various techniques.

The sample code below shows a call to the ```drawExternalMask()``` function.  As with the ```drawOverwrite()``` function, it has specifies the frame for each image.

```
Sprites::drawExtnerlaMask(3, 4, dinosaur_still, dinosaur_mask, 0, 0);
```

## Frames

One challenge when writing a game is to ensure that the speed of the game is not affected by the amount of activity that is going on.  The Arduboy supports a concept called frames which allows activity to be coordinated - paused even – so that game play speed is constant.  Think of it like a traffic cop that slows down fast processes and waits until it’s time for the next round of activity.

The Arduboy allows you to specify the number of frames you want per second.  A large number will result in many frames per second and therefore the period between frames is very short.  A small number will result in a larger gap between frames.  The code below shows a typical setup:

```
void setup() {

    arduboy.boot();
    arduboy.setFrameRate(75);

}

void loop() {

    if (!(arduboy.nextFrame())) return;

    …
}
```

The first line of code in the ```loop()``` is crucial.  It induces a delay if it is not time to process the next frame.

Frames can also be used as a basic counter and to animate graphics.  Later when we look at rendering Steve, we will use the frame count to alternate between an images where his left foot is down to a second where his right foot is down.


# Moving the Ground

The illusion of movement in an endless runner is important to game play.  To provide a little variety, I have designed three separate graphics which include flat land, a bump and a pot-hole.  The variations are irrelevant to the game play.

![../../../Desktop/Ground%201.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.013.png)        ![../../../Desktop/Ground%202.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.014.png)         ![../../../Desktop/Ground%203.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.015.png)


These are enumerated in an enum called ```GroundType```, as shown below.  The first element in an enumeration is implicitly assigned a value of zero and subsequent elements increase in value by one.  An array of images has also been declared with the images arranged in the same order as the enumeration allowing us to use the enumeration elements as index values to retrieve the images.

```
enum GroundType {
    Flat,
    Bump,
    Hole,
};

const byte *ground_images[] = { ground_flat, ground_bump, ground_hole };
```

The ‘ground’ itself is made up of five images that are 32 pixels wide to give a combined width of 160 pixels.  As you will see in a moment, the array of images will be rendered across the page overlapping the 128 pixels of the screen width.  Moving the images a pixel to the left and re-rendering them will provide the illusion that the ground is moving.

The array is declared and initialized as shown below:
```
GroundType ground[5] = {
    GroundType::Flat,
    GroundType::Flat,
    GroundType::Hole,
    GroundType::Flat,
    GroundType::Flat,
};
```

When rendering the ground for the first time, the first four images are rendered at X position 0, 32, 64 and 96 respectively.  The fifth image is also rendered but as its X position is 128 it is not visible off to the right of the screen.  

The following code renders the ground.  It loops through the ground array and draws the five elements 32 pixels apart.   


```
for (byte i = 0; i < 5; i++) {

    Sprites::drawSelfMasked((i * 32) - groundX, GROUND_LEVEL, ground_images[ground[i]], frame);   

}
```

In the code above, the variable ```groundX``` is used as an offset and is initially set to zero so has no affect.  To scroll the ground to the left, the ground variable is incremented.  Assuming the value is now one, this results in the five images being rendered at X positions -1, 31, 63, 95, and 127 respectively.  The left most pixels of the first image are no longer visible and the left most pixels of the right are now rendered on the right most side of the screen.

The ground can be continued to be scrolled until the offset equals 32 (our image width) at which point the images are being rendered at the X positions -32, 0, 32, 64 and 96 respectively.  At this point the first image is completely off screen.  At this point, we need to move the elements of the ground array to the left one position and randomly select an image for the fifth position.

The code below detects when the offset has reached the 32 and randomly selects a number between 0 and 5 and assigns the ground type accordingly.  One thing to note about the ```random()``` function is that the lower bound is inclusive whereas the upper bound is inclusive.  A value of 3 or lower results in a flat section of ground whereas the values 4 and 5 are mapped to a bump and a pothole.  This approach ensures that many more flat sections of ground are generated.

Finally, the elements of the array are shuffled to the left and the newly generated ground type is assigned to the fifth element.

```
if (groundX == 32) {

    groundX = 0;

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

    ground[0] = ground[1];
    ground[1] = ground[2];
    ground[2] = ground[3];
    ground[3] = ground[4];
    ground[4] = (GroundType)type;

}

groundX++;
```


# Moving and Rendering Steve

When Steve is not standing around waiting for the game to begin he may be running, ducking or simply be dead.  These various states or stances are shown below.


![../../../Desktop/Steve%201.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.016.png)        ![../../../Desktop/Steve%201.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.017.png)        ![../../../Desktop/Steve%202.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.018.png)        ![../../../Desktop/Steve%207.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.019.png)

![../../../Desktop/Steve%204.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.020.png)      ![../../../Desktop/Steve%205.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.021.png)          ![../../../Desktop/Steve%206.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.022.png)


These stances are enumerated in an enum called ```Stance```.  As with the ground types detailed above, a matching array of dinosaur images has been declared with the images arranged in the same order as the Stance enumeration.  A second array of masks has also been declared – again with the masks in the same order as the ```Stance``` enumeration – to allow us to use the ```Stance``` values as indexes.

```
enum Stance {
    Standing,
    Running1,
    Running2,
    Ducking1,
    Ducking2,
    Dead1,
    Dead2,
};

const byte *steve_images[] = { dinosaur_still, dinosaur_running_1, dinosaur_running_2, dinosaur_ducking_1, dinosaur_ducking_2, dinosaur_dead_1, dinosaur_dead_2 };

const byte *steve_masks[] = { dinosaur_still_mask, dinosaur_running_1_mask, dinosaur_running_2_mask, dinosaur_ducking_1_mask, dinosaur_ducking_2_mask, dinosaur_dead_2_mask, dinosaur_dead_2_mask };
```

All of the details relating to Steve’s position and current stance are stored in a single structure.  Note that in structures, we can include fields using the common data types (integers, Booleans and so forth) and ones that refer to the enumerations we have already defined.   

```
struct Steve {
    int x;
    int y;
    Stance stance;
    bool jumping;
    byte jumpIndex;
    const byte *image;
    const byte *mask;
};
```

Once the structure has been declared, we can create an instance of it and initialise all of the elements in a single line as shown below.  We now have a container to track changes in Steve’s position.
```
Steve steve = {0, STEVE_GROUND_LEVEL, Stance::Standing, false, false, dinosaur_still, dinosaur_still_mask };
```

Our Steve structure has two fields that point to the image and mask that represent his current stance.  Later we will use these to determine whether Steve has crashed into a cactus or pterodactyl.   For now, we can populate these by looking up looking up the image and mask from the array of images using Steve’s current stance as an index.  

Steve can then be rendered using the ```Sprites::drawExternalMask()``` function with Steve’s current position and the two image references.  As Steve can be standing or ducking, I have assumed that his coordinates represent the lower left position.  By default, images are rendered from the top left position and therefore I have subtracted the height of the image from Steve’s Y position to accommodate this.

```
void drawSteve() {
    steve.image = steve_images[steve.stance];
    steve.mask = steve_masks[steve.stance];
    Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(steve.image), steve.image, steve.mask, frame, frame);
}
```

Now that we can draw Steve on the screen, we need to be able to move him around.  Our structure includes an X and Y coordinates plus fields that represent his current stance and whether or not he is jumping.

Handling user input is generally handled in the main loop of the program.  The code below tests first whether Steve is jumping and, if not, allows the player to control him.  The Arduboy library provides four commands for detecting the pressing and releasing of buttons ```- justPressed()```, ```justReleased()```, ```pressed()``` and ```notPressed()```.   The first two commands are used in conjunction with the command ```pollButtons()```.

A call to ```pollButtons()``` at the start of the main loop captures the current state of the Arduboy’s buttons.  The ```justPressed()``` and ```justReleased()``` commands will return true if the user has pressed a button since the last time ```pollButtons()``` was called.  

The ```pressed()``` command returns true if the nominated button is pressed or remains pressed over a period of time.  You can see that I am using the ```justPressed()``` command to detect a jump as it is a one off event whereas I am allowing the user to hold the left and right buttons to move continuously by using the ```pressed()``` command .

```
arduboy.pollButtons();
…
if (!steve.jumping) {

    if (arduboy.justPressed(A_BUTTON)) {
        steve.jumping = true;
        steve.jumpIndex = 0;
    }

    if (arduboy.justPressed(B_BUTTON)) {
        if (steve.stance != Stance::Ducking2) {
            steve.stance = Stance::Ducking1;
        };
    }
    if (arduboy.pressed(LEFT_BUTTON) && steve.x > 0) {
        steve.x--;
    }
    if (arduboy.pressed(RIGHT_BUTTON) && steve.x < 100) {
        steve.x++;
    }

    if (arduboy.notPressed(B_BUTTON) && 
        (steve.stance == Stance::Ducking1 || steve.stance == Stance::Ducking2)) {
        steve.stance = Stance::Running1;
    }
}
```

The last clause of the code above detects whether the player has let go of the B button (the duck button) and returns Steve to a running stance if he was already ducking.  The second part of the condition is important as the ```notPressed()``` command - as the name suggests - will return true when the user is not pressing the button and we do not want Steve’s stance to be changed unless he was actually ducking.

At the end of each loop, we need to update Steve’s position if he is jumping or change the image displayed to give the illusion of his feet moving.  

## Jumping

Previously, we detected if the user had pressed the A button and set the jumping flag to true.  If it is true we want Steve to jump an over oncoming obstacles in an arc that somewhat simulates the effects of gravity – fast acceleration at the start of the jump, deceleration as he reaches full height followed by acceleration as he falls to earth,

The array below describes the Y positions of Steve as his jump progresses.  It starts and ends at Y = 55 (or ground level) and reaches a height of 19 in the middle.  

```
unsigned char jumpCoords[] = {55, 52, 47, 43, 40, 38, 36, 34, 33, 31, 30, 29, 28, 27, 26, 25, 24, 24, 23, 23, 22, 22, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 26, 27, 28, 29, 30, 31, 33, 34, 36, 38, 40, 43, 47, 51, 55 };
```

The code below updates Steve’s position when jumping. In addition to the jumping flag, the data structure that maintains Steve’s details also includes an array index, named ```jumpIndex```, which is used to keep track of the current position in the array.  On each pass of the main loop, adjust Steve’s height to the value within the array that the index points to.  We then increment the array index up until the last value of the array is reached at which point Steve has returned to the ground and the jump is over.  Steve’s jumping property is set to false and the array index is set to zero in anticipation of his next jump.



```
void updateSteve() {

    if (steve.jumping) {

        steve.y = jumpCoords[steve.jumpIndex];
        steve.jumpIndex++;

        if (steve.jumpIndex == sizeof(jumpCoords)) {

            steve.jumping = false;
            steve.jumpIndex = 0;
            steve.y = STEVE_GROUND_LEVEL;
            
        }
    }
    else {

        ...
    }
}
```

## Running or Ducking

The illusion of running is achieved by alternating the images used when rendering Steve.  If we alternate the images every frame (or every time we pass through the main loop), Steve’s legs will appear as a blur as this is too fast.  To slow this down, we can use an Arduboy library function called ```everyXFrames(n)``` which returns true on every nth frame.

The code below alternates Steve’s stance every 3 frames.  If his current stance is ```Stance::Running1``` then it becomes ```Stance::Running2``` – if it was already ```Stance::Running2``` then it reverts back to the original value.  This is true of the ducking images but not true of the dead images – once you are dead, you remain dead.

```
void updateSteve(){

    if (steve.jumping) {
        ...

    }
    else {

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
```

Now that we have Steve running, jumping and ducking we can move on to the obstacles he will need to avoid.


# Launching an Obstacle

Steve the Dinosaur must avoid four type of obstacles - the single, double and triple cacti and a flying pterodactyl.  These various obstacle types are enumerated in an ```enum``` called ```ObstacleType``` as shown below.  The first three elements are self-explanatory - the two pterodactyl elements are used to represent the animal (I was about to say bird but they were in fact reptiles!) with its wing up and down.  Later we will see how we animate the image but for now you can ignore the second element, ```Pterodactyl2```.

```
enum ObstacleType {
    SingleCactus,
    DoubleCactus,
    TripleCactus,
    Pterodactyl1,
    Pterodactyl2,
    Count_AllObstacles = 4,
};
```

Unless otherwise specified, elements in an enumeration are assigned values starting from zero.  In the enumeration above, the ```SingleCactus``` element has a value of 0 and the ```Pterodactly2``` element has a value of 4.  Although there are five elements, when we randomly launch objects there are only four types to choose from as the Pterodactyl1 and ```Pterodacytl2``` elements describe the same thing.  The element ```Count_AllObstacles``` is used to define the number of options available.  Note that I have explicitly assigned it a value of 4.  Enumerations do not have to have contiguous element values and multiple elements can have the same values.

The details of a single obstacle are stored in a structure as defined below.  In addition to the obstacle’s position, the structure also contains the object type, an enabled flag and a reference to the image that will be used when rendering it.  As mentioned earlier, structures are a great mechanism for capturing related data together.

```
struct Obstacle {
    int x;
    int y;
    ObstacleType type;
    bool enabled;
    const byte *image;
};
```

At any time during game play, two or even three obstacles may be visible on the screen.  To cater for this, I have created an array of obstacles and initialised them with default values.  Note that all of the obstacles are disabled by default.

```
#define NUMBER_OF_OBSTACLES  3

Obstacle obstacles[NUMBER_OF_OBSTACLES] = {
    { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
    { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
    { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
};
```

When launching obstacles, we need to make sure that the obstacles are randomly placed but not too close together otherwise Steve may not be able to land between them and jump again.  To facilitate this, we generate a random delay and store this into the variable ```obstacleLaunchCountdown``` which is decremented each pass of the main game loop.  When this variable reaches zero, a simple loop passes through the ```obstacles[]``` array looking for the first inactive obstacle in the collection.  

```
#define LAUNCH_DELAY_MIN   90
#define LAUNCH_DELAY_MAX   200

--obstacleLaunchCountdown;

if (obstacleLaunchCountdown == 0) {

    for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

        if (!obstacles[i].enabled) { 
            launchObstacle(i); 
            break;
        }
    }

    obstacleLaunchCountdown = random(LAUNCH_DELAY_MIN, LAUNCH_DELAY_MAX);

}
```

The actual code to launch a new obstacle is shown below.  The input parameter, ```obstacleNumber```, defines which of the three obstacles in the array to activate.  To help the player get accustomed to the various obstacles, they are introduced slowly as the player’s score increases.  

The first section of the routine calculates which of the elements in the ```ObstacleType``` enumeration can be chosen based on the player’s score.  When the player’s score is less than 100, only the single cacti is valid.  When the player’s score is less than 200, the single and double cacti images are valid - likewise a score less than 300 allows all three cacti obstacles to be chosen.  Once the score exceeds 300 all obstacles including the pterodactyl are valid. 

If a pterodactyl obstacle is chosen then a flying height is randomly selected between an upper and lower limit as defined by the two constants ```PTERODACTYL_UPPER_LIMIT``` and ```PTERODACTYL_LOWER_LIMIT```.  In contrast, cacti are all launched at ground level.

```
#define PTERODACTYL_UPPER_LIMIT     27
#define PTERODACTYL_LOWER_LIMIT     48

void launchObstacle(byte obstacleNumber) {

    // Randomly pick an obstacle ..

    ObstacleType randomUpper = ObstacleType::SingleCactus;

    switch (score) {

        case 0 ... 99: 
            randomUpper = ObstacleType::SingleCactus;
            break;

        case 100 ... 199: 
            randomUpper = ObstacleType::DoubleCactus;
            break;

        case 200 ... 299: 
            randomUpper = ObstacleType::TripleCactus;
            break;

        default:
            randomUpper = ObstacleType::Count_AllObstacles;
            break;
    }

    ObstacleType type = (ObstacleType)random(ObstacleType::SingleCactus, randomUpper + 1);


    // Launch the obstacle ..

    obstacles[obstacleNumber].type = type;
    obstacles[obstacleNumber].enabled = true;
    obstacles[obstacleNumber].x = WIDTH - 1;

    if (type == ObstacleType::Pterodactyl1) {
        
        obstacles[obstacleNumber].y = random(PTERODACTYL_UPPER_LIMIT, PTERODACTYL_LOWER_LIMIT);

    }

    else {

        obstacles[obstacleNumber].y = CACTUS_GROUND_LEVEL;

    }

}
```

It’s worth pointing out the use of ranges in the case statement in the above example.  Ranges must be specified in the format shown with three decimal points between them and the ranges cannot overlap.  Although this syntax is valid in the Arduino / Arduboy environment, it is not valid in most C++ implementations.

# Moving Obstacles

The obstacles in our game move from right to left, one pixel per frame.  The code below checks to see which of our obstacles are enabled and decrements their x coordinate by one.  As the objects move out of view on the left hand side of the screen, they are disabled which allows them to be relaunched in the future.

We apply the same technique used to animate Steve’s feet to animate the pterodactyl’s wings.  

```
void updateObstacles() {

    for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

        if (obstacles[i].enabled == true) {

            switch (obstacles[i].type) {

                case ObstacleType::Pterodactyl1:
                case ObstacleType::Pterodactyl2:

                    if (arduboy.everyXFrames(2)) {
                        if (obstacles[i].type == Pterodactyl1) { 
                            obstacles[i].type = Pterodactyl2;
                        }
                        else {
                            obstacles[i].type = Pterodactyl1;
                        }
                    }

                    obstacles[i].x--;
                    break;

                case ObstacleType::SingleCactus:
                case ObstacleType::DoubleCactus:
                case ObstacleType::TripleCactus:

                    obstacles[i].x--;
                    break;

            }

        // Has the obstacle moved out of view ?

            if (obstacles[i].x < -getImageWidth(obstacles[i].image)) {
                obstacles[i].enabled = false; 
            }
        }
    }
}
```

# Detecting Crashes

The Arduboy library provides a simple function that detects the collision between two rectangles.  To detect a collision between Steve and a cactus, the rectangular boundary of the images could be supplied.

The following code loops through the array of obstacles and test for collisions between Steve and enabled or active obstacles.  The boundaries of each image are captured into a ```Rect``` object - a structure provided by the Arduboy library - and these use as parameters in the Arduboy ```collision()``` method.  If a collision or overlap of the two rectangles is detected, the function returns true.

```
bool collision () {

    for (byte i = 0; i < NUMBER_OF_OBSTACLES; i++) {

        if (obstacles[i].enabled == true) {

            Rect steveRect = Rect{  steve.x, 
                                    steve.y - getImageHeight(steve.image),
                                    getImageWidth(steve.image),
                                    getImageHeight(steve.image) };

            Rect obsRect =   Rect{  obstacles[i].x, 
                                    obstacles[i].y - getImageHeight(obstacles[i].image),
                                    getImageWidth(obstacles[i].image), 
                                    getImageHeight(obstacles[i].image) };

            if (arduboy.collide(steveRect, obsRect)) {
                return true;
            }
        }
    }

    return false;
}
```

Hang on! This code isn’t the same as in the sample code.  Right .. the reason for this is that the standard collision code works well for detecting head-on collisions between images that fill the majority of the rectangle they are contained within but performs poorly when trying to detect collisions between image corners or images that do not fill the entirety of their containing rectangle.

This is shown in the example below.  Although the Arduboy ```collision()``` function reports a collision, they are clearly not touching.


![../../../Desktop/SteveOverlap.png](README_Images/Aspose.Words.c20c4235-5805-4aa2-89bf-125951a0ee82.023.png)

The code in the sample application uses a modified collision detection function that I have described in detail in the Arduboy Magazine which looks at the pixels of the images themselves rather than the containing rectangle to detect a collision.  

If you are interested, you can read this advanced article [here](https://issuu.com/arduboymag/docs/arduboy_magazine_8).  

# Saving Scores

The Arduboy includes a small amount of non-volatile memory, known as EEPROM, which can store and retain information even when the unit is turned off.

EEPROM is ideal for saving user settings, high scores and other information between sessions.  EEPROM stands for Electrically Erasable Programmable Read-Only Memory but this is a misnomer as the memory can actually be updated.  EEPROMs have a limited life and will eventually fail after they have been erased and rewritten too many times – this number may be in the millions of operations but a poorly written program that attempts to use it as working memory could easily reach that.

The EEPROM class provides three basic functions to read and write a single byte of memory, as shown below.  The memory location can be anywhere in the 1Kb and equates to a value between 0 and 1023.  The ```update()``` function differs from the ```write()``` function in that it checks the value to be written against what is already stored in order to minimize the number of updates thus prolonging the life of the EEPROM.


```
EEPROM.read(memory_location);
EEPROM.update(memory_location, value);
EEPROM.write(memory_location, value);
```

The library also offers two other functions that can save and retrieve datatypes other than a byte, such as a float, integer or even a structure.

```
EEPROM.put(memory_location, value);
EEPROM.get(memory_location, value);
```

Using these functions, we can save Steve’s top scores.  We can save it anywhere in the 1Kb range however the first 16 bytes are reserved for storing Arduboy system details including the current sound state (on / off), the unit name and other bits and pieces.

The Arduboy library defines a constant, ```EEPROM_STORAGE_SPACE_START```, which indicates the first memory location free for user information.  The code below allows us to save and retrieve Steve’s score into the lowest available EEPROM memory location.

```
EEPROM.get(EEPROM_STORAGE_SPACE_START, highScore);
EEPROM.put(EEPROM_STORAGE_SPACE_START, highScore);
```

Depending on what other games we have been playing previously, these memory locations may contain invalid data that can cause an error or, at worst, report unrealistically high scores.  To overcome this, I like to store two fixed characters in front of my application’s data.  When the application starts, it checks in the EEPROM memory for the two characters and if it does not find them clears out the memory it plans to use.  It then populates the two characters so future checks do not clear the score again.

This is achieved using the following code:

```
#define EEPROM_START_C1               EEPROM_STORAGE_SPACE_START
#define EEPROM_START_C2               EEPROM_START_C1 + 1
#define EEPROM_SCORE                   EEPROM_START_C1 + 2

void initEEPROM() {

    unsigned char c1 = EEPROM.read(EEPROM_START_C1);
    unsigned char c2 = EEPROM.read(EEPROM_START_C2);

    if (c1 != ‘S’ || c2 != ‘T’) {   

        EEPROM.update(EEPROM_START_C1, ‘S’);
        EEPROM.update(EEPROM_START_C2, ‘T’);
        EEPROM.put(EEPROM_SCORE, (unsigned int)0);
    
    }
}
```


# Putting It All Together

So far we have delved into the details of creating an endless runner game.  We have moving ground, a moving player and random obstacles to avoid.  If we do hit an obstacle it can be detected and we can save our best scores into EEPROM for posterity.

But how do we put it all together?

In the final part of Crait’s lessons, [Make Your Own Arduboy Game: Part 7 - Make Pong From Scratch!](https://community.arduboy.com/t/make-your-own-arduboy-game-part-7-make-pong-from-scratch/2615/47), he discusses a ‘state machine’ pattern for controlling the flow of a game between introduction screens, the actual game play and game over screens.  Our game uses this exact same concept with a minor change – the gameStatus variable that holds the current state is defined as an enumeration which in turn makes the code more readable.

```
GameStatus gameStatus = GameStatus::Introduction;

void loop() {

    …

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
```

The main ```loop()``` is intentionally kept clean and all work is done by discrete functions.  The ```introduction()``` function performs the single task of displaying the introduction screen and waiting for the user to start a game.  

Before completing it updates the gameStatus variable with the next state – in this case ```GameStatus::PlayGame```.  On the next pass through the main ```loop()```, control will pass to the ```playGame()``` function. 

```
void introduction() {

    EEPROM.get(EEPROM_SCORE, highScore);
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
```

I encourage you to review the structure of the code taking note how control is passed between the functions using the ```gameStatus``` variable.  Digging into these functions will reveal the snippets of code we have discussed above and they should be quite recognisable.

Finally, what could you change to make this version of the game your own?  Maybe some different obstacles – you could simply replace the images or you could add new obstacles by creating images, adding the new types into the ```ObstacleType``` enumeration and changing the ```launchObstacle()``` function.  Why not add a caveman with a big club?  

You can download the finished game [here](https://github.com/filmote/Steve).

Have fun and watch those pterodactyls.


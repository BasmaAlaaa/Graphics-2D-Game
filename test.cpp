#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <time.h>

// Global Variables

float playerX = 400, playerY = 400; // Player starts in the middle of the screen
float playerRotation = 0.0; // For player rotation
const int screenWidth = 800, screenHeight = 800;
int powerUp1Timer = 0;
int powerUp2Timer = 0;
int powerUpDuration = 2;
int lives = 5;
int score = 0;
int gameTime = 500;
int PLAYER_SPEED = 10;
bool powerUp1Active = false, powerUp2Active = false;
struct Player {
    float x;
    float y;
    float rotation;  // in degrees
};

Player player = { screenWidth / 2, screenHeight / 2, 0.0f };


// Prototype functions
void drawCircle(int x, int y, float radius);
void drawRect(int x, int y, int width, int height);
void checkCollision();


// Assuming these data structures
struct GameObject {
    float x;
    float y;
};

const int NUM_OBSTACLES = 5;
const int NUM_COLLECTABLES = 5;
const int NUM_POWERUPS = 2;

GameObject obstacles[NUM_OBSTACLES];
GameObject collectables[NUM_COLLECTABLES];
GameObject powerUps[NUM_POWERUPS];

void generateRandomGameObjects() {
    srand(time(NULL));  // Seed for random numbers

    // Randomly position obstacles
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].x = rand() % screenWidth;
        obstacles[i].y = rand() % screenHeight;
    }

    // Randomly position collectables
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        collectables[i].x = rand() % screenWidth;
        collectables[i].y = rand() % screenHeight;
    }

    // Randomly position power-ups
    for (int i = 0; i < NUM_POWERUPS; i++) {
        powerUps[i].x = rand() % screenWidth;
        powerUps[i].y = rand() % screenHeight;
    }
}

void drawGameObject(const GameObject& obj, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(obj.x - 5, obj.y - 5);
    glVertex2f(obj.x + 5, obj.y - 5);
    glVertex2f(obj.x + 5, obj.y + 5);
    glVertex2f(obj.x - 5, obj.y + 5);
    glEnd();
}



void drawCircle(int x, int y, float r) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    GLUquadric* quadObj = gluNewQuadric();
    gluDisk(quadObj, 0, r, 50, 50);
    glPopMatrix();
}


void drawRect(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawPlayer() {
    glPushMatrix();

    // Translate to player's position
    glTranslatef(player.x, player.y, 0.0f);

    // Rotate the player
    glRotatef(player.rotation, 0.0f, 0.0f, 1.0f);

    // Set player's color
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for visibility

    // Draw player as a triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 10.0f);  // top vertex
    glVertex2f(-5.0f, -5.0f);  // bottom-left vertex
    glVertex2f(5.0f, -5.0f);  // bottom-right vertex
    glEnd();

    glPopMatrix();
}

// ... other methods ...

void movePlayerUp() {
    player.y += PLAYER_SPEED;
    player.rotation = 0.0f;  // Pointing upwards
}

void movePlayerDown() {
    player.y -= PLAYER_SPEED;
    player.rotation = 180.0f;  // Pointing downwards
}

void movePlayerLeft() {
    player.x -= PLAYER_SPEED;
    player.rotation = 270.0f;  // Pointing left
}

void movePlayerRight() {
    player.x += PLAYER_SPEED;
    player.rotation = 90.0f;  // Pointing right
}


///////////////////////////////////////////
void resetGameState() {
    // Reset all game states to their initial values
    lives = 5;
    score = 0;
    gameTime = 500;
    powerUp1Active = false;
    powerUp1Timer = 0;
    powerUp2Active = false;
    powerUp2Timer = 0;

    // TODO: Reset player position to the center of the screen

    // TODO: Reposition obstacles, collectables, and power-ups for a new game run

    // TODO: Set game state to running or any other state you have for active gameplay
}

void drawEndScreen(bool isWin) {
    glClear(GL_COLOR_BUFFER_BIT);  // Clear the screen

    // TODO: Set a background color or image if you'd like

    glColor3f(1.0, 1.0, 1.0);  // Set text color to white

    if (isWin) {
        // Display "You Win!" on the screen
        glRasterPos2i(screenWidth / 2 - 50, screenHeight / 2);
        const char* winText = "You Win!";
        while (*winText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *winText++);
        }
    }
    else {
        // Display "Game Over!" on the screen
        glRasterPos2i(screenWidth / 2 - 60, screenHeight / 2);
        const char* loseText = "Game Over!";
        while (*loseText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *loseText++);
        }
    }

    // Optionally, add a button or prompt to restart or go back to the main menu
    // For simplicity, let's assume pressing 'R' restarts the game
    glRasterPos2i(screenWidth / 2 - 100, screenHeight / 2 - 20);
    const char* restartText = "Press 'R' to restart";
    while (*restartText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *restartText++);
    }

    glutSwapBuffers();  // Swap buffers to display the end screen
}

void endGame(bool isWin) {
    resetGameState();
    drawEndScreen(isWin);

    // Pause or stop any ongoing game mechanics
    // TODO: Stop player movement, obstacle movement, etc.

    // Listen for the 'R' key to restart the game
    // NOTE: You should have a keyboard callback function where you can implement this.
}
void checkCollision() {
    // 1. Collision with boundaries
    if (player.x - 20 < 0 || player.x + 20 > screenWidth ||
        player.y - 20 < 0 || player.y + 20 > screenHeight) {
        // Player collided with the boundaries
        lives--;  // Lose a life
        player.x = screenWidth / 2;  // Reset player's position
        player.y = screenHeight / 2;
    }

    // 2. Collision with obstacles
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        float dx = player.x - obstacles[i].x;
        float dy = player.y - obstacles[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25) {  // Considering 20 as player's radius and 5 as half of obstacle's side length
            // Collision detected with an obstacle
            lives--;  // Lose a life
            obstacles[i].x = rand() % screenWidth;  // Reposition the obstacle
            obstacles[i].y = rand() % screenHeight;
        }
    }

    // 3. Collision with collectables
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        float dx = player.x - collectables[i].x;
        float dy = player.y - collectables[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25) {  // Considering 20 as player's radius and 5 as half of collectable's side length
            // Collision detected with a collectable
            score += 10;  // Add to score
            collectables[i].x = rand() % screenWidth;  // Reposition the collectable
            collectables[i].y = rand() % screenHeight;
        }
    }

    // 4. Collision with power-ups
    for (int i = 0; i < NUM_POWERUPS; i++) {
        float dx = player.x - powerUps[i].x;
        float dy = player.y - powerUps[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25) {  // Considering 20 as player's radius and 5 as half of power-up's side length
            // Collision detected with a power-up
            if (i == 0) {  // First power-up (assuming two different power-ups for simplicity)
                powerUp1Active = true;
                powerUp1Timer = 0;
                // Here, you can implement the effects of this power-up
                // For example, increasing the player's speed
            }
            else {  // Second power-up
                powerUp2Active = true;
                powerUp2Timer = 0;
                // Here, you can implement the effects of this power-up
                // For example, making the player invincible for a short time
            }

            powerUps[i].x = rand() % screenWidth;  // Reposition the power-up
            powerUps[i].y = rand() % screenHeight;
        }
    }
}


void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
    case 'R':
    case 'r':
        resetGameState();
        break;
    case 'W':
    case 'w':
        movePlayerUp();
        break;
    case 'S':
    case 's':
        movePlayerDown();
        break;
    case 'A':
    case 'a':
        movePlayerLeft();
        break;
    case 'D':
    case 'd':
        movePlayerRight();
        break;
    default:
        break;
    }  checkCollision();  // Check collisions after every move

    glutPostRedisplay();
}

/////////////////////////////

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(playerX, playerY, 0);
    glRotatef(playerRotation, 0, 0, 1); // Rotate the player in the direction of motion
    // Draw Player
    glColor3f(0, 0, 1);
    drawCircle(0, 0, 20);
    glPopMatrix();

    // Drawing the player
    drawPlayer();

    // Drawing obstacles (in red for clarity)
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        drawGameObject(obstacles[i], 1.0, 0.0, 0.0);
    }

    // Drawing collectables (in green for clarity)
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        drawGameObject(collectables[i], 0.0, 1.0, 0.0);
    }

    // Drawing power-ups (in blue for clarity)
    for (int i = 0; i < NUM_POWERUPS; i++) {
        drawGameObject(powerUps[i], 0.0, 0.0, 1.0);
    }

    // Drawing the health bar (5 units for 5 lives)
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < lives; i++) {
        glRectf(10 + (i * 20), screenHeight - 20, 30 + (i * 20), screenHeight - 10);
    }

    // TODO: Display score and remaining game time
    // For a real game, consider using bitmap fonts or other text rendering method

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    // TODO: If the game ends, display the end screen here
    // and possibly reset the game or go back to the main menu.

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        movePlayerUp();
        break;
    case GLUT_KEY_DOWN:
        movePlayerDown();
        break;
    case GLUT_KEY_LEFT:
        movePlayerLeft();
        break;
    case GLUT_KEY_RIGHT:
        movePlayerRight();
        break;
    }

    checkCollision();  // Check collisions after every move

    glutPostRedisplay();
}

void timer(int value) {
    // Decrease remaining game time
    if (gameTime > 0) {
        gameTime--;
    }
    else {
        endGame(false); // Player ran out of time
        return;
    }

    // Deactivate power-ups after they've been active for a certain amount of time
    if (powerUp1Active) {
        powerUp1Timer++;
        if (powerUp1Timer >= powerUpDuration) {
            powerUp1Active = false;
            powerUp1Timer = 0; // Reset the timer
            // Effects of powerUp1: For example, let's assume it doubles the player's speed
        //    playerSpeed /= 2;  // Revert to the original speed
        }
    }

    if (powerUp2Active) {
        powerUp2Timer++;
        if (powerUp2Timer >= powerUpDuration) {
            powerUp2Active = false;
            powerUp2Timer = 0; // Reset the timer
            // Effects of powerUp2: For example, let's assume it makes the player invincible
        //    playerInvincibility = false;  // Turn off invincibility
        }
    }

    // Check if player ran out of lives
    if (lives <= 0) {
        endGame(false); // Player lost all lives
        return;
    }

    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);  // Call timer every 1 second
}


int main(int argc, char** argr) {
    glutInit(&argc, argr);

    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Top-View Game");

    glutDisplayFunc(display);
    /////////////
    generateRandomGameObjects();
    ///////////
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glClearColor(1, 1, 1, 0);
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glutMainLoop();

    return 0;
}

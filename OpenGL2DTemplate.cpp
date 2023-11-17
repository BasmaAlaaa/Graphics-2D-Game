#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <stdlib.h>
#include <time.h>

// Global Variables
float playerX = 400, playerY = 400;
float playerRotation = 0.0;
const int screenWidth = 800, screenHeight = 600;
int powerUp1Timer = 0;
int powerUp2Timer = 0;
int powerUpDuration = 5;
int lives = 5;
int score = 0;
int fixedGameTime = 40;
int gameTime = 40;
int PLAYER_SPEED = 10;
bool gameRunning = true;
bool isWin = false;
bool powerUp1Active = false, powerUp2Active = false;
float globalTime = 0.0f;
int triangleSize = 15;
float rotationAngle = 0.0f;
float goalX = 25.0f;
float goalY = 25.0f;
float goalSize = 50.0f;
const float BOUNDARY_WIDTH = 50.0f;
const float OBJECT_RADIUS = 40.0f;
float offset = 5.0f;
struct Player
{
    float x;
    float y;
    float rotation;
};
Player player = { screenWidth / 2, screenHeight / 2, 0.0f };
struct GameObject
{
    float x;
    float y;
};
const int NUM_OBSTACLES = 7;
const int NUM_COLLECTABLES = 7;
const int NUM_POWERUPS = 1;
GameObject obstacles[NUM_OBSTACLES];
GameObject collectables[NUM_COLLECTABLES];
GameObject powerUps1[NUM_POWERUPS];
GameObject powerUps2[NUM_POWERUPS];
float collectiblesRotationAngles[NUM_COLLECTABLES] = { 0 };
const float ROTATION_SPEED = 10.0f;
bool isOverlappingHouse(float x, float y)
{
    float houseWidth = 100.0f;
    float houseHeight = 100.0f;
    float houseStartX = 10.0f;
    float houseStartY = 10.0f;
    float totalHouseHeight = houseHeight + 50.0f;
    return x > houseStartX && x < houseStartX + houseWidth &&
        y > houseStartY && y < houseStartY + totalHouseHeight;
}


void update(float deltaTime)
{
    globalTime += deltaTime;
}
float colorValue = (sin(globalTime / 5.0f) + 1) / 2;
void drawBackground()
{ // Oscillate between 0 and 1 over 5 seconds
    float colorValue = (sin(globalTime / 5.0f) + 1) / 2;
    // gradient quad filling the whole background
    glBegin(GL_QUADS);

    glColor3f(colorValue, 0.5f, 1.0f - colorValue);
    glVertex2f(0, screenHeight);

    glColor3f(colorValue, 0.5f, 1.0f - colorValue);
    glVertex2f(screenWidth, screenHeight);

    glColor3f(0.5f - colorValue, 0.5f, 0.5f + colorValue);
    glVertex2f(screenWidth, 0);

    glColor3f(0.5f - colorValue, 0.5f, 0.5f + colorValue);
    glVertex2f(0, 0);

    glEnd();
}

void rotatePoint(float cx, float cy, float angle, float& x, float& y)
{
    float s = sin(angle);
    float c = cos(angle);
    x -= cx;
    y -= cy;
    float xnew = x * c - y * s;
    float ynew = x * s + y * c;
    x = xnew + cx;
    y = ynew + cy;
}
// Prototype functions
void drawCircle(int x, int y, float radius);
void drawRect(int x, int y, int width, int height);
void checkCollision();
void generateRandomGameObjects();
void drawGameObject(const GameObject& obj, float r, float g, float b);
void drawPlayer();
void movePlayerUp();
void movePlayerDown();
void movePlayerLeft();
void movePlayerRight();
void resetGameState();
void drawEndScreen(bool isWin);
void endGame(bool isWin);
void keyboardFunc(unsigned char key, int x, int y);
void display();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void timer(int value);
void displayText(float x, float y, const char* string);
void drawText(const char* str, int x, int y, void* font);
int getTextWidth(const char* str, void* font);
void drawObstacles(const GameObject& obj, float r, float g, float b);
void drawCollectibles(const GameObject& obj, float r, float g, float b);
void drawCollectibles2(const GameObject& obj);
void drawGoal();
void drawSceneBoundaries();

void drawSceneBoundaries()
{
    float offset = 10.0f; // distance between the parallel lines
    glColor3f(0.8, 0.8, 1.0);
    glBegin(GL_LINES);

    // Top boundary - Line 1
    glVertex2f(0.0f, offset);
    glVertex2f(screenWidth, offset);

    // Top boundary - Line 2
    glVertex2f(0.0f, 0.0f + 2 * offset);
    glVertex2f(screenWidth, 0.0f + 2 * offset);

    // Bottom boundary - Line 1
    glVertex2f(0.0f, screenHeight - offset);
    glVertex2f(screenWidth, screenHeight - offset);

    // Bottom boundary - Line 2
    glVertex2f(0.0f, screenHeight - 2 * offset);
    glVertex2f(screenWidth, screenHeight - 2 * offset);

    // Left boundary - Line 1
    glVertex2f(offset, 0.0f);
    glVertex2f(offset, screenHeight);

    // Left boundary - Line 2
    glVertex2f(2 * offset, 0.0f);
    glVertex2f(2 * offset, screenHeight);

    // Right boundary - Line 1
    glVertex2f(screenWidth - offset, 0.0f);
    glVertex2f(screenWidth - offset, screenHeight);

    // Right boundary - Line 2
    glVertex2f(screenWidth - 2 * offset, 0.0f);
    glVertex2f(screenWidth - 2 * offset, screenHeight);

    glEnd();
    // Draw triangles for the top and bottom boundaries
    for (float x = 0.0f; x <= screenWidth - triangleSize; x += triangleSize)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(x, offset);
        glVertex2f(x + triangleSize, offset);
        glVertex2f(x + triangleSize / 2, 2 * offset);
        glVertex2f(x, screenHeight - offset);
        glVertex2f(x + triangleSize, screenHeight - offset);
        glVertex2f(x + triangleSize / 2, screenHeight - 2 * offset);
        glEnd();
    }

    // Draw triangles for the left and right boundaries
    for (float y = 0.0f; y <= screenHeight - triangleSize; y += triangleSize)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(offset, y);
        glVertex2f(offset, y + triangleSize);
        glVertex2f(2 * offset, y + triangleSize / 2);
        glVertex2f(screenWidth - offset, y);
        glVertex2f(screenWidth - offset, y + triangleSize);
        glVertex2f(screenWidth - 2 * offset, y + triangleSize / 2);
        glEnd();
    }
}

bool isOverlapping(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx * dx + dy * dy) < 2 * OBJECT_RADIUS;
}
void generateRandomGameObjects()
{
    srand(time(NULL)); // Seed for random numbers

    auto placeObject = [&](GameObject& object)
        {
            bool overlap;
            do
            {
                object.x = 2 * offset + OBJECT_RADIUS + (rand() / (float)RAND_MAX) * (screenWidth - 4 * offset - 2 * OBJECT_RADIUS);
                object.y = 2 * offset + OBJECT_RADIUS + (rand() / (float)RAND_MAX) * (screenHeight - 4 * offset - 2 * OBJECT_RADIUS);

                overlap = false;

                for (int i = 0; i < NUM_OBSTACLES; i++)
                {
                    if (&object != &obstacles[i] && isOverlapping(object.x, object.y, obstacles[i].x, obstacles[i].y))
                    {
                        overlap = true;
                        break;
                    }
                }

                if (!overlap)
                {
                    for (int i = 0; i < NUM_COLLECTABLES; i++)
                    {
                        if (&object != &collectables[i] && isOverlapping(object.x, object.y, collectables[i].x, collectables[i].y))
                        {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap)
                {
                    for (int i = 0; i < NUM_POWERUPS; i++)
                    {
                        if (&object != &powerUps1[i] && isOverlapping(object.x, object.y, powerUps1[i].x, powerUps1[i].y))
                        {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap)
                {
                    for (int i = 0; i < NUM_POWERUPS; i++)
                    {
                        if (&object != &powerUps2[i] && isOverlapping(object.x, object.y, powerUps2[i].x, powerUps2[i].y))
                        {
                            overlap = true;
                            break;
                        }
                    }
                }
                if (isOverlappingHouse(object.x, object.y))
                {
                    overlap = true;
                }
            } while (overlap);
        };

    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        placeObject(obstacles[i]);
    }

    for (int i = 0; i < NUM_COLLECTABLES; i++)
    {
        placeObject(collectables[i]);
    }

    for (int i = 0; i < NUM_POWERUPS; i++)
    {
        placeObject(powerUps1[i]);
        placeObject(powerUps2[i]);
    }
}

void drawHouse()
{
    float houseWidth = 100.0f;
    float houseHeight = 100.0f;
    float roofHeight = 50.0f;
    float doorWidth = 30.0f;
    float doorHeight = 50.0f;
    float windowWidth = 20.0f;
    float windowHeight = 30.0f;
    float houseStartX = 20.0f;
    float houseStartY = 20.0f;
    float oscillationAmount = 10.0f;
    houseStartX += oscillationAmount * sin(globalTime);
    // Main structure of the house
    glColor3f(0.8f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(houseStartX, houseStartY);
    glVertex2f(houseStartX + houseWidth, houseStartY);
    glVertex2f(houseStartX + houseWidth, houseStartY + houseHeight);
    glVertex2f(houseStartX, houseStartY + houseHeight);
    glEnd();
    // Roof
    glColor3f(0.5f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(houseStartX, houseStartY + houseHeight);
    glVertex2f(houseStartX + houseWidth, houseStartY + houseHeight);
    glVertex2f(houseStartX + houseWidth / 2, houseStartY + houseHeight + roofHeight);
    glEnd();
    // Door
    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(houseStartX + houseWidth / 2 - doorWidth / 2, houseStartY);
    glVertex2f(houseStartX + houseWidth / 2 + doorWidth / 2, houseStartY);
    glVertex2f(houseStartX + houseWidth / 2 + doorWidth / 2, houseStartY + doorHeight);
    glVertex2f(houseStartX + houseWidth / 2 - doorWidth / 2, houseStartY + doorHeight);
    glEnd();
    // Left window
    glColor3f(0.7f, 0.7f, 1.0f);
    float leftWindowStartX = houseStartX + houseWidth / 4 - windowWidth / 2;
    float windowStartY = houseStartY + houseHeight / 2 - windowHeight / 2 + 20;
    glBegin(GL_QUADS);
    glVertex2f(leftWindowStartX, windowStartY);
    glVertex2f(leftWindowStartX + windowWidth, windowStartY);
    glVertex2f(leftWindowStartX + windowWidth, windowStartY + windowHeight);
    glVertex2f(leftWindowStartX, windowStartY + windowHeight);
    glEnd();
    // Right window
    float rightWindowStartX = houseStartX + 3 * houseWidth / 4 - windowWidth / 2;
    glBegin(GL_QUADS);
    glVertex2f(rightWindowStartX, windowStartY);
    glVertex2f(rightWindowStartX + windowWidth, windowStartY);
    glVertex2f(rightWindowStartX + windowWidth, windowStartY + windowHeight);
    glVertex2f(rightWindowStartX, windowStartY + windowHeight);
    glEnd();

    glColor3f(0.4f, 0.2f, 0.1f);
    float lineY = houseStartY + houseHeight / 2 + 20; // Halfway up the house
    glBegin(GL_LINES);
    glVertex2f(houseStartX, lineY);
    glVertex2f(houseStartX + houseWidth, lineY);
    glEnd();
}
void drawObstacles(const GameObject& obj, float r, float g, float b)
{
    glColor3f(r, g, b);
    float radius = 20.0f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(obj.x, obj.y); // center of the circle
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(obj.x + cos(degInRad) * radius, obj.y + sin(degInRad) * radius);
    }
    glEnd();
    // eyes
    glColor3f(1.0f, 0.0f, 0.0f);

    float eyeRadius = 5.0f;    // Radius for the eyes
    float eyeDistance = 10.0f; // Distance between the two eyes and from the center

    // Left eye
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(obj.x - eyeDistance, obj.y); // Left of center
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(obj.x - eyeDistance + cos(degInRad) * eyeRadius, obj.y + sin(degInRad) * eyeRadius);
    }
    glEnd();

    // Right eye
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(obj.x + eyeDistance, obj.y); // Right of center
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(obj.x + eyeDistance + cos(degInRad) * eyeRadius, obj.y + sin(degInRad) * eyeRadius);
    }
    glEnd();

    glColor3f(r, g, b);
    //  circle on top-left
    float smallRadius = 6.0f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(obj.x - 25, obj.y + 25);
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f((obj.x - 7) + cos(degInRad) * smallRadius, (obj.y + 7) + sin(degInRad) * smallRadius);
    }
    glEnd();
    //  circle on top-right
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(obj.x + 25, obj.y + 25);
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f((obj.x + 7) + cos(degInRad) * smallRadius, (obj.y + 7) + sin(degInRad) * smallRadius);
    }
    glEnd();
}
void drawStar(float x, float y, float size)
{
    float doublePi = 2.0f * 3.14159f;
    float starPoints[10][2];

    for (int i = 0; i < 5; i++)
    {
        starPoints[i][0] = x + size * cos(i * doublePi / 5);
        starPoints[i][1] = y + size * sin(i * doublePi / 5);

        starPoints[i + 5][0] = x + (size / 2.5) * cos((i + 0.5) * doublePi / 5);
        starPoints[i + 5][1] = y + (size / 2.5) * sin((i + 0.5) * doublePi / 5);
    }

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 5; i++)
    {
        glVertex2f(starPoints[i][0], starPoints[i][1]);
        glVertex2f(starPoints[i + 5][0], starPoints[i + 5][1]);
    }
    glEnd();
}
void drawCollectible3(const GameObject& obj, float r, float g, float b, float rotationAngle)
{
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0.0f);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    float size = 50.0f;
    float halfSize = size / 2.0f;
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    // Top triangle
    glVertex2f(0, halfSize);
    glVertex2f(-halfSize, 0);
    glVertex2f(halfSize, 0);
    // Bottom triangle
    glVertex2f(0, -halfSize);
    glVertex2f(-halfSize, 0);
    glVertex2f(halfSize, 0);
    glEnd();

    float smallRadius = size / 8.0f;
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, halfSize);
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(cos(degInRad) * smallRadius, halfSize + sin(degInRad) * smallRadius);
    }
    glEnd();

    float midRadius = size / 4.0f;
    glColor3f(r * 0.75, g * 0.75, b * 0.75);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= 360; i += 10)
    {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(cos(degInRad) * midRadius, sin(degInRad) * midRadius);
    }
    glEnd();
    glPopMatrix();
}

void drawPowerUp1(const GameObject& obj, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0.0f);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-obj.x, -obj.y, 0.0f);
    glColor3f(r, g, b);
    drawStar(obj.x, obj.y, 30.0f);
    drawStar(obj.x, obj.y, 15.0f);
    drawStar(obj.x, obj.y, 10.0f);
    drawStar(obj.x, obj.y, 5.0f);

    glPopMatrix();
}

void drawCollectibles(const GameObject& obj, float r, float g, float b)
{
    glColor3f(r, g, b);

    // Square (centered around obj's position)
    float halfSide = 8.0f;
    glBegin(GL_QUADS);
    glVertex2f(obj.x - halfSide, obj.y - halfSide);
    glVertex2f(obj.x + halfSide, obj.y - halfSide);
    glVertex2f(obj.x + halfSide, obj.y + halfSide);
    glVertex2f(obj.x - halfSide, obj.y + halfSide);
    glEnd();

    // Triangle above the square
    float triangleHeight = halfSide * 1.5f;
    glBegin(GL_TRIANGLES);
    glVertex2f(obj.x, obj.y + halfSide + triangleHeight);
    glVertex2f(obj.x - halfSide, obj.y + halfSide);
    glVertex2f(obj.x + halfSide, obj.y + halfSide);
    glEnd();

    // Line below the square
    float halfLineWidth = 1.5f * halfSide;
    glBegin(GL_LINES);
    glVertex2f(obj.x - halfLineWidth, obj.y - halfSide - halfLineWidth);
    glVertex2f(obj.x + halfLineWidth, obj.y - halfSide - halfLineWidth);
    glEnd();
}
void drawGoal()
{
    float size = 50.0f;
    float centerX = size / 2;
    float centerY = size / 2;
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i++)
    {
        float rad = i * (3.14159 / 180.0f);
        glVertex2f(centerX + cos(rad) * (size / 2), centerY + sin(rad) * (size / 2));
    }
    glEnd();

    // Four lines crossing through the center
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(0, centerY);
    glVertex2f(size, centerY);

    // Vertical line
    glVertex2f(centerX, 0);
    glVertex2f(centerX, size);

    // Diagonal line from top-left to bottom-right
    glVertex2f(0, 0);
    glVertex2f(size, size);

    // Diagonal line from top-right to bottom-left
    glVertex2f(size, 0);
    glVertex2f(0, size);
    glEnd();
}

void drawGameObject(const GameObject& obj, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(obj.x - 5, obj.y - 5);
    glVertex2f(obj.x + 5, obj.y - 5);
    glVertex2f(obj.x + 5, obj.y + 5);
    glVertex2f(obj.x - 5, obj.y + 5);
    glEnd();
}

void drawCircle(float x, float y, float radius)
{
    float doublePi = 2.0f * 3.14159f;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i++)
    {
        float rad = i * (3.14159 / 180.0f);
        glVertex2f(x + cos(rad) * radius, y + sin(rad) * radius);
    }
    glEnd();
}

void drawPowerUp2(const GameObject& obj, float r, float g, float b)
{
    glPushMatrix();

    glTranslatef(obj.x, obj.y, 0.0f);           // Move to the object's position
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f); // Rotate
    glTranslatef(-obj.x, -obj.y, 0.0f);         // Move back by the same amount

    glColor3f(r, g, b);
    drawCircle(obj.x, obj.y, 20.0f);
    drawCircle(obj.x, obj.y, 15.0f);
    drawCircle(obj.x, obj.y, 10.0f);
    drawCircle(obj.x, obj.y, 5.0f);
    glPopMatrix();
}

void drawRect(int x, int y, int w, int h)
{
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawPlayer()
{
    glPushMatrix();
    // First, translate the player to its position
    glTranslatef(player.x, player.y, 0.0f);
    // Then, rotate the player around its own origin
    glRotatef(player.rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0, 1.0, 1.0);
    // Draw head
    glBegin(GL_QUADS);
    glVertex2f(-10, 20); // Top-left
    glVertex2f(10, 20);  // Top-right
    glVertex2f(10, 0);   // Bottom-right
    glVertex2f(-10, 0);  // Bottom-left
    glEnd();

    // Draw body
    glBegin(GL_POLYGON);
    glVertex2f(0, -5);    // Top-center
    glVertex2f(10, -15);  // Upper-right
    glVertex2f(7, -30);   // Lower-right
    glVertex2f(-7, -30);  // Lower-left
    glVertex2f(-10, -15); // Upper-left
    glEnd();
    // legs
    glBegin(GL_LINES);
    // Left leg
    glVertex2f(-5, -30); // Starting from bottom of the body
    glVertex2f(-5, -50); // End at some distance below

    // Right leg
    glVertex2f(5, -30); // Starting from bottom of the body
    glVertex2f(5, -50); // End at some distance below
    glEnd();

    // Draw eyes using GL_POINTS
    glColor3f(0.0, 0.0, 0.0); // White color for the eyes
    glPointSize(3.0);         // set point size
    glBegin(GL_POINTS);
    glVertex2f(-5, 10); // Left eye
    glVertex2f(5, 10);  // Right eye
    glEnd();

    glPopMatrix();
}
void DrawPlayer() {
    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);
    glRotatef(playerRotation, 0.0f, 0.0f, 1.0f);

    // Hasameeh Wall-E <3

    // Raso (Quad)
    glColor3f(0.5451f, 0.5529f, 0.5529f);
    glBegin(GL_QUADS);
    glVertex2f(playerX - 10, playerY + 30);
    glVertex2f(playerX + 10, playerY + 30);
    glVertex2f(playerX + 10, playerY + 40);
    glVertex2f(playerX - 10, playerY + 40);
    glEnd();

    // 3eeno (point)
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex2f(playerX - 5, playerY + 35); // Left eye
    glVertex2f(playerX + 5, playerY + 35); // Right eye
    glEnd();

    // Gesmo (quad)
    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(playerX - 10, playerY - 20);
    glVertex2f(playerX + 10, playerY - 20);
    glVertex2f(playerX + 10, playerY + 20);
    glVertex2f(playerX - 10, playerY + 20);
    glEnd();


    //Connection raso b gesmo (line)
    glLineWidth(2.0f);
    glColor3f(0.5451f, 0.5529f, 0.5529f);
    glBegin(GL_LINES);
    glVertex2f(playerX, playerY + 40);
    glVertex2f(playerX, playerY + 20);
    glEnd();

    // Regleeh (triangle)
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(playerX - 10, playerY - 20);
    glVertex2f(playerX + 10, playerY - 20);
    glVertex2f(playerX, playerY - 30);
    glEnd();

    // Dera3o (line)
    glLineWidth(5);
    glColor3f(0.5451f, 0.5529f, 0.5529f);
    glBegin(GL_LINES);
    // Left arm
    glVertex2f(playerX - 10, playerY);
    glVertex2f(playerX - 20, playerY - 10);
    // Right arm
    glVertex2f(playerX + 10, playerY);
    glVertex2f(playerX + 20, playerY - 10);
    glEnd();

    glPopMatrix();
}

void drawPlayer2()
{
    glPushMatrix();

    // Translate to player's position
    glTranslatef(player.x, player.y, 0.0f);

    // Rotate the player
    glRotatef(player.rotation, 0.0f, 0.0f, 1.0f);

    // Set player's color
    glColor3f(0.0f, 0.0f, 0.0f);

    // Draw player as a triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 10.0f);  // top vertex
    glVertex2f(-5.0f, -5.0f); // bottom-left vertex
    glVertex2f(5.0f, -5.0f);  // bottom-right vertex
    glEnd();

    glPopMatrix();
}

void movePlayerUp()
{
    player.y += PLAYER_SPEED;
    player.rotation = 0.0f; // Pointing upwards
}

void movePlayerDown()
{
    player.y -= PLAYER_SPEED;
    player.rotation = 180.0f; // Pointing downwards
}

void movePlayerLeft()
{
    player.x -= PLAYER_SPEED;
    player.rotation = 90.0f; // Pointing left
}

void movePlayerRight()
{
    player.x += PLAYER_SPEED;
    player.rotation = 270.0f; // Pointing right
}
void resetGameState()
{
    gameRunning = true;
    lives = 5;
    score = 0;
    gameTime = fixedGameTime;
    powerUp1Active = false;
    powerUp1Timer = 0;
    powerUp2Active = false;
    powerUp2Timer = 0;
    player.x = screenWidth / 2;
    player.y = screenHeight / 2;
    player.rotation = 0.0f;
}
void drawText(const char* str, int x, int y, void* font)
{
    glRasterPos2i(x, y);
    while (*str)
    {
        glutBitmapCharacter(font, *str++);
    }
}

int getTextWidth(const char* str, void* font)
{
    int width = 0;
    while (*str)
    {
        width += glutBitmapWidth(font, *str++);
    }
    return width;
}

void drawEndScreen(bool isWin)
{
    glClearColor(colorValue, 0.5f, 1.0f - colorValue, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // glColor3f(0.8, 0.8, 1.0);
    const char* displayText;
    void* font = GLUT_BITMAP_HELVETICA_18;
    if (isWin)
    {
        displayText = "You Win!!";
    }
    else
    {
        displayText = "Game Over!";
    }
    int textWidth = getTextWidth(displayText, font);
    drawText(displayText, (screenWidth - textWidth) / 2, screenHeight / 2, font);

    const char* restartText = "Press 'R' to restart";
    font = GLUT_BITMAP_HELVETICA_12;
    textWidth = getTextWidth(restartText, font);
    drawText(restartText, (screenWidth - textWidth) / 2, (screenHeight / 2) - 30, font);

    glutSwapBuffers();
}

void endGame(bool isWin)
{
    gameRunning = false;
    drawEndScreen(isWin);
}
void checkCollision()
{
    // 1. Collision with boundaries
    if (player.x - 20 < 0 || player.x + 20 > screenWidth ||
        player.y - 20 < 0 || player.y + 20 > screenHeight)
    {
        lives--;                    // Lose a life
        player.x = screenWidth / 2; // Reset player's position
        player.y = screenHeight / 2;
        player.rotation = 0.0f;
    }

    // 2. Collision with obstacles
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        float dx = player.x - obstacles[i].x;
        float dy = player.y - obstacles[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 30)
        {
            lives--; // Lose a life
            player.x = screenWidth / 2;
            player.y = screenHeight / 2;
            player.rotation = 0.0f;
        }
    }
    // 3. Collision with collectables
    for (int i = 0; i < NUM_COLLECTABLES; i++)
    {
        float dx = player.x - collectables[i].x;
        float dy = player.y - collectables[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        float jewelHalfSize = 25.0f;
        float playerRadius = 20.0f;

        if (distance < (playerRadius + jewelHalfSize))
        {
            // Collision detected with a collectable
            score += 10; // Add to score
            collectables[i].x = 2 * offset + OBJECT_RADIUS + (rand() / (float)RAND_MAX) * (screenWidth - 4 * offset - 2 * OBJECT_RADIUS);
            collectables[i].y = 2 * offset + OBJECT_RADIUS + (rand() / (float)RAND_MAX) * (screenHeight - 4 * offset - 2 * OBJECT_RADIUS);
        }
    }
    // 4. Collision with power-ups
    for (int i = 0; i < NUM_POWERUPS; i++)
    {
        float dx = player.x - powerUps1[i].x;
        float dy = player.y - powerUps1[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25)
        {
            if (i == 0)
            {
                powerUp1Active = true;
                powerUp1Timer = 0;
                score = score + 50;
            }
            powerUps1[i].x = -1000; // Reposition the power-up
            powerUps1[i].y = -1000;
        }
    }
    for (int i = 0; i < NUM_POWERUPS; i++)
    {
        float dx = player.x - powerUps2[i].x;
        float dy = player.y - powerUps2[i].y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25)
        {
            if (i == 0)
            {
                powerUp2Active = true;
                powerUp2Timer = 0;
                PLAYER_SPEED += 10;
            }
            powerUps2[i].x = -1000; // Reposition the power-up
            powerUps2[i].y = -1000;
        }
    }
    float houseWidth = 100.0f;
    float houseHeight = 100.0f;
    float houseStartX = 10.0f;
    float houseStartY = 10.0f;
    float playerRadius = 30.0f;
    bool collisionRight = (player.x - playerRadius < houseStartX + houseWidth);
    bool collisionLeft = (player.x + playerRadius > houseStartX);
    bool collisionTop = (player.y - playerRadius < houseStartY + houseHeight);
    bool collisionBottom = (player.y + playerRadius > houseStartY);
    if (collisionRight && collisionLeft && collisionTop && collisionBottom)
    {
        isWin = true;
        gameRunning = false;
        endGame(true); // Player collided with the house and wins
        return;
    }
    if (lives == 0)
    {
        endGame(false); // Player lost all lives
    }
}
void keyboardFunc(unsigned char key, int x, int y)
{
    if (!gameRunning && key == 'r')
    {
        resetGameState();
    }
    if (gameRunning)
    {
        switch (key)
        {
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
        }
        checkCollision();
    }
    glutPostRedisplay();
}
void timer(int value)
{
    if (!gameRunning)
    {
        endGame(false);
        return;
    }
    if (gameTime > 0)
    {
        gameTime--;
        update(1.0f);
    }
    else
    {
        gameRunning = false;
        endGame(false);
        return;
    }
    // Deactivate power-ups after they've been active for a certain amount of time
    if (powerUp1Active)
    {
        powerUp1Timer++;
        if (powerUp1Timer >= powerUpDuration)
        {
            powerUp1Active = false;
            powerUp1Timer = 0; // Reset the timer
        }
    }
    if (powerUp2Active)
    {
        powerUp2Timer++;
        if (powerUp2Timer >= powerUpDuration)
        {
            powerUp2Active = false;
            PLAYER_SPEED = 10;
            powerUp2Timer = 0;
        }
    }
    if (lives == 0)
    {
        endGame(false);
    }

    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0); // Call timer every 1 second
}
void displayText(float x, float y, const char* string)
{
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameRunning)
    {
        drawBackground();
        drawSceneBoundaries();
        // Drawing the player
        drawPlayer();
        globalTime += 0.03f; // adjust this value to change el speed of animation
        drawHouse();
        for (int i = 0; i < NUM_OBSTACLES; i++)
        {
            drawObstacles(obstacles[i], 0.0, 0.0, 0.0);
        }
        float z = (rand() % 10 + 1) / 10.0f;
        float zz = rand() % 5 + 5;

        for (int i = 0; i < NUM_COLLECTABLES; i++)
        {
            collectiblesRotationAngles[i] += 5.0f;
            if (collectiblesRotationAngles[i] >= 360.0f)
            {
                collectiblesRotationAngles[i] -= 360.0f;
            }
            drawCollectible3(collectables[i], 0.2, 0.9, 0.9, collectiblesRotationAngles[i]);
        }
        rotationAngle += 10.5f;
        if (rotationAngle >= 360.0f)
        {
            rotationAngle -= 360.0f; // Keep the angle between 0 and 360 degrees
        }
        drawPowerUp1(powerUps1[0], 1.0, 0.0, 1.0);
        drawPowerUp1(powerUps1[1], 0.5, 1.0, 0.0);
        drawPowerUp2(powerUps2[0], 0.5, 0.0, 1.0);
        // Drawing the health bar (5 units for 5 lives)
        glColor3f(0.0, 1.0, 0.0);
        for (int i = 0; i < lives; i++)
        {
            glRectf(10 + (i * 20) + 10, screenHeight - 20 - 10, 30 + (i * 20) + 10, screenHeight - 10 - 10);
        }
        // Display score
        char scoreString[50];
        sprintf(scoreString, "Score: %d", score);
        glColor3f(0.8, 0.8, 1.0);
        displayText(30, screenHeight - 50, scoreString);
        // Display time
        char timeString[50];
        int minutes = (int)(gameTime / 60);
        int seconds = (int)(gameTime) % 60;
        sprintf(timeString, "Time left: %02d:%02d", minutes, seconds);
        displayText(650, screenHeight - 50, timeString);
        // Display lives
        char livesString[50];
        sprintf(livesString, "Lives: %d", lives);
        displayText(350, screenHeight - 50, livesString);

        if (lives == 0)
        {
            endGame(false); // Player lost all lives
        }
    }
    else
    {
        drawEndScreen(isWin);
    }

    glFlush();
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
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
    checkCollision(); // Check collisions after every move
    glutPostRedisplay();
}

int main(int argc, char** argr)
{
    glutInit(&argc, argr);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Agmad Game");
    glutDisplayFunc(display);
    generateRandomGameObjects();
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glClearColor(1, 1, 1, 0);
    gluOrtho2D(0, screenWidth, 0, screenHeight);
    glutMainLoop();
    return 0;
}
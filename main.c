#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Canvas dimensions
#define ROWS 20
#define COLS 60
#define MAX_SHAPES 100

char canvas[ROWS][COLS];

// Structure to store drawing objects for adding, modifying, or deleting
typedef struct {
    int type; // 1: Line, 2: Rectangle, 3: Circle, 4: Triangle
    int x1, y1, x2, y2, x3, y3;
    int dimension1, dimension2; // Width, height, or radius
    char borderChar;
} GeometricShape;

// Array to act as our database of active objects (Replaces C++ Vector)
GeometricShape objectDatabase[MAX_SHAPES];
int shapeCount = 0;

// Requirement 3: Clear the 2D array and reset background/borders
void clearCanvas() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                canvas[i][j] = '#'; // Solid frame boundary
            } else {
                canvas[i][j] = ' '; // Empty space inside canvas
            }
        }
    }
}

// Helper function to safely plot a point within boundaries
void drawPixel(int x, int y, char ch) {
    if (x > 0 && x < COLS - 1 && y > 0 && y < ROWS - 1) {
        canvas[y][x] = ch;
    }
}

// Requirement 1: Draw a Line using Bresenham's Line Algorithm
void drawLine(int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        drawPixel(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Requirement 1: Draw a Rectangle by combining four straight lines
void drawRectangle(int x, int y, int width, int height, char ch) {
    drawLine(x, y, x + width, y, ch);               
    drawLine(x, y + height, x + width, y + height, ch); 
    drawLine(x, y, x, y + height, ch);               
    drawLine(x + width, y, x + width, y + height, ch); 
}

// Requirement 1: Draw a Circle using the Midpoint Circle Engine
void drawCircle(int xc, int yc, int r, char ch) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (y >= x) {
        drawPixel(xc + x, yc + y, ch);
        drawPixel(xc - x, yc + y, ch);
        drawPixel(xc + x, yc - y, ch);
        drawPixel(xc - x, yc - y, ch);
        drawPixel(xc + y, yc + x, ch);
        drawPixel(xc - y, yc + x, ch);
        drawPixel(xc + y, yc - x, ch);
        drawPixel(xc - y, yc - x, ch);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Requirement 1: Draw a Triangle by connecting three dynamic vertex points
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, char ch) {
    drawLine(x1, y1, x2, y2, ch);
    drawLine(x2, y2, x3, y3, ch);
    drawLine(x3, y3, x1, y1, ch);
}

// Render loop: Cycles through saved objects and plots them to array
void refreshCanvas() {
    clearCanvas();
    for (int i = 0; i < shapeCount; i++) {
        GeometricShape shape = objectDatabase[i];
        if (shape.type == 1) {
            drawLine(shape.x1, shape.y1, shape.x2, shape.y2, shape.borderChar);
        } else if (shape.type == 2) {
            drawRectangle(shape.x1, shape.y1, shape.dimension1, shape.dimension2, shape.borderChar);
        } else if (shape.type == 3) {
            drawCircle(shape.x1, shape.y1, shape.dimension1, shape.borderChar);
        } else if (shape.type == 4) {
            drawTriangle(shape.x1, shape.y1, shape.x2, shape.y2, shape.x3, shape.y3, shape.borderChar);
        }
    }
}

// Requirement 4: Print the current 2D array state cleanly onto console
void displayCanvas() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int userChoice = 0;
    
    while (userChoice != 4) {
        refreshCanvas();
        printf("\n============================================\n");
        printf("         INTERACTIVE 2D GRAPHICS EDITOR     \n");
        printf("============================================\n");
        displayCanvas();
        
        printf("\n[1] Add Shape  |  [2] Delete Last Shape  |  [3] Clear All  |  [4] Exit Editor\n");
        printf("Select an option: ");
        scanf("%d", &userChoice);
        
        if (userChoice == 1) {
            if (shapeCount >= MAX_SHAPES) {
                printf(">> Error: Shape database full!\n");
                continue;
            }
            
            GeometricShape newShape;
            printf("\nSelect Primitive: (1: Line, 2: Rectangle, 3: Circle, 4: Triangle): ");
            scanf("%d", &newShape.type);
            printf("Enter brush stroke character (e.g., *, _, @): ");
            scanf(" %c", &newShape.borderChar); // Space before %c clears newline buffer
            
            if (newShape.type == 1) {
                printf("Enter values (Start X Y & End X Y): ");
                scanf("%d %d %d %d", &newShape.x1, &newShape.y1, &newShape.x2, &newShape.y2);
            } else if (newShape.type == 2) {
                printf("Enter values (Top-Left corner X Y, Width, Height): ");
                scanf("%d %d %d %d", &newShape.x1, &newShape.y1, &newShape.dimension1, &newShape.dimension2);
            } else if (newShape.type == 3) {
                printf("Enter values (Center X Y & Radius size): ");
                scanf("%d %d %d", &newShape.x1, &newShape.y1, &newShape.dimension1);
            } else if (newShape.type == 4) {
                printf("Enter 3 Coordinate Points (X1 Y1, X2 Y2, X3 Y3): ");
                scanf("%d %d %d %d %d %d", &newShape.x1, &newShape.y1, &newShape.x2, &newShape.y2, &newShape.x3, &newShape.y3);
            }
            
            objectDatabase[shapeCount] = newShape;
            shapeCount++;
            
        } else if (userChoice == 2) {
            if (shapeCount > 0) {
                shapeCount--;
                printf(">> Last element successfully removed.\n");
            } else {
                printf(">> Canvas object buffer is empty.\n");
            }
        } else if (userChoice == 3) {
            shapeCount = 0;
            printf(">> Entire composition cleared.\n");
        }
    }
    
    printf("\nThank you for using 2D Graphics Editor. Exiting process...\n");
    return 0;
}
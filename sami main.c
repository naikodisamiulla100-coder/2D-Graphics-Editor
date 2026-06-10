/*
 * 2D Graphics Editor
 * Uses a 2D array of characters as the canvas.
 * Supports: Line, Rectangle, Circle, Triangle
 * Operations: Add, Delete, Modify, Display
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS       25       /* canvas height */
#define COLS       60       /* canvas width  */
#define MAX_OBJ    50       /* max shapes    */

/* ─────────── canvas ─────────── */
char canvas[ROWS][COLS];

void clear_canvas() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            canvas[i][j] = ' ';
}

/* safely set one character on the canvas */
void put(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

void display_canvas() {
    printf("\n+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");
    for (int i = 0; i < ROWS; i++) {
        printf("|");
        for (int j = 0; j < COLS; j++) printf("%c", canvas[i][j]);
        printf("|\n");
    }
    printf("+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");
}

/* ─────────── shape types ─────────── */
typedef enum { SHAPE_LINE, SHAPE_RECT, SHAPE_CIRCLE, SHAPE_TRIANGLE } ShapeType;

typedef struct {
    int       active;   /* 1 = alive, 0 = deleted */
    ShapeType type;
    int       r1, c1;   /* point 1  (or center for circle) */
    int       r2, c2;   /* point 2  (or width/height for rect) */
    int       r3, c3;   /* point 3  (triangle only) */
    int       radius;   /* circle only */
    char      sym;      /* '*' or '_' */
} Shape;

Shape shapes[MAX_OBJ];
int   num_shapes = 0;

/* ─────────── drawing algorithms ─────────── */

/* Bresenham line */
void draw_line(int r1, int c1, int r2, int c2, char ch) {
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    int sr = (r1 < r2) ? 1 : -1;
    int sc = (c1 < c2) ? 1 : -1;
    int err = dr - dc;
    while (1) {
        put(r1, c1, ch);
        if (r1 == r2 && c1 == c2) break;
        int e2 = 2 * err;
        if (e2 > -dc) { err -= dc; r1 += sr; }
        if (e2 <  dr) { err += dr; c1 += sc; }
    }
}

/* 4 sides */
void draw_rect(int r, int c, int w, int h, char ch) {
    for (int j = c; j < c + w; j++) { put(r, j, ch); put(r + h - 1, j, ch); }
    for (int i = r; i < r + h; i++) { put(i, c, ch); put(i, c + w - 1, ch); }
}

/* Midpoint circle */
void draw_circle(int cr, int cc, int rad, char ch) {
    int x = 0, y = rad, d = 3 - 2 * rad;
    while (y >= x) {
        put(cr+x, cc+y, ch); put(cr-x, cc+y, ch);
        put(cr+x, cc-y, ch); put(cr-x, cc-y, ch);
        put(cr+y, cc+x, ch); put(cr-y, cc+x, ch);
        put(cr+y, cc-x, ch); put(cr-y, cc-x, ch);
        if (d <= 0) d += 4*x + 6;
        else        { d += 4*(x - y) + 10; y--; }
        x++;
    }
}

/* 3 lines */
void draw_triangle(int r1,int c1,int r2,int c2,int r3,int c3,char ch) {
    draw_line(r1, c1, r2, c2, ch);
    draw_line(r2, c2, r3, c3, ch);
    draw_line(r3, c3, r1, c1, ch);
}

/* ─────────── redraw entire scene ─────────── */
void redraw() {
    clear_canvas();
    for (int i = 0; i < num_shapes; i++) {
        if (!shapes[i].active) continue;
        Shape *s = &shapes[i];
        switch (s->type) {
            case SHAPE_LINE:     draw_line(s->r1,s->c1,s->r2,s->c2,s->sym);                      break;
            case SHAPE_RECT:     draw_rect(s->r1,s->c1,s->r2,s->c2,s->sym);                      break;
            case SHAPE_CIRCLE:   draw_circle(s->r1,s->c1,s->radius,s->sym);                      break;
            case SHAPE_TRIANGLE: draw_triangle(s->r1,s->c1,s->r2,s->c2,s->r3,s->c3,s->sym);     break;
        }
    }
}

/* ─────────── object management ─────────── */
int add_shape(Shape s) {
    if (num_shapes >= MAX_OBJ) { printf("Canvas full!\n"); return -1; }
    s.active = 1;
    shapes[num_shapes++] = s;
    redraw();
    return num_shapes - 1;
}

void delete_shape(int id) {
    if (id < 0 || id >= num_shapes || !shapes[id].active) {
        printf("Invalid ID.\n"); return;
    }
    shapes[id].active = 0;
    redraw();
    printf("Object %d deleted.\n", id);
}

void list_shapes() {
    printf("\n--- Object List ---\n");
    int any = 0;
    for (int i = 0; i < num_shapes; i++) {
        if (!shapes[i].active) continue;
        any = 1;
        Shape *s = &shapes[i];
        printf("[%d] ", i);
        switch (s->type) {
            case SHAPE_LINE:     printf("LINE     (%d,%d)->(%d,%d)            sym='%c'\n",s->r1,s->c1,s->r2,s->c2,s->sym);                break;
            case SHAPE_RECT:     printf("RECT     row=%d col=%d w=%d h=%d     sym='%c'\n",s->r1,s->c1,s->r2,s->c2,s->sym);               break;
            case SHAPE_CIRCLE:   printf("CIRCLE   center=(%d,%d) r=%d         sym='%c'\n",s->r1,s->c1,s->radius,s->sym);                  break;
            case SHAPE_TRIANGLE: printf("TRIANGLE (%d,%d),(%d,%d),(%d,%d)     sym='%c'\n",s->r1,s->c1,s->r2,s->c2,s->r3,s->c3,s->sym);  break;
        }
    }
    if (!any) printf("No objects yet.\n");
    printf("-------------------\n");
}

/* ─────────── input helpers ─────────── */
void flush() { int c; while ((c = getchar()) != '\n' && c != EOF); }

char get_sym() {
    char ch;
    printf("Symbol (* or _): ");
    scanf(" %c", &ch); flush();
    return ch;
}

/* ─────────── main menu ─────────── */
int main() {
    clear_canvas();
    int choice;

    while (1) {
        printf("\n====== 2D Graphics Editor ======\n");
        printf(" 1. Add Line\n");
        printf(" 2. Add Rectangle\n");
        printf(" 3. Add Circle\n");
        printf(" 4. Add Triangle\n");
        printf(" 5. Delete Object\n");
        printf(" 6. Modify Object\n");
        printf(" 7. Display Canvas\n");
        printf(" 8. List Objects\n");
        printf(" 0. Exit\n");
        printf("================================\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { flush(); continue; }
        flush();

        Shape s = {0};
        int id;

        switch (choice) {

            /* ── Exit ── */
            case 0:
                printf("Bye!\n");
                return 0;

            /* ── Add Line ── */
            case 1:
                printf("Enter  row1 col1 row2 col2  (e.g.  5 5 5 30): ");
                scanf("%d %d %d %d", &s.r1,&s.c1,&s.r2,&s.c2); flush();
                s.sym  = get_sym();
                s.type = SHAPE_LINE;
                id = add_shape(s);
                printf("Line added as object #%d\n", id);
                display_canvas();
                break;

            /* ── Add Rectangle ── */
            case 2:
                printf("Enter  row col width height  (e.g.  3 10 20 8): ");
                scanf("%d %d %d %d", &s.r1,&s.c1,&s.r2,&s.c2); flush();
                s.sym  = get_sym();
                s.type = SHAPE_RECT;
                id = add_shape(s);
                printf("Rectangle added as object #%d\n", id);
                display_canvas();
                break;

            /* ── Add Circle ── */
            case 3:
                printf("Enter  center_row center_col radius  (e.g.  12 30 8): ");
                scanf("%d %d %d", &s.r1,&s.c1,&s.radius); flush();
                s.sym  = get_sym();
                s.type = SHAPE_CIRCLE;
                id = add_shape(s);
                printf("Circle added as object #%d\n", id);
                display_canvas();
                break;

            /* ── Add Triangle ── */
            case 4:
                printf("Enter  r1 c1 r2 c2 r3 c3  (e.g.  2 30 18 5 18 55): ");
                scanf("%d %d %d %d %d %d", &s.r1,&s.c1,&s.r2,&s.c2,&s.r3,&s.c3); flush();
                s.sym  = get_sym();
                s.type = SHAPE_TRIANGLE;
                id = add_shape(s);
                printf("Triangle added as object #%d\n", id);
                display_canvas();
                break;

            /* ── Delete ── */
            case 5:
                list_shapes();
                printf("Enter ID to delete: ");
                scanf("%d", &id); flush();
                delete_shape(id);
                display_canvas();
                break;

            /* ── Modify ── */
            case 6: {
                list_shapes();
                printf("Enter ID to modify: ");
                scanf("%d", &id); flush();
                if (id < 0 || id >= num_shapes || !shapes[id].active) {
                    printf("Invalid ID.\n"); break;
                }
                Shape *sp = &shapes[id];
                sp->sym = get_sym();
                switch (sp->type) {
                    case SHAPE_LINE:
                        printf("New  row1 col1 row2 col2: ");
                        scanf("%d %d %d %d", &sp->r1,&sp->c1,&sp->r2,&sp->c2); flush();
                        break;
                    case SHAPE_RECT:
                        printf("New  row col width height: ");
                        scanf("%d %d %d %d", &sp->r1,&sp->c1,&sp->r2,&sp->c2); flush();
                        break;
                    case SHAPE_CIRCLE:
                        printf("New  center_row center_col radius: ");
                        scanf("%d %d %d", &sp->r1,&sp->c1,&sp->radius); flush();
                        break;
                    case SHAPE_TRIANGLE:
                        printf("New  r1 c1 r2 c2 r3 c3: ");
                        scanf("%d %d %d %d %d %d", &sp->r1,&sp->c1,&sp->r2,&sp->c2,&sp->r3,&sp->c3); flush();
                        break;
                }
                redraw();
                printf("Object %d modified.\n", id);
                display_canvas();
                break;
            }

            /* ── Display ── */
            case 7:
                display_canvas();
                break;

            /* ── List ── */
            case 8:
                list_shapes();
                break;

            default:
                printf("Invalid choice, try again.\n");
        }
    }
    return 0;
}
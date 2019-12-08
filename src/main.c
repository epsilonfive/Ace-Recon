// Program Name: Ace Recon
// Author(s): epsilon5
// Description: CC23 entry

// [disjax] [Zeda] Reserve space for an array. Keep some reference to the number of bullets. Add an element by incrementing the number of bullets and appending to the end
//if there is enough room. Remove a bullet by swapping it with the last element and decrementing the number of bullets

//1:44:41 PM [#] [MateoC] 10:42 <MateoC> link each list to each node initially
//1:44:41 PM [#] [MateoC] 10:40 <MateoC> well you would just have a node pool

/*1:48:47 PM [#] [disjax] [c4ooo] so the data for the deleted element is gone
1:48:39 PM [#] [disjax] [c4ooo] then decrease the list size by 1
1:48:29 PM [#] [disjax] [c4ooo] you overwrite the deleted element with the last element in the list
1:48:13 PM [#] [disjax] [c4ooo] no
*/

//NEED TO DO PARTIAL REDRAW AND STUFF

/* Keep these headers */
#define FORCE_INTERRUPTS
#include <tice.h>

/* Standard headers - it's recommended to leave them included */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* Other available headers */
// including stdarg.h, setjmp.h, assert.h, ctype.h, float.h, iso646.h, limits.h, errno.h
/* Available libraries */
// including lib/ce/graphc.h, lib/ce/fileioc.h, and lib/ce/keypadc.h
// Sample: #include <lib/ce/graphc.h>
#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <intce.h>
#include <tice.h>
#include <math.h>
#define fabsf
/* Put your function prototypes here */

//variables definitions
char Appvars[9][10];
char currpackname[9];
char search_String[] = "ACE";
short numlevels = 20;
float plx = 160;
float ply = 120;
float plangle = 0;
bool canfire = true;
short tilefound = 0;
bool playerseen = false;
int startx;
int starty;
bool playeralive = true;
short playerlives = 5;
bool levelpassed = false;
char menutext[70];
short stringlength;
short numguards = 5;
int coins = 0;
int guardselim = 0;
int speed = 3;
float rotation = 0.1;
short gamemode;
bool ownedbomb;
bool ownedwindowstealth;
bool bounce;
char description[25];
char descriptionb[25];
char descriptionc[25];
char descriptiond[25];
//matrix for map data storage
uint8_t MapData[14][10];
short methodofdeath;
int numbullets;
gfx_sprite_t *playersprite;
gfx_sprite_t *guardspritea;
gfx_sprite_t *guardspriteb;
gfx_sprite_t *guardspritec;
gfx_sprite_t *bulletsprite;


//bullets structure
typedef struct {
    float x;
    float y;
    float xinc;
    float yinc;
    bool bounced;
    float angle;
    gfx_sprite_t *spr;
} bullet_t;
bullet_t bullet[20];


typedef struct {
    bool owned;
    short price;
    gfx_sprite_t *icon;
} item_t;
item_t item[8];

typedef struct {
    uint16_t x;
    uint16_t y;
    float angle;
    float attangle;
    bool knockedout;
    short lastfired;
    short type;
    bool patrolling;
    bool collected;
} guard_t;
guard_t guard[5];

void BlaText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);
    gfx_SetTextTransparentColor(0);
}
void WhiText(void) {
    gfx_SetTextBGColor(255);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(255);
}
void ErrorMessage(char str1[10], char str2[50]) {
    short length;
    length = gfx_GetStringWidth(str2);
    gfx_Blit(1);
    gfx_SetColor(0);
    gfx_FillRectangle(155 - length / 2, 98, length + 10, 45);
    gfx_SetColor(74);
    gfx_Rectangle(155 - length / 2, 98, length + 10, 45);
    gfx_Rectangle(156 - length / 2, 99, length + 8, 43);
    gfx_SetTextScale(2, 2);
    length = gfx_GetStringWidth(str1);
    gfx_PrintStringXY(str1, 160 - length / 2, 103);
    gfx_SetTextScale(1, 1);
    length = gfx_GetStringWidth(str2);
    gfx_PrintStringXY(str2, 160 - length / 2, 125);
    gfx_SwapDraw();
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
    }
}

//detection stuff
short findPacks() {
    char *varName;
    uint8_t *searchPos = NULL;
    ti_var_t slot;
    short count = 0;
    ti_CloseAll();
    while ((varName = ti_Detect(&searchPos, search_String)) != NULL) {
        ti_CloseAll();
        if ((slot = ti_Open(varName, "r"))) {
            strcpy(Appvars[count], varName);
            ti_SetArchiveStatus(true, slot);
        }
        ti_CloseAll();
        count++;
        if (count > 10) {
            break;
        }
    }
    return count;
}





void showPacks() {
    uint8_t x;
    ti_var_t slota;
    uint8_t y;
    uint8_t pltile;
    uint8_t tile;
    char str1[] = "this is some buffer text";
    uint8_t plx;
    uint8_t current = 0;
    WhiText();
    gfx_Blit(0);
    y = findPacks();
    x = 30 + y * 15;
    pltile = 120 - x / 2;
    while(!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        gfx_SetColor(0);
        gfx_FillRectangle(40, pltile, 240, x);
        BlaText();
        gfx_SetTextScale(2, 2);
        strcpy(str1, "Select a pack");
        tile = gfx_GetStringWidth(str1);
        gfx_PrintStringXY(str1, 160 - tile / 2, pltile + 5);
        gfx_SetTextScale(1, 1);
        for (plx = 0; plx < y; plx++) {
            strcpy(str1, Appvars[plx]);
            tile = gfx_GetStringWidth(str1);
            gfx_PrintStringXY(str1, 160 - tile / 2, (plx * 15) + pltile + 27);
        }
        if ((kb_Data[7] & kb_Down) && (current < y - 1)) {
            current++;
            while (kb_Data[7] & kb_Down) {
                kb_Scan();
            }
        }
        if ((kb_Data[7] & kb_Up) && (current > 0)) {
            current--;
            while (kb_Data[7] & kb_Up) {
                kb_Scan();
            }
        }
        if (kb_Data[6] & kb_Clear) {
            break;
        }
        gfx_SetColor(7);
        gfx_Rectangle(40, current * 15 + pltile + 24, 240, 14);
        gfx_Rectangle(40, current * 15 + pltile + 25, 240, 12);
        //outside of box
        strcpy(currpackname, Appvars[current]);
        ti_CloseAll();
        slota = ti_Open(currpackname, "r");
        ti_Seek(4, 0, slota);
        ti_Read(&numlevels, 1, 1, slota);
        gfx_SetColor(74);
        gfx_Rectangle(40, pltile, 240, x);
        gfx_Rectangle(41, pltile + 1, 238, x - 2);
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        strcpy(str1, Appvars[current]);
        gfx_SetTextFGColor(7);
        tile = gfx_GetStringWidth(str1);
        gfx_PrintStringXY(str1, 160 - tile / 2, current * 15 + pltile + 27);
        gfx_SwapDraw();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
        gfx_SetColor(255);
        gfx_Rectangle(40, current * 15 + pltile + 24, 240, 14);
        gfx_Rectangle(40, current * 15 + pltile + 25, 240, 12);
        gfx_SetColor(74);
        gfx_Rectangle(40, pltile, 240, x);
        gfx_Rectangle(41, pltile + 1, 238, x - 2);
        gfx_SetTextFGColor(255);
        gfx_PrintStringXY(str1, 160 - tile / 2, (current * 15) + pltile + 27);
        gfx_SwapDraw();
    }
}





void GetMenuTextB(short current) {
    if (current == 1) {
        strcpy(menutext, "Speedy");
        strcpy(description, "Increase player");
        strcpy(descriptionb, "movement speed by");
        strcpy(descriptionc, "50%.");
    }
    if (current == 2) {
        strcpy(menutext, "Windmill");
        strcpy(description, "Increase player");
        strcpy(descriptionb, "rotation speed by");
        strcpy(descriptionc, "50%.");
    }
    if (current == 3) {
        strcpy(menutext, "Bounce");
        strcpy(description, "Enables the player to");
        strcpy(descriptionb, "bounce bullets off");
        strcpy(descriptionc, "the edge of the");
        strcpy(descriptiond, "screen.");
    }
    if (current == 4) {
        strcpy(menutext, "Hitbox");
        strcpy(description, "Enemy hitbox size");
        strcpy(descriptionb, "increased by 50%.");
    }
    if (current == 5) {
        strcpy(menutext, "Points");
        strcpy(description, "Increase points");
        strcpy(descriptionb, "scoring by 25%.");
    }
    if (current == 6) {
        strcpy(menutext, "Stealth");
        strcpy(description, "Guards will be unable");
        strcpy(descriptionb, "to see the player");
        strcpy(descriptionc, "through windows.");
    }
    if (current == 7) {
        strcpy(menutext, "Bomb");
        strcpy(description, "Enables the player to");
        strcpy(descriptionb, "place and activate a");
        strcpy(descriptionc, "remote bomb.");
    }
    if (current == 8) {
        strcpy(menutext, "Jammer");
        strcpy(description, "Jams the alarm during");
        strcpy(descriptionb, "countdown. Can only");
        strcpy(descriptionc, "be used once per");
        strcpy(descriptiond, "level.");
    }
}



void GetMenuText(int current) {
    if (current == 1) {
        strcpy(menutext, "Play");
    }
    if (current == 2) {
        strcpy(menutext, "About");
    }
    if (current == 3) {
        strcpy(menutext, "Statistics");
    }
    if (current == 4) {
        strcpy(menutext, "Options");
    }
    if (current == 5) {
        strcpy(menutext, "Exit");
    }
}

void CenterText(void) {
    stringlength = gfx_GetStringWidth(menutext);
    stringlength = 160 - stringlength / 2;
}

void RenderMap(int level) {
    gfx_sprite_t *spr;
    ti_var_t slota;
    ti_var_t slotb;
    uint8_t x;
    uint8_t y;
    uint8_t tile = 0;
    ti_CloseAll();
    slota = ti_Open(currpackname, "r");
    slotb = ti_Open("ACRCNSPR", "r");
    gfx_FillScreen(0);
    gfx_SetColor(74);
    gfx_Rectangle(19, 19, 282, 202);
    if ((slota) && (slotb)) {
        ti_Seek(206 * level + 5, 0, slota);
        ti_Read(&MapData, 140, 1, slota);
        ti_Read(&numguards, 1, 1, slota);
        for (x = 0; x < numguards; ++x) {
            guard_t *g = &guard[x];
            ti_Read(&g->type, 1, 1, slota);
            ti_Read(&g->x, 2, 1, slota);
            ti_Read(&g->y, 2, 1, slota);
            ti_Read(&g->angle, 4, 1, slota);
            ti_Read(&g->angle, 4, 1, slota);
            g->knockedout = false;
            g->collected = false;
            g->x = g->x + 15;
        }
        for(y = 0; y < 10; ++y) {
            for(x = 0; x < 14; ++x) {
                tile = MapData[x][y];
                ti_Seek(402 * tile, 0, slotb);
                spr = ti_GetDataPtr(slotb);
                gfx_Sprite(spr, x * 20 + 20, y * 20 + 20);
                if (tile == 5) {
                    startx = x * 20 + 30;
                    starty = y * 20 + 30;
                }
            }
        }
    }
}

int PauseMenu(bool menuwrapping) {
    short y = 0;
    gfx_Blit(0);
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        gfx_SetColor(0);
        gfx_FillRectangle(100, 80, 120, 76);
        gfx_SetTextFGColor(255);
        gfx_SetTextBGColor(0);
        gfx_SetTextTransparentColor(0);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Paused.");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 85);
        gfx_SetTextScale(1, 1);
        strcpy(menutext, "Resume");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 107);
        strcpy(menutext, "Restart");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 123);
        strcpy(menutext, "Quit");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 139);
        if (kb_Data[7] & kb_Down) {
            y++;
            while (kb_Data[7] & kb_Down) {
                kb_Scan();
            }
        }
        if (y > 2) {
            if (menuwrapping) {
                y = 0;
            } else {
                y = 2;
            }
        }
        if (kb_Data[7] & kb_Up) {
            y--;
            while (kb_Data[7] & kb_Up) {
                kb_Scan();
            }
        }
        if (y < 0) {
            if (menuwrapping) {
                y = 2;
            } else {
                y = 0;
            }
        }
        if (kb_Data[6] & kb_Clear) {
            y = 4;
            break;
        }
        gfx_SetColor(7);
        gfx_Rectangle(100, y * 16 + 104, 120, 14);
        gfx_Rectangle(101, y * 16 + 105, 118, 12);
        //outside of box
        gfx_SetColor(74);
        gfx_Rectangle(100, 80, 120, 76);
        gfx_Rectangle(101, 81, 118, 74);
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        if (y == 0) {
            strcpy(menutext, "Resume");
        }
        if (y == 1) {
            strcpy(menutext, "Restart");
        }
        if (y == 2) {
            strcpy(menutext, "Quit");
        }
        CenterText();
        gfx_SetTextFGColor(7);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
        gfx_SetColor(255);
        gfx_Rectangle(100, y * 16 + 104, 120, 14);
        gfx_Rectangle(101, y * 16 + 105, 118, 12);
        gfx_SetColor(74);
        gfx_Rectangle(100, 80, 120, 76);
        gfx_Rectangle(101, 81, 118, 74);
        gfx_SetTextFGColor(255);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    return y;
}


void FindTile(int x, int y) {
    tilefound = MapData[x / 20][y / 20];
}

bool TileBetween(float xa, float ya, float xb, float yb, bool patrolling) {
    uint16_t dist;
    uint24_t xcheck = 0;
    uint8_t ycheck = 0;
    float cangle = 0;
    float sangle = 0;
    float angle = 0;
    bool TileBetween = false;
    uint8_t i;
    dist = sqrt(((xa - xb) * (xa - xb)) + ((ya - yb) * (ya - yb)));
    angle = atan2(yb - ya, xb - xa);
    cangle = 15 * cos(angle);
    sangle = 15 * sin(angle);
    xcheck = xa;
    ycheck = ya;
    for(i = 1; i < (dist / 15); ++i) {
        xcheck = xcheck + cangle;
        ycheck = ycheck + sangle;
        tilefound = MapData[(xcheck - 20) / 20][(ycheck - 20) / 20];
        if (patrolling) {
            if (tilefound == 2) {
                TileBetween = true;
            }
        }
        if ((!(patrolling)) || (ownedwindowstealth)) {
            if ((tilefound >= 2) && (tilefound <= 4)) {
                TileBetween = true;
            }
        }
        if (TileBetween) {
            break;
        }
    }
    return TileBetween;
}


void FireBullet(int x, int y, float angle) {
    if (numbullets < 20) {
        bullet_t *b = &bullet[numbullets++];
        b->x = x + 10;
        b->y = y + 10;
        b->xinc = 8 * cos(angle);
        b->yinc = 8 * sin(angle);
        b->bounced = false;
        b->angle = angle;
        gfx_GetSprite(b->spr, b->x, b->y);
    }
}



void RenderGuards(void) {
    uint8_t i;
    int xcheck;
    int ycheck;
    for (i = 0; i < numguards; ++i) {
        guard_t *g = &guard[i];
        if ((!(g->knockedout)) && (g->patrolling)) {
            xcheck = g->x + 15 * cos(g->angle - 0.5) - 20;
            ycheck = g->y + 15 * sin(g->angle - 0.5) - 20;
            tilefound = MapData[xcheck / 20][ycheck / 20];
            if ((tilefound >= 2) || (xcheck >= 276) || (xcheck <= 4) || (ycheck >= 196) || (ycheck <= 4)) {
                g->angle = g->angle + 0.05;
            } else {
                g->x = g->x + 2 * cos(g->angle);
                g->y = g->y + 2 * sin(g->angle);
            }
            if (g->angle > 6.28) {
                g->angle = 0;
            }
            if (g->angle < 0) {
                g->angle = 6.28;
            }
            if ((fabsf((atan2(g->y - ply, g->x - plx) + 3.14) - g->angle)) <= 0.02) {
                if (!(TileBetween(plx, ply, g->x, g->y, g->patrolling))) {
                    playerseen = true;
                    g->patrolling = false;
                }
            }
        }
        if ((!(g->patrolling)) && (!(g->knockedout))) {
            if (!(TileBetween(plx, ply, g->x, g->y, g->patrolling))) {
                if (g->lastfired >= 10) {
                    g->attangle = atan2(g->y - ply, g->x - plx) + 3.14;
                }
                if (g->attangle < g->angle) {
                    if (fabsf(g->angle - g->attangle) >= 0.1) {
                        g->angle = g->angle - 0.05;
                    } else {
                        if (g->lastfired >= 10) {
                            FireBullet((g->x + 10) + 20 * cos(g->angle), (g->y + 10) + 20 * sin(g->angle), g->angle);
                            g->lastfired = 0;
                        } else {
                            g->lastfired++;
                        }
                    }
                } else {
                    if (fabsf(g->angle + g->attangle) >= 0.1) {
                        g->angle = g->angle + 0.05;
                    } else {
                        if (g->lastfired >= 10) {
                            FireBullet((g->x + 10) + 20 * cos(g->angle), (g->y + 10) + 20 * sin(g->angle), g->angle);
                            g->lastfired = 0;
                        } else {
                            g->lastfired++;
                        }
                    }
                }
            } else {
                g->patrolling = true;
            }
        }
        if (!(g->knockedout)) {
            gfx_RotatedScaledTransparentSprite_NoClip(guardspritea, g->x, g->y, g->angle * 41 + 64, 64);
        }
        if ((g->knockedout) && (!(g->collected))) {
            gfx_SetColor(167);
            gfx_FillCircle(g->x + 10, g->y + 10, 4);
            if ((abs(g->x - plx + 20) <= 9) && (abs(g->y - ply + 20) <= 9)) {
                coins++;
                g->collected = true;
            }
        }
    }
}



bool Confirm(void) {
    uint8_t y = 0;
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        gfx_SetColor(0);
        gfx_FillRectangle(0, 80, 320, 60);
        gfx_SetTextFGColor(255);
        gfx_SetTextBGColor(0);
        gfx_SetTextTransparentColor(0);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Are you sure?");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 85);
        gfx_SetTextScale(1, 1);
        strcpy(menutext, "Yeah.");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 107);
        strcpy(menutext, "No, take me back.");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 123);
        if ((kb_Data[7] & kb_Down) && (y < 1)) {
            y++;
            while (kb_Data[7] & kb_Down) {
                kb_Scan();
            }
        }
        if ((kb_Data[7] & kb_Up) && (y > 0)) {
            y--;
            while (kb_Data[7] & kb_Up) {
                kb_Scan();
            }
        }
        gfx_SetColor(7);
        gfx_Rectangle(0, y * 16 + 104, 320, 14);
        gfx_Rectangle(1, y * 16 + 105, 318, 12);
        //outside of box
        gfx_SetColor(74);
        gfx_Rectangle(0, 80, 320, 60);
        gfx_Rectangle(1, 81, 318, 58);
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        if (y == 0) {
            strcpy(menutext, "Yeah.");
        } else {
            strcpy(menutext, "No, take me back.");
        }
        CenterText();
        gfx_SetTextFGColor(7);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
        gfx_SetColor(255);
        gfx_Rectangle(0, y * 16 + 104, 320, 14);
        gfx_Rectangle(1, y * 16 + 105, 318, 12);
        gfx_SetColor(74);
        gfx_Rectangle(0, 80, 320, 60);
        gfx_Rectangle(1, 81, 318, 58);
        gfx_SetTextFGColor(255);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    if (y == 0) {
        return true;
    } else {
        return false;
    }
}

short GameMode(bool menuwrapping) {
    short y = 0;
    bool keypress = false;
    gfx_Blit(0);
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        gfx_SetColor(0);
        gfx_FillRectangle(0, 80, 320, 75);
        gfx_SetTextFGColor(255);
        gfx_SetTextBGColor(0);
        gfx_SetTextTransparentColor(0);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Select a game mode.");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 85);
        gfx_SetTextScale(1, 1);
        strcpy(menutext, "Classic");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 107);
        strcpy(menutext, "Chaos");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 123);
        strcpy(menutext, "Multiplayer");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 139);
        if ((kb_Data[7] & kb_Down) && (keypress)) {
            y++;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Up) && (keypress)) {
            y--;
            keypress = false;
        }
        if (y > 2) {
            if (menuwrapping) {
                y = 0;
            } else {
                y = 2;
            }
        }
        if (y < 0) {
            if (menuwrapping) {
                y = 2;
            } else {
                y = 0;
            }
        }
        if (!kb_AnyKey()) {
            keypress = true;
        }
        if (kb_Data[6] & kb_Clear) {
            y = 4;
            break;
        }
        gfx_SetColor(7);
        gfx_Rectangle(0, y * 16 + 104, 320, 14);
        gfx_Rectangle(1, y * 16 + 105, 318, 12);
        //outside of box
        gfx_SetColor(74);
        gfx_Rectangle(0, 80, 320, 75);
        gfx_Rectangle(1, 81, 318, 73);
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        if (y == 0) {
            strcpy(menutext, "Classic");
        }
        if (y == 1) {
            strcpy(menutext, "Chaos");
        }
        if (y == 2) {
            strcpy(menutext, "Multiplayer");
        }
        CenterText();
        gfx_SetTextFGColor(7);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
        gfx_SetColor(255);
        gfx_Rectangle(0, y * 16 + 104, 320, 14);
        gfx_Rectangle(1, y * 16 + 105, 318, 12);
        gfx_SetColor(74);
        gfx_Rectangle(0, 80, 320, 75);
        gfx_Rectangle(1, 81, 318, 73);
        gfx_SetTextFGColor(255);
        gfx_PrintStringXY(menutext, stringlength, y * 16 + 107);
        gfx_SwapDraw();
    }
    return y;
}



void ItemShop(void) {
    short y = 0;
    uint8_t i;
    uint8_t min = 0;
    uint8_t max = 7;
    short bulletspeed = 8;
    short playerspeed = 3;
    float rotationspeed = 0.1;
    float playerx = 240;
    float playery = 120;
    float playerangle = 0;
    bool found = true;
    short j;
    short las = 0;
    bool fire = true;
    typedef struct {
        float x;
        float y;
        float angle;
        bool valid;
    } bull_t;
    bull_t bull[5];
    item[0].price = 3;
    item[1].price = 3;
    item[2].price = 5;
    item[3].price = 8;
    item[4].price = 10;
    item[5].price = 10;
    item[6].price = 7;
    item[7].price = 7;
    while (!(kb_Data[6] & kb_Clear)) {
        item_t *t = &item[y];
        kb_Scan();
        if (y == 0) {
            playerspeed = 4;
        } else {
            playerspeed = 3;
        }
        if (y == 1) {
            rotationspeed = 0.15;
        } else {
            rotationspeed = 0.1;
        }
        gfx_ZeroScreen();
        WhiText();
        gfx_SetColor(255);
        for(i = 0; i < 5; i++) {
            gfx_SetPixel(randInt(0, 320), randInt(0, 240));
        }
        gfx_SetColor(7);
        gfx_SetTextScale(2, 2);
        gfx_SetTextFGColor(7);
        strcpy(menutext, "Recon Shop");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 5);
        if (y <= 2) {
            min = 2 - y;
        } else {
            min = 0;
        }
        if (y >= 5) {
            max = 10 - y;
        } else {
            max = 5;
        }
        gfx_SetTextScale(1, 1);
        for(i = min; i < max; ++i) {
            gfx_SetColor(0);
            gfx_FillRectangle(5 + i * 63, 195, 58, 20);
            if (i == 2) {
                gfx_SetColor(7);
            } else {
                gfx_SetColor(74);
            }
            gfx_Rectangle(6 + i * 63, 196, 56, 18);
            gfx_Rectangle(5 + i * 63, 195, 58, 20);
            GetMenuTextB((min - 1) + y + (i - min));
            stringlength = gfx_GetStringWidth(menutext);
            stringlength = 34 + i * 63 - (stringlength / 2);
            BlaText();
            gfx_PrintStringXY(menutext, stringlength, 200);
        }
        strcpy(description, " ");
        strcpy(descriptionb, " ");
        strcpy(descriptionc, " ");
        strcpy(descriptiond, " ");
        GetMenuTextB(y + 1);
        gfx_SetColor(0);
        gfx_FillRectangle(164, 27, 152, 163);
        gfx_FillRectangle(5, 45, 155, 63);
        gfx_FillRectangle(5, 113, 155, 77);
        gfx_SetColor(74);
        gfx_Rectangle(164, 27, 152, 163);
        gfx_Rectangle(165, 28, 150, 161);
        gfx_FillRectangle(5, 27, 155, 18);
        gfx_FillRectangle(164, 27, 152, 18);
        gfx_Rectangle(5, 45, 155, 63);
        gfx_Rectangle(6, 46, 153, 61);
        gfx_Rectangle(5, 113, 155, 77);
        gfx_Rectangle(6, 114, 153, 75);
        gfx_FillRectangle(5, 113, 155, 18);
        gfx_PrintStringXY(description, 10, 50);
        gfx_PrintStringXY(descriptionb, 10, 65);
        gfx_PrintStringXY(descriptionc, 10, 80);
        gfx_PrintStringXY(descriptiond, 10, 95);
        sprintf(menutext, "Costs: %d", t->price);
        gfx_PrintStringXY(menutext, 10, 136);
        sprintf(menutext, "Available coins: %d", coins);
        gfx_PrintStringXY(menutext, 10, 151);
        sprintf(menutext, "Own status: %d", t->owned);
        gfx_PrintStringXY(menutext, 10, 166);
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(7);
        gfx_SetTextTransparentColor(0);
        gfx_PrintStringXY("Description", 10, 33);
        gfx_PrintStringXY("Item information", 10, 119);
        gfx_PrintStringXY("Item preview", 169, 33);
        for (i = 0; i < 3; ++i) {
            gfx_SetColor(74);
            gfx_FillRectangle(i * 106.667 + 20, 225, 67, 15);
            gfx_FillTriangle(i * 106.667 + 20, 240, i * 106.667 + 20, 225, i * 106.667, 240);
            gfx_FillTriangle(i * 106.667 + 86.667, 240, i * 106.667 + 86.667, 225, i * 106.667 + 106.667, 240);
            //Rectangle(startx + 20, 225, width, 15);
            //Triangle(startx, 240, startx, 225, startx - 20, 240);
            //Triangle(startx + width, 240, startx + width, 225, startx + width + 20, 240)
            gfx_SetColor(255);
            gfx_FillRectangle(i * 106.667 + 22, 227, 63, 13);
            gfx_FillTriangle(i * 106.667 + 22, 240, i * 106.667 + 22, 227, i * 106.667 + 2, 240);
            gfx_FillTriangle(i * 106.667 + 84.667, 240, i * 106.667 + 84.667, 227, i * 106.667 + 104.667, 240);
        }
        WhiText();
        strcpy(menutext, "Item +");
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 266.667 - stringlength / 2;
        gfx_PrintStringXY(menutext, stringlength, 230);
        strcpy(menutext, "Item -");
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 53 - stringlength / 2;
        gfx_PrintStringXY(menutext, stringlength, 230);
        if ((kb_Data[1] & kb_Graph) && (y < 7)) {
            y++;
            while (kb_Data[1] & kb_Graph) {
                kb_Scan();
            }
        }
        if ((kb_Data[1] & kb_Yequ) && (y > 0)) {
            y--;
            while (kb_Data[1] & kb_Yequ) {
                kb_Scan();
            }
        }
        if (kb_Data[7] & kb_Left) {
            playerangle = playerangle - rotationspeed;
        }
        if (kb_Data[7] & kb_Right) {
            playerangle = playerangle + rotationspeed;
        }
        if (kb_Data[7] & kb_Up) {
            playerx = playerx + playerspeed * cos(playerangle);
            playery = playery + playerspeed * sin(playerangle);
        }
        if (playerx > 314) {
            playerx = 314;
        }
        if (playery > 188) {
            playery = 188;
        }
        if (playerx < 166) {
            playerx = 166;
        }
        if (playery < 47) {
            playery = 47;
        }
        if (playerangle > 6.28) {
            playerangle = 0;
        }
        if (playerangle < 0) {
            playerangle = 6.28;
        }
        //render the bullets like it was done in an early version of the game
        found = false;
        for (j = 0; j < 5; ++j) {
            bull_t *i = &bull[j];
            if ((!(i->valid)) && (!(found))) {
                las = j;
                found = true;
                continue;
            }
            i->x = i->x + bulletspeed * cos(i->angle);
            i->y = i->y + bulletspeed * sin(i->angle);
            if (i->x > 314 || i->x < 166 || i->y > 188 || i->y < 47) {
                if (y == 2) {
                    if (i->x > 314 || i->x < 166) {
                        i->x = i->x - bulletspeed * cos(i->angle);
                        i->y = i->y - bulletspeed * sin(i->angle);
                        i->angle = 3.14 - i->angle;
                    } else {
                        i->x = i->x - bulletspeed * cos(i->angle);
                        i->y = i->y - bulletspeed * sin(i->angle);
                        i->angle = 6.28 - i->angle;
                    }
                } else {
                    i->valid = false;
                }
            }
            if (i->valid) {
                gfx_SetColor(224);
                gfx_FillCircle(i->x, i->y, 2);
            }
        }
        if ((kb_Data[1] & kb_2nd) && (fire)) {
            if (found) {
                bull_t *p = &bull[las];
                p->x = playerx;
                p->y = playery;
                p->angle = playerangle;
                p->valid = true;
            }
            fire = false;
        }
        if (!(kb_Data[1] & kb_2nd)) {
            fire = true;
        }
        gfx_SetColor(18);
        gfx_FillCircle(playerx, playery, 5);
        gfx_Line(playerx + 15 * cos(playerangle), playery + 15 * sin(playerangle), playerx, playery);
        if ((kb_Data[6] & kb_Enter) && (t->price <= coins) && (!(t->owned))) {
            while (kb_Data[6] & kb_Enter) {
                kb_Scan();
                gfx_SwapDraw();
            }
            if (Confirm()) {
                t->owned = true;
                coins = coins - t->price;
                if (y == 0) {
                    speed = 4;
// more code goes here.......................................
                }
                if (y == 1) {
                    rotation = 0.15;
                }
                if (y == 2) {
                    bounce = true;
                }
                if (y == 6) {
                    ownedbomb = true;
                }
                if (y == 5) {
                    ownedwindowstealth = true;
                }
            }
        }
        gfx_SwapDraw();
    }
}



void RenderBullets(void) {
    uint8_t i = 0;
    uint8_t j;
    uint8_t ycheck;
    uint16_t xcheck;
    bullet_t *b;
    BlaText();
    for (i = 0; i < numbullets; i++) {
        b = &bullet[i];
        gfx_Sprite(b->spr, b->x, b->y);
    }
    for (i = 0; i < numbullets; i++) {
        b = &bullet[i];
        b->x = b->x + b->xinc;
        b->y = b->y + b->yinc;
        for (j = 0; j < numguards; j++) {
            guard_t *g = &guard[j];
            if (!(g->knockedout)) {
                if ((abs(b->x - g->x) <= 3) && (abs(b->y - g->y) <= 3)) {
                    g->knockedout = true;
                    bullet[i] = bullet[--numbullets];
                    i--;
                    guardselim++;
                    if (gamemode == 0) {
                        g->collected = true;
                    }
                    continue;
                }
            }
        }
        if ((abs(b->x - plx) <= 3) && (abs(b->y - ply) <= 3)) {
            playeralive = false;
            methodofdeath = 0;
        }
        FindTile(b->x + 5, b->y + 5);
        if ((b->x > 290) || (b->x < 20) || (b->y > 210) || (b->y < 20)) {
            if ((bounce) && (!(b->bounced))) {
                if (b->x > 290 || b->x < 20) {
                    b->xinc = -(b->xinc);
                    b->bounced = true;
                } else {
                    b->yinc = -(b->yinc);
                    b->bounced = true;
                }
            } else {
                bullet[i] = bullet[--numbullets];
                i--;
                continue;
            }
        }
        if ((tilefound > 1) && (tilefound < 5)) {
            if ((bounce) && (!(b->bounced))) {
                xcheck = b->x + b->xinc;
                ycheck = b->y - b->yinc;
                tilefound = MapData[(xcheck - 20) / 20][(ycheck - 20) / 20];
                if ((tilefound > 1) && (tilefound < 5)) {
                    b->xinc = -(b->xinc);
                } else {
                    b->yinc = -(b->yinc);
                }
                b->bounced = true;
            } else {
                bullet[i] = bullet[--numbullets];
                i--;
                continue;
            }
        }
        gfx_GetSprite(b->spr, b->x, b->y);
    }
    gfx_SetDraw(1);
    for (i = 0; i < numbullets; i++) {
        //gfx_RotatedScaledTransparentSprite_NoClip(bulletsprite, b->x, b->y, b->angle * 41, 64);
    }
    for (i = 0; i < 5; i++) {
        b = &bullet[i];
        gfx_SetTextXY(307, i * 40 + 20);
        gfx_PrintInt(i, 1);
        gfx_Sprite(b->spr, 305, i * 40 + 40);
    }
}



/* Put all your globals here. */

/* Put all your code here */

void main(void) {
// Main program code goes here
    gfx_sprite_t *buffb = gfx_MallocSprite(20, 20);
    gfx_sprite_t *logoa;
    gfx_sprite_t *logob;
    int score = 0;
    int gamesplayed = 0;
    int highscore = 0;
    int playerdeaths = 0;
    bool menuwrapping = false;
    bool displayfps = false;
    bool playerquitted = false;
    uint8_t levels = 0;
    short current = 1;
    bool condition = false;
    bool keypress = true;
    short max = 5;
    int i;
    int xa;
    int ya;
    int levelscleared = 0;
    int bullets = 10;
    uint8_t bomby;
    uint16_t bombx;
    bool bombactive;
    bool canplacebomb;
//Appvars check
    ti_var_t slota;
    ti_var_t slotb;
    ti_var_t slotc;
    ti_var_t slotd;
    ti_CloseAll();
    slota = ti_Open("ACRCNPLY","r");
    slotb = ti_Open("ACRCNSPR","r");
    slotc = ti_Open("ACRCNMMS","r");
    slotd = ti_Open("ACRCNDAT","r");
    if ((slota == 0) || (slotb == 0) || (slotc == 0)) {
        gfx_Begin();
        gfx_PrintStringXY("You don't have all the required appvars.", 5, 5);
        gfx_PrintStringXY("Please install them and try again.", 5, 15);
        while (!(kb_Data[6] & kb_Enter)) {
            kb_Scan();
        }
        goto QUIT;
    }
    if (slotd == 0) {
        slotd = ti_Open("ACRCNDAT","w+");
        ti_Write(&gamesplayed, 3, 1, slotd);
        ti_Write(&guardselim, 3, 1, slotd);
        ti_Write(&highscore, 3, 1, slotd);
        ti_Write(&playerdeaths, 3, 1, slotd);
        ti_Write(&coins, 3, 1, slotd);
        ti_Write(&levelscleared, 3, 1, slotd);
        ti_Write(&menuwrapping, 1, 1, slotd);
        ti_Write(&displayfps, 1, 1, slotd);
    }
    if (slotd != 0) {
        ti_Read(&gamesplayed, 3, 1, slotd);
        ti_Read(&guardselim, 3, 1, slotd);
        ti_Read(&highscore, 3, 1, slotd);
        ti_Read(&playerdeaths, 3, 1, slotd);
        ti_Read(&coins, 3, 1, slotd);
        ti_Read(&levelscleared, 3, 1, slotd);
        ti_Read(&menuwrapping, 1, 1, slotd);
        ti_Read(&displayfps, 1, 1, slotd);
    }
//sprites from ACRCNSPR
    for(i = 0; i < 5; ++i) {
        ti_Seek(402 * i, 0, slota);
        if (i == 0) {
            playersprite = ti_GetDataPtr(slota);
        }
        if (i == 1) {
            guardspritea = ti_GetDataPtr(slota);
        }
        if (i == 2) {
            guardspriteb = ti_GetDataPtr(slota);
        }
        if (i == 3) {
            guardspritec = ti_GetDataPtr(slota);
        }
        if (i == 4) {
            bulletsprite = ti_GetDataPtr(slota);
        }
    }
    for (i = 0; i < 20; i++) {
        bullet_t *b = &bullet[i];
        b->spr = gfx_MallocSprite(10, 10);
    }
//End of appvars check
//Logos from ACRCNMMS
    logoa = ti_GetDataPtr(slotc);
    ti_Seek(452, 0, slotc);
    logob = ti_GetDataPtr(slotc);
//initialize graphics once, not multiple times
    gfx_Begin();
    current = 1;
//Main menu
MAINMENU:
    max = 5;
    gfx_SetDraw(1);
    gfx_SetTextScale(1, 1);
    condition = false;
    while (!(condition)) {
        kb_Scan();
        gfx_FillScreen(0);
        gfx_SetColor(255);
        for(i = 0; i < 5; i++) {
            xa = randInt(0, 320);
            ya = randInt(0, 240);
            gfx_SetPixel(xa, ya);
        }
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        gfx_SetTransparentColor(255);
        gfx_ScaledTransparentSprite_NoClip(logoa, 70, 5, 3, 2);
        gfx_ScaledTransparentSprite_NoClip(logob, 160, 5, 3, 2);
        for(i = 1; i < max + 1; ++i) {
            GetMenuText(i);
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, i * 22 + 58);
        }
        if ((kb_Data[7] & kb_Down) && (keypress)) {
            current++;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Up) && (keypress)) {
            current--;
            keypress = false;
        }
        if ((kb_Data[1] & kb_Yequ) && (keypress)) {
            showPacks();
        }
        if (!kb_AnyKey()) {
            keypress = true;
        }
        if (current > max) {
            if (menuwrapping) {
                current = 1;
            } else {
                current = max;
            }
        }
        if (current < 1) {
            if (menuwrapping) {
                current = max;
            } else {
                current = 1;
            }
        }
        gfx_SetTextFGColor(7);
        strcpy(menutext, "Ace Recon");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 35);
        gfx_SetColor(255);
        gfx_FillRectangle(88, 225, 144, 15);
        gfx_FillTriangle(88, 240, 88, 225, 68, 240);
        gfx_FillTriangle(232, 240, 232, 225, 252, 240);
        gfx_SetColor(0);
        gfx_FillRectangle(90, 227, 140, 16);
        gfx_FillTriangle(90, 240, 90, 227, 70, 240);
        gfx_FillTriangle(230, 240, 230, 227, 250, 240);
        gfx_SetTextFGColor(255);
        strcpy(menutext, "created by epsilon5");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 229);
        gfx_SetColor(7);
        gfx_Rectangle(-2, current * 22 + 55, 324, 14);
        gfx_Rectangle(-2, current * 22 + 56, 324, 12);
        gfx_SetTextBGColor(255);
        gfx_SetTextFGColor(7);
        gfx_SetTextTransparentColor(255);
        GetMenuText(current);
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, current * 22 + 58);
        if ((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
            gfx_SetTextBGColor(0);
            gfx_SetTextFGColor(255);
            gfx_SetTextTransparentColor(0);
            gfx_SetColor(255);
            gfx_Rectangle(-2, current * 22 + 55, 324, 14);
            gfx_Rectangle(-2, current * 22 + 56, 324, 12);
            gfx_PrintStringXY(menutext, stringlength, current * 22 + 58);
            gfx_SwapDraw();
            while ((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
                kb_Scan();
                condition = true;
            }
        }
        gfx_SwapDraw();
    }

    if (current == 2) {
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        gfx_SetColor(0);
        gfx_FillRectangle(20, 40, 280, 160);
        gfx_SetColor(74);
        gfx_Rectangle(19, 39, 282, 162);
        gfx_Rectangle(20, 40, 280, 160);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Introduction");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 45);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("This game was created by epsilon5 for", 25, 65);
        gfx_PrintStringXY("CC23. It is my first ever C program and", 25, 77);
        gfx_PrintStringXY("3rd game. I hope you like it! Please post", 25, 89);
        gfx_PrintStringXY("any feedback or suggestions to", 25, 101);
        gfx_PrintStringXY("www.cemetech.net.", 25, 113);
        strcpy(menutext, "[enter]: continue to Controls");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 176);
        gfx_SwapDraw();
        while (!(kb_Data[6] & kb_Enter)) {
            kb_Scan();
        }
        while (kb_Data[6] & kb_Enter) {
            kb_Scan();
        }
        gfx_SetColor(0);
        gfx_FillRectangle(20, 40, 280, 160);
        gfx_SetColor(74);
        gfx_Rectangle(19, 39, 282, 162);
        gfx_Rectangle(20, 40, 280, 160);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Controls");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 45);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("[arrows]: navigate menus", 25, 65);
        gfx_PrintStringXY("[<] and [>]: rotate player", 25, 77);
        gfx_PrintStringXY("[clear]: quit game", 25, 89);
        gfx_PrintStringXY("[enter]: confirm selection", 25, 101);
        gfx_PrintStringXY("[2nd]: fire bullets", 25, 113);
        gfx_PrintStringXY("[y=]: pause the game", 25, 125);
        gfx_PrintStringXY("[^]: move forward", 25, 137);
        strcpy(menutext, "[enter]: continue to Conclusion");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 176);
        gfx_SwapDraw();
        while (!(kb_Data[6] & kb_Enter)) {
            kb_Scan();
        }
        while (kb_Data[6] & kb_Enter) {
            kb_Scan();
        }
        gfx_SetColor(0);
        gfx_FillRectangle(20, 40, 280, 160);
        gfx_SetColor(74);
        gfx_Rectangle(19, 39, 282, 162);
        gfx_Rectangle(20, 40, 280, 160);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Conclusion");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 45);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("As stated earlier, this game was made", 25, 65);
        gfx_PrintStringXY("for Cemetech Contest 23. If you enjoy", 25, 77);
        gfx_PrintStringXY("this game, or want to see the other", 25, 89);
        gfx_PrintStringXY("entries, go to www.cemetech.net, then", 25, 101);
        gfx_PrintStringXY("to 'contests' in the Forums page.", 25, 113);
        gfx_PrintStringXY("Thanks again for playing.", 25, 125);
        strcpy(menutext, "[enter]: return to the menu");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 176);
        gfx_SwapDraw();
        while (!(kb_Data[6] & kb_Enter)) {
            kb_Scan();
        }
        while (kb_Data[6] & kb_Enter) {
            kb_Scan();
        }
        goto MAINMENU;
    }

    if (current == 4) {
        short y = 0;
        while ((!(kb_Data[6] & kb_Enter)) || (!(y == 3))) {
            kb_Scan();
            gfx_SetColor(0);
            gfx_FillRectangle(40, 80, 240, 90);
            gfx_SetTextFGColor(255);
            gfx_SetTextBGColor(0);
            gfx_SetTextTransparentColor(0);
            gfx_SetTextScale(2, 2);
            strcpy(menutext, "Options");
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, 85);
            gfx_SetTextScale(1, 1);
            strcpy(menutext, "Menu wrapping");
            gfx_PrintStringXY(menutext, 45, 107);
            strcpy(menutext, "FPS meter");
            gfx_PrintStringXY(menutext, 45, 123);
            strcpy(menutext, "Reset highscore");
            gfx_PrintStringXY(menutext, 45, 139);
            strcpy(menutext, "Save and exit");
            gfx_PrintStringXY(menutext, 45, 155);
            if ((kb_Data[7] & kb_Down) && (keypress)) {
                y++;
                keypress = false;
            }
            if ((kb_Data[7] & kb_Up) && (keypress)) {
                y--;
                keypress = false;
            }
            if (!kb_AnyKey()) {
                keypress = true;
            }
            if (y > 3) {
                if (menuwrapping) {
                    y = 0;
                } else {
                    y = 3;
                }
            }
            if (y < 0) {
                if (menuwrapping) {
                    y = 3;
                } else {
                    y = 0;
                }
            }
            if (kb_Data[7] & kb_Left) {
                if (y == 0) {
                    menuwrapping = false;
                }
                if (y == 1) {
                    displayfps = false;
                }
            }
            if (kb_Data[7] & kb_Right) {
                if (y == 0) {
                    menuwrapping = true;
                }
                if (y == 1) {
                    displayfps = true;
                }
            }
            if (menuwrapping) {
                gfx_SetTextFGColor(7);
                strcpy(menutext, "< ON");
            } else {
                gfx_SetTextFGColor(224);
                strcpy(menutext, "OFF >");
            }
            stringlength = 275 - gfx_GetStringWidth(menutext);
            gfx_PrintStringXY(menutext, stringlength, 107);
            if (displayfps) {
                gfx_SetTextFGColor(7);
                strcpy(menutext, "< ON");
            } else {
                gfx_SetTextFGColor(224);
                strcpy(menutext, "OFF >");
            }
            stringlength = 275 - gfx_GetStringWidth(menutext);
            gfx_PrintStringXY(menutext, stringlength, 123);
            gfx_SetTextFGColor(0);
            if (kb_Data[6] & kb_Enter) {
                gfx_SetColor(255);
                gfx_SetTextFGColor(255);
                if (y == 2) {
                    gfx_Blit(0);
                    gfx_Rectangle(40, y * 16 + 104, 140, 14);
                    gfx_Rectangle(41, y * 16 + 105, 138, 12);
                    gfx_SwapDraw();
                    while (kb_Data[6] & kb_Enter) {
                        kb_Scan();
                    }
                    if (Confirm()) {
                        highscore = 0;
                        playerdeaths = 0;
                        guardselim = 0;
                        levelscleared = 0;
                        gamesplayed = 0;
                    }
                }
            } else {
                gfx_SetColor(7);
                gfx_SetTextFGColor(7);
            }
            gfx_Rectangle(40, y * 16 + 104, 140, 14);
            gfx_Rectangle(41, y * 16 + 105, 138, 12);
            //outside of box
            gfx_SetColor(74);
            gfx_Rectangle(40, 80, 240, 90);
            gfx_Rectangle(41, 81, 238, 88);
            if (y == 0) {
                strcpy(menutext, "Menu wrapping");
            }
            if (y == 1) {
                strcpy(menutext, "FPS meter");
            }
            if (y == 2) {
                strcpy(menutext, "Reset highscore");
            }
            if (y == 3) {
                strcpy(menutext, "Save and exit");
            }
            gfx_PrintStringXY(menutext, 45, y * 16 + 107);
            gfx_SwapDraw();
        }
        while (kb_Data[6] & kb_Enter) {
            kb_Scan();
        }
        goto MAINMENU;
    }

    if (current == 3) {
        gfx_SetTextBGColor(0);
        gfx_SetTextFGColor(255);
        gfx_SetTextTransparentColor(0);
        gfx_SetColor(0);
        gfx_FillRectangle(40, 60, 240, 120);
        gfx_SetColor(74);
        gfx_Rectangle(39, 59, 242, 122);
        gfx_Rectangle(40, 60, 240, 120);
        gfx_SetTextScale(2, 2);
        strcpy(menutext, "Statistics");
        CenterText();
        gfx_PrintStringXY(menutext, stringlength, 65);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("Games played:", 45, 84);
        gfx_PrintStringXY("Levels passed:", 45, 98);
        gfx_PrintStringXY("Guards eliminated:", 45, 112);
        gfx_PrintStringXY("Deaths:", 45, 126);
        gfx_PrintStringXY("Kill/death ratio:", 45, 140);
        gfx_PrintStringXY("Highscore:", 45, 154);
        gfx_PrintStringXY("Current coins:", 45, 168);
        sprintf(menutext, "%d", gamesplayed);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 84);
        sprintf(menutext, "%d", levelscleared);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 98);
        sprintf(menutext, "%d", guardselim);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 112);
        sprintf(menutext, "%d", playerdeaths);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 126);
        if (guardselim > playerdeaths) {
            sprintf(menutext, "%d : 1", guardselim / playerdeaths);
        } else {
            sprintf(menutext, "1 : %d", playerdeaths / guardselim);
        }
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 140);
        sprintf(menutext, "%d", highscore);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 154);
        sprintf(menutext, "%d", coins);
        stringlength = gfx_GetStringWidth(menutext);
        stringlength = 275 - stringlength;
        gfx_PrintStringXY(menutext, stringlength, 168);
        gfx_SwapDraw();
        while (!(kb_Data[6] & kb_Enter)) {
            kb_Scan();
        }
        while (kb_Data[6] & kb_Enter) {
            kb_Scan();
        }
        goto MAINMENU;
    }


    if (current == 5) {
        if (Confirm()) {
            goto QUIT;
        } else {
            goto MAINMENU;
        }
    }

    if (current == 1) {
//Main game loop
        gamemode = GameMode(menuwrapping);
        if (gamemode == 4) {
            goto MAINMENU;
        }
        levels = 0;
        score = 0;
        speed = 3;
        rotation = 0.1;
        bounce = false;
        ownedbomb = false;
        bullets = 10;
        gamesplayed++;
        playerlives = 5;
        numguards = 0;
//NOTE: I changed this 0 to a 4 for temporary testing purposes, needs to be changed back eventually
        while (playerlives > 0) {
            gfx_SetDraw(1);
            numbullets = 0;
            levelpassed = false;
            playeralive = true;
            playerseen = false;
            playerquitted = false;
            gfx_SetDraw(0);
            ti_CloseAll();
            for (i = 0; i < 50; ++i) {
                gfx_SetColor(7);
                for(xa = 0; xa < 5; xa++) {
                    gfx_SetPixel(randInt(0, 320), randInt(0, 240));
                }
                gfx_FillScreen(0);
                gfx_SetTextScale(3, 3);
                gfx_SetTextBGColor(0);
                gfx_SetTextFGColor(255);
                gfx_SetTextTransparentColor(0);
                gfx_SetColor(255);
                gfx_FillRectangle(0, 77, 320, 23);
                gfx_SetColor(7);
                gfx_FillRectangle(0, 75, 320, 2);
                gfx_FillRectangle(0, 100, 320, 2);
                sprintf(menutext, "Level  %d", levels + 1);
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 30);
                gfx_SetTextScale(1, 1);
                sprintf(menutext, "Lives x%d", playerlives);
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 210);
                sprintf(menutext, "Guards x%d", numguards);
                CenterText();
                gfx_SetTextFGColor(0);
                gfx_SetTextBGColor(255);
                gfx_SetTextTransparentColor(255);
                gfx_PrintStringXY(menutext, stringlength, 85);
                gfx_SetTextScale(1, 1);
                gfx_SwapDraw();
            }
            plx = startx;
            ply = starty;
            bombactive = false;
            gfx_SwapDraw();
            gfx_SetTextBGColor(0);
            gfx_SetTextFGColor(255);
            gfx_SetTextTransparentColor(0);
            if (displayfps) {
                timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;
            }
            gfx_SetDraw(1);
            RenderMap(levels);
            gfx_SetColor(74);
            gfx_Rectangle(19, 19, 282, 202);
            gfx_SetColor(224);
            for(i = 0; i < 5; ++i) {
                if (i < playerlives) {
                    gfx_FillCircle(i * 15 + 9, 6, 4);
                } else {
                    gfx_Circle(i* 15 + 9, 6, 4);
                }
            }
            gfx_SetTextFGColor(255);
            sprintf(menutext, "Pack: %s Level: %d", currpackname, levels + 1);
            stringlength = gfx_GetStringWidth(menutext);
            gfx_PrintStringXY(menutext, 160 - stringlength / 2, 2);
            gfx_Blit(1);
            gfx_GetSprite(buffb, plx, ply);
            while (!(levelpassed)) {
                kb_Scan();
                gfx_Sprite(buffb, plx, ply);
                if (playerseen) {
                    gfx_SetColor(224);
                    gfx_Rectangle(0, 0, 2, 240);
                    gfx_Rectangle(318, 0, 2, 240);
                }
                if (kb_Data[7] & kb_Left) {
                    plangle = plangle - rotation;
                }
                if (kb_Data[7] & kb_Right) {
                    plangle = plangle + rotation;
                }
                FindTile((plx + 4 * cos(plangle)) - 10, (ply + 4 * cos(plangle)) - 10);
                if (tilefound == 6) {
                    break;
                }
                if (!(playeralive)) {
                    break;
                }
                if (kb_Data[6] & kb_Clear) {
                    gfx_Blit(0);
                    if (Confirm()) {
                        goto MAINMENU;
                    }
                }
                if ((kb_Data[7] & kb_Up) && ((tilefound < 2) || (tilefound == 5) || (tilefound == 6))) {
                    plx = plx + speed * cos(plangle);
                    ply = ply + speed * sin(plangle);
                }
                if ((kb_Data[1] & kb_2nd) && (canfire) && ((gamemode == 1) || (bullets > 0))) {
                    FireBullet(plx + 20 * cos(plangle), ply + 20 * sin(plangle), plangle);
                    canfire = false;
                    if (gamemode == 0) {
                        bullets--;
                    }
                }
                if (!(kb_Data[1] & kb_2nd)) {
                    canfire = true;
                }
                if (kb_Data[1] & kb_Yequ) {
                    max = PauseMenu(menuwrapping);
                    if (max == 1) {
                        if (Confirm()) {
                            playeralive = false;
                            playerquitted = true;
                            playerlives--;
                        }
                    }
                    if (max == 2) {
                        if (Confirm()) {
                            goto MAINMENU;
                        }
                    }
                }
                if (ownedbomb) {
                    if ((kb_Data[2] & kb_Alpha) && (canplacebomb)) {
                        canplacebomb = false;
                        if (!(bombactive)) {
                            bombx = plx;
                            bomby = ply;
                            bombactive = true;
                            canplacebomb = false;
                        } else {
                            for (i = 0; i < numguards; ++i) {
                                guard_t *g = &guard[i];
                                if (!(g->knockedout)) {
                                    if ((abs(g->x - bombx) <= 35) && (abs(g->y - bomby) <= 35)) {
                                        g->knockedout = true;
                                        guardselim++;
                                        if (gamemode == 0) {
                                            g->collected = true;
                                        }
                                    }
                                }
                            }
                            if ((abs(bombx - plx) <= 35) && (abs(bomby - ply) <= 35)) {
                                playeralive = false;
                                methodofdeath = 1;
                            }
                            gfx_SetColor(231);
                            gfx_Blit(0);
                            for (i = 0; i < 7; ++i) {
                                if (i < 3) {
                                    gfx_FillCircle(bombx, bomby, i * 5);
                                } else {
                                    gfx_FillCircle(bombx, bomby, (7 - (i - 3)) * 5);
                                }
                                gfx_SwapDraw();
                            }
                            bombactive = false;
                        }
                    }
                    if (!(kb_Data[2] & kb_Alpha)) {
                        canplacebomb = true;
                    }
                    if (bombactive) {
                        gfx_SetColor(0);
                        gfx_FillCircle(bombx, bomby, 4);
                    }
                }
                if (plx > 280) {
                    plx = 280;
                }
                if (ply > 200) {
                    ply = 200;
                }
                if (plx < 20) {
                    plx = 20;
                }
                if (ply < 20) {
                    ply = 20;
                }
                if (plangle > 6.28) {
                    plangle = 0;
                }
                if (plangle < 0) {
                    plangle = 6.28;
                }
                gfx_GetSprite(buffb, plx, ply);
                RenderGuards();
                RenderBullets();
                gfx_RotatedScaledTransparentSprite_NoClip(playersprite, plx, ply, plangle * 41 + 64, 64);
                gfx_SetTextFGColor(167);
                if (gamemode == 1) {
                    sprintf(menutext, "Coins x%d", coins);
                }
                if (gamemode == 0) {
                    sprintf(menutext, "Bullets x%d", bullets);
                }
                gfx_SetTextFGColor(255);
                stringlength = gfx_GetStringWidth(menutext);
                gfx_PrintStringXY(menutext, 315 - stringlength, 2);
                if (displayfps) {
                    gfx_SetColor(0);
                    sprintf(menutext, "FPS = %d", 32768 / timer_1_Counter);
                    CenterText();
                    gfx_FillRectangle(0, 230, 320, 8);
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    timer_1_Counter = 0;
                }
                sprintf(menutext, "Numbullets = %d", numbullets);
                gfx_PrintStringXY(menutext, 5, 230);
                //taken out for the time being
                gfx_SwapDraw();
            }
            if ((!(playeralive)) && (!(playerquitted))) {
                playerdeaths++;
                while (!(kb_Data[1] & kb_Graph)) {
                    kb_Scan();
                    gfx_SetTextScale(2, 2);
                    gfx_ZeroScreen();
                    gfx_SetTextFGColor(224);
                    if (methodofdeath == 0) {
                        strcpy(menutext, "You were shot.");
                    } else {
                        strcpy(menutext, "You were blown up.");
                    }
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 5);
                    gfx_SetTextScale(1, 1);
                    gfx_SetTextFGColor(0);
                    gfx_SetTextBGColor(255);
                    gfx_SetTextTransparentColor(255);
                    for (i = 0; i < 3; ++i) {
                        gfx_SetColor(74);
                        gfx_FillRectangle(i * 106.667 + 20, 225, 67, 15);
                        gfx_FillTriangle(i * 106.667 + 20, 240, i * 106.667 + 20, 225, i * 106.667, 240);
                        gfx_FillTriangle(i * 106.667 + 86.667, 240, i * 106.667 + 86.667, 225, i * 106.667 + 106.667, 240);
                        //Rectangle(startx + 20, 225, width, 15);
                        //Triangle(startx, 240, startx, 225, startx - 20, 240);
                        //Triangle(startx + width, 240, startx + width, 225, startx + width + 20, 240)
                        gfx_SetColor(255);
                        gfx_FillRectangle(i * 106.667 + 22, 227, 63, 13);
                        gfx_FillTriangle(i * 106.667 + 22, 240, i * 106.667 + 22, 227, i * 106.667 + 2, 240);
                        gfx_FillTriangle(i * 106.667 + 84.667, 240, i * 106.667 + 84.667, 227, i * 106.667 + 104.667, 240);
                    }
                    strcpy(menutext, "Continue");
                    stringlength = gfx_GetStringWidth(menutext);
                    stringlength = 266.667 - stringlength / 2;
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    if (kb_Data[1] & kb_Zoom) {
                        if (Confirm()) {
                            goto MAINMENU;
                        }
                    }
                    strcpy(menutext, "Main menu");
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    gfx_SetColor(224);
                    for(i = 0; i < 10; i++) {
                        xa = randInt(0, 320);
                        ya = randInt(0, 240);
                        gfx_SetPixel(xa, ya);
                    }
                    if (kb_Data[1] & kb_Zoom) {
                        goto MAINMENU;
                    }
                    gfx_SwapDraw();
                    levelpassed = false;
                }
                while (kb_Data[1] & kb_Graph) {
                    kb_Scan();
                }
                playerlives--;
            }
            if (tilefound == 6) {
                score = score + ((levels + 1) * 20);
                score = score + ((playerseen == false) * (10 * numguards));
                levels++;
                levelpassed = false;
                gfx_SetColor(7);
                levelscleared++;
                if (gamemode == 0) {
                    bullets = bullets + 5;
                }
                while (!(kb_Data[1] & kb_Graph)) {
                    gfx_ZeroScreen();
                    gfx_SetTextBGColor(0);
                    gfx_SetTextFGColor(7);
                    gfx_SetTextTransparentColor(0);
                    gfx_SetTextScale(2, 2);
                    strcpy(menutext, "You passed the level!");
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 5);
                    gfx_SetTextFGColor(255);
                    gfx_SetTextScale(1, 1);
                    sprintf(menutext, "Total score: %d", score);
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 45);
                    sprintf(menutext, "Level score: %d", (levels * 20) + ((playerseen == false) * (10 * numguards)));
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 60);
                    sprintf(menutext, "Stealth bonus: %d", (playerseen == false) * (10 * numguards));
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 75);
                    if (gamemode == 0) {
                        strcpy(menutext, "Bullets gained: 5");
                        CenterText();
                        gfx_PrintStringXY(menutext, stringlength, 90);
                    }
                    gfx_SetTextFGColor(0);
                    gfx_SetTextBGColor(255);
                    gfx_SetTextTransparentColor(255);
                    for (i = 0; i < 3; ++i) {
                        gfx_SetColor(74);
                        gfx_FillRectangle(i * 106.667 + 20, 225, 67, 15);
                        gfx_FillTriangle(i * 106.667 + 20, 240, i * 106.667 + 20, 225, i * 106.667, 240);
                        gfx_FillTriangle(i * 106.667 + 86.667, 240, i * 106.667 + 86.667, 225, i * 106.667 + 106.667, 240);
                        //Rectangle(startx + 20, 225, width, 15);
                        //Triangle(startx, 240, startx, 225, startx - 20, 240);
                        //Triangle(startx + width, 240, startx + width, 225, startx + width + 20, 240)
                        gfx_SetColor(255);
                        gfx_FillRectangle(i * 106.667 + 22, 227, 63, 13);
                        gfx_FillTriangle(i * 106.667 + 22, 240, i * 106.667 + 22, 227, i * 106.667 + 2, 240);
                        gfx_FillTriangle(i * 106.667 + 84.667, 240, i * 106.667 + 84.667, 227, i * 106.667 + 104.667, 240);
                    }
                    if (gamemode == 1) {
                        strcpy(menutext, "Shop");
                    } else {
                        strcpy(menutext, " ");
                    }
                    stringlength = gfx_GetStringWidth(menutext);
                    stringlength = 53.333 - stringlength / 2;
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    if ((kb_Data[1] & kb_Yequ) && (gamemode == 1)) {
                        ItemShop();
                    }
                    strcpy(menutext, "Continue");
                    stringlength = gfx_GetStringWidth(menutext);
                    stringlength = 266.667 - stringlength / 2;
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    strcpy(menutext, "Main menu");
                    CenterText();
                    gfx_PrintStringXY(menutext, stringlength, 230);
                    if (kb_Data[1] & kb_Zoom) {
                        if (Confirm()) {
                            goto MAINMENU;
                        }
                    }
                    for(i = 0; i < 10; i++) {
                        xa = randInt(0, 320);
                        ya = randInt(0, 240);
                        gfx_SetPixel(xa, ya);
                    }
                    gfx_SwapDraw();
                    kb_Scan();
                }
                while (kb_Data[1] & kb_Graph) {
                    kb_Scan();
                }
            }
            playeralive = true;
            playerseen = false;
            if ((tilefound == 6) && (levels == numlevels)) {
                gfx_ZeroScreen();
                gfx_ScaledTransparentSprite_NoClip(logoa, 70, 5, 3, 2);
                gfx_ScaledTransparentSprite_NoClip(logob, 160, 5, 3, 2);
                gfx_SetTextScale(1, 1);
                gfx_SetTextBGColor(0);
                gfx_SetTextFGColor(7);
                gfx_SetTextTransparentColor(0);
                strcpy(menutext, "Ace Recon");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 35);
                gfx_SetTextFGColor(255);
                gfx_SetTextScale(3, 3);
                strcpy(menutext, "You win!");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 115);
                gfx_SetTextScale(1, 1);
                strcpy(menutext, "Stay tuned for more buggy fun");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 210);
                strcpy(menutext, "in the full release...");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 225);
                gfx_SwapDraw();
                while (!(kb_Data[6] & kb_Enter)) {
                    kb_Scan();
                }
                while (kb_Data[6] & kb_Enter) {
                    kb_Scan();
                }
            }
            if (playerlives == 0) {
                gfx_ZeroScreen();
                gfx_ScaledTransparentSprite_NoClip(logoa, 70, 5, 3, 2);
                gfx_ScaledTransparentSprite_NoClip(logob, 160, 5, 3, 2);
                gfx_SetTextScale(1, 1);
                gfx_SetTextBGColor(0);
                gfx_SetTextFGColor(7);
                gfx_SetTextTransparentColor(0);
                strcpy(menutext, "Ace Recon");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 35);
                gfx_SetTextFGColor(224);
                gfx_SetTextScale(3, 3);
                strcpy(menutext, "You lose...");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 115);
                gfx_SetTextScale(1, 1);
                gfx_SetTextFGColor(255);
                strcpy(menutext, "Returning to the main menu.");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 210);
                strcpy(menutext, "Please play again!");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 225);
                gfx_SwapDraw();
                while (!(kb_Data[6] & kb_Enter)) {
                    kb_Scan();
                }
                while (kb_Data[6] & kb_Enter) {
                    kb_Scan();
                }
            }
            if ((tilefound == 6) && (levels == numlevels)) {
                playerlives = 0;
            }
        }
        i = 0;
        while (!(kb_Data[1] & kb_Graph)) {
            kb_Scan();
            if (kb_Data[1] & kb_Zoom) {
                gfx_Blit(0);
                if (Confirm()) {
                    goto QUIT;
                }
            }
            gfx_ZeroScreen();
            gfx_SetColor(255);
            for(i = 0; i < 10; i++) {
                xa = randInt(0, 320);
                ya = randInt(0, 240);
                gfx_SetPixel(xa, ya);
            }
            gfx_ScaledTransparentSprite_NoClip(logoa, 70, 5, 3, 2);
            gfx_ScaledTransparentSprite_NoClip(logob, 160, 5, 3, 2);
            gfx_SetTextScale(1, 1);
            gfx_SetTextBGColor(0);
            gfx_SetTextTransparentColor(0);
            gfx_SetTextFGColor(7);
            strcpy(menutext, "Ace Recon");
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, 35);
            gfx_SetTextFGColor(255);
            gfx_SetTextScale(1, 1);
            sprintf(menutext, "Final score: %d", score);
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, 60);
            sprintf(menutext, "Highscore: %d", highscore);
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, 75);
            if (score >= highscore) {
                gfx_SetTextScale(2, 2);
                gfx_SetTextFGColor(randInt(0, 255));
                strcpy(menutext, "New highscore!");
                CenterText();
                gfx_PrintStringXY(menutext, stringlength, 180);
                highscore = score;
            }
            gfx_SetTextScale(1, 1);
            for (i = 0; i < 3; ++i) {
                gfx_SetColor(74);
                gfx_FillRectangle(i * 106.667 + 20, 225, 67, 15);
                gfx_FillTriangle(i * 106.667 + 20, 240, i * 106.667 + 20, 225, i * 106.667, 240);
                gfx_FillTriangle(i * 106.667 + 86.667, 240, i * 106.667 + 86.667, 225, i * 106.667 + 106.667, 240);
                //Rectangle(startx + 20, 225, width, 15);
                //Triangle(startx, 240, startx, 225, startx - 20, 240);
                //Triangle(startx + width, 240, startx + width, 225, startx + width + 20, 240)
                gfx_SetColor(255);
                gfx_FillRectangle(i * 106.667 + 22, 227, 63, 13);
                gfx_FillTriangle(i * 106.667 + 22, 240, i * 106.667 + 22, 227, i * 106.667 + 2, 240);
                gfx_FillTriangle(i * 106.667 + 84.667, 240, i * 106.667 + 84.667, 227, i * 106.667 + 104.667, 240);
            }
            gfx_SetTextBGColor(255);
            gfx_SetTextTransparentColor(255);
            gfx_SetTextFGColor(0);
            strcpy(menutext, "Main menu");
            stringlength = gfx_GetStringWidth(menutext);
            stringlength = 266.667 - stringlength / 2;
            gfx_PrintStringXY(menutext, stringlength, 230);
            strcpy(menutext, "Quit");
            CenterText();
            gfx_PrintStringXY(menutext, stringlength, 230);
            gfx_SwapDraw();
        }
        goto MAINMENU;
    }
QUIT:
    ti_CloseAll();
    gfx_End();
    slotd = ti_Open("ACRCNDAT","w+");
    ti_Write(&gamesplayed, 3, 1, slotd);
    ti_Write(&guardselim, 3, 1, slotd);
    ti_Write(&highscore, 3, 1, slotd);
    ti_Write(&playerdeaths, 3, 1, slotd);
    ti_Write(&coins, 3, 1, slotd);
    ti_Write(&levelscleared, 3, 1, slotd);
    ti_Write(&menuwrapping, 1, 1, slotd);
    ti_Write(&displayfps, 1, 1, slotd);
    ti_SetArchiveStatus(1, slotd);
}

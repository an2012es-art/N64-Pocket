/*
 * N64 Pocket - Nintendo DS/DSi homebrew interface prototype
 *
 * This program draws a rotating 3D-style cube on the top screen and a game
 * list on the bottom screen. It is NOT an N64 emulator yet: no N64 CPU,
 * graphics, audio, ROM loader, or cartridge compatibility is implemented.
 *
 * Build with devkitPro + devkitARM + libnds.
 */
#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

static PrintConsole bottomConsole;

static const char *gameNames[] = {
    "Demo Slot 01",
    "Demo Slot 02",
    "Demo Slot 03",
    "Demo Slot 04",
    "Demo Slot 05"
};
#define GAME_COUNT (sizeof(gameNames) / sizeof(gameNames[0]))

static int selected = 0;
static int angle = 0;
static bool infoMode = false;

/* A simple rotating cube: genuine DS hardware 3D, used only as a UI effect. */
static void drawCube(void) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -4.4);
    glRotateYi(angle);
    glRotateXi(angle / 2);
    glRotateZi(angle / 3);

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_ID(0));

    glBegin(GL_QUADS);
        /* Front: purple */
        glColor3b(22, 4, 31);
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16( 1));
        glVertex3v16(inttov16( 1), inttov16(-1), inttov16( 1));
        glVertex3v16(inttov16( 1), inttov16( 1), inttov16( 1));
        glVertex3v16(inttov16(-1), inttov16( 1), inttov16( 1));

        /* Back: dark pink */
        glColor3b(31, 5, 18);
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));
        glVertex3v16(inttov16(-1), inttov16( 1), inttov16(-1));
        glVertex3v16(inttov16( 1), inttov16( 1), inttov16(-1));
        glVertex3v16(inttov16( 1), inttov16(-1), inttov16(-1));

        /* Left: blue-purple */
        glColor3b(9, 7, 31);
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16( 1));
        glVertex3v16(inttov16(-1), inttov16( 1), inttov16( 1));
        glVertex3v16(inttov16(-1), inttov16( 1), inttov16(-1));

        /* Right: pink */
        glColor3b(31, 9, 24);
        glVertex3v16(inttov16(1), inttov16(-1), inttov16(-1));
        glVertex3v16(inttov16(1), inttov16( 1), inttov16(-1));
        glVertex3v16(inttov16(1), inttov16( 1), inttov16( 1));
        glVertex3v16(inttov16(1), inttov16(-1), inttov16( 1));

        /* Top: lavender */
        glColor3b(24, 18, 31);
        glVertex3v16(inttov16(-1), inttov16(1), inttov16(-1));
        glVertex3v16(inttov16(-1), inttov16(1), inttov16( 1));
        glVertex3v16(inttov16( 1), inttov16(1), inttov16( 1));
        glVertex3v16(inttov16( 1), inttov16(1), inttov16(-1));

        /* Bottom: near black purple */
        glColor3b(10, 3, 14);
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));
        glVertex3v16(inttov16( 1), inttov16(-1), inttov16(-1));
        glVertex3v16(inttov16( 1), inttov16(-1), inttov16( 1));
        glVertex3v16(inttov16(-1), inttov16(-1), inttov16( 1));
    glEnd();
    glFlush(0);
}

static void drawBottom(void) {
    consoleSelect(&bottomConsole);
    consoleClear();

    iprintf("\x1b[1;5H N64 POCKET");
    iprintf("\x1b[2;1H------------------------------");
    iprintf("\x1b[3;2H\x1b[35mGAME LIBRARY\x1b[39m");

    for (int i = 0; i < GAME_COUNT; ++i) {
        int row = 5 + i * 2;
        if (i == selected) {
            iprintf("\x1b[%d;2H\x1b[35m> %-23s\x1b[39m", row, gameNames[i]);
        } else {
            iprintf("\x1b[%d;2H  %-23s", row, gameNames[i]);
        }
    }

    iprintf("\x1b[15;1H------------------------------");
    if (infoMode) {
        iprintf("\x1b[16;2H%s", gameNames[selected]);
        iprintf("\x1b[17;2HDemo placeholder only");
        iprintf("\x1b[18;2HNo N64 ROM/core implemented");
        iprintf("\x1b[19;2HB: back   START: toggle info");
    } else {
        iprintf("\x1b[16;2HUP/DOWN: choose slot");
        iprintf("\x1b[17;2HA: details  B: close");
        iprintf("\x1b[18;2HSTART: toggle info");
        iprintf("\x1b[19;2H3D cube is a UI animation");
    }
}

int main(void) {
    /* 3D engine on the main/top screen. */
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);
    glClearColor(2, 1, 5, 31);
    glClearPolyID(0);
    glClearDepth(0x7FFF);
    glViewport(0, 0, 255, 191);

    consoleInit(&bottomConsole, 0, BgType_Text4bpp, BgSize_T_256x256,
                31, 0, false, true);
    BG_PALETTE_SUB[0] = RGB15(2, 1, 5);
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31);

    while (1) {
        scanKeys();
        int down = keysDown();

        if (down & KEY_UP) {
            if (selected > 0) selected--;
            drawBottom();
        }
        if (down & KEY_DOWN) {
            if (selected < GAME_COUNT - 1) selected++;
            drawBottom();
        }
        if (down & KEY_A) {
            infoMode = true;
            drawBottom();
        }
        if (down & KEY_B) {
            infoMode = false;
            drawBottom();
        }
        if (down & KEY_START) {
            infoMode = !infoMode;
            drawBottom();
        }

        glClearColor(2, 1, 5, 31);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawCube();
        angle = (angle + 384) & 65535;

        swiWaitForVBlank();
    }
    return 0;
}

//  ---------------------------------------------------------------------------
//
//  @file       TwSimpleSDL2.c
//  @brief      A simple example that uses AntTweakBar for SDL2 with OpenGL and SDL2
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              SDL:         http://www.libsdl.org
//  
//  @author     Blinky0815
//
//  ---------------------------------------------------------------------------

#include <AntTweakBar/AntTweakBar.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

using namespace std;

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

int width = 640, height = 480;
int bpp, flags;
int quit = 0;
TwBar *bar;
int n, numCubes = 30;
float color0[] = {1.0f, 0.5f, 0.0f};
float color1[] = {0.5f, 1.0f, 0.0f};
double ka = 5.3, kb = 1.7, kc = 4.1;

void initGL() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

int setViewport(int width, int height) {
    width = width;
    height = height;
    GLfloat ratio;
    if (height == 0) {
        height = 1;
    }
    ratio = (GLfloat) width / (GLfloat) height;
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return 1;
}

void initAntweakBar(){
    TwInit(TW_OPENGL, NULL);

    // Tell the window size to AntTweakBar
    TwWindowSize(width, height);

    // Create a tweak bar
    bar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with SDL and OpenGL.\nPress [Space] to toggle fullscreen.' "); // Message added to the help bar.

    // Add 'width' and 'height' to 'bar': they are read-only (RO) variables of type TW_TYPE_INT32.
    TwAddVarRO(bar, "Width", TW_TYPE_INT32, &width,
            " label='Wnd width' help='Width of the graphics window (in pixels)' ");

    TwAddVarRO(bar, "Height", TW_TYPE_INT32, &height,
            " label='Wnd height' help='Height of the graphics window (in pixels)' ");

    // Add 'numCurves' to 'bar': this is a modifiable variable of type TW_TYPE_INT32. Its shortcuts are [c] and [C].
    TwAddVarRW(bar, "NumCubes", TW_TYPE_INT32, &numCubes,
            " label='Number of cubes' min=1 max=100 keyIncr=c keyDecr=C help='Defines the number of cubes in the scene.' ");

    // Add 'ka', 'kb and 'kc' to 'bar': they are modifiable variables of type TW_TYPE_DOUBLE
    TwAddVarRW(bar, "ka", TW_TYPE_DOUBLE, &ka,
            " label='X path coeff' keyIncr=1 keyDecr=CTRL+1 min=-10 max=10 step=0.01 ");
    TwAddVarRW(bar, "kb", TW_TYPE_DOUBLE, &kb,
            " label='Y path coeff' keyIncr=2 keyDecr=CTRL+2 min=-10 max=10 step=0.01 ");
    TwAddVarRW(bar, "kc", TW_TYPE_DOUBLE, &kc,
            " label='Z path coeff' keyIncr=3 keyDecr=CTRL+3 min=-10 max=10 step=0.01 ");

    // Add 'color0' and 'color1' to 'bar': they are modifable variables of type TW_TYPE_COLOR3F (3 floats color)
    TwAddVarRW(bar, "color0", TW_TYPE_COLOR3F, &color0,
            " label='Start color' help='Color of the first cube.' ");
    TwAddVarRW(bar, "color1", TW_TYPE_COLOR3F, &color1,
            " label='End color' help='Color of the last cube. Cube colors are interpolated between the Start and End colors.' ");

    // Add 'quit' to 'bar': this is a modifiable (RW) variable of type TW_TYPE_BOOL32 
    // (a boolean stored in a 32 bits integer). Its shortcut is [ESC].
    TwAddVarRW(bar, "Quit", TW_TYPE_BOOL32, &quit,
            " label='Quit?' true='+' false='-' key='ESC' help='Quit program.' ");
}

void render() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -6.0f);

    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, 0.0f); 
    glEnd();

    /* Move Right 3 Units */
    glTranslatef(3.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS); /* Draw A Quad */
    glVertex3f(-1.0f, 1.0f, 0.0f); /* Top Left */
    glVertex3f(1.0f, 1.0f, 0.0f); /* Top Right */
    glVertex3f(1.0f, -1.0f, 0.0f); /* Bottom Right */
    glVertex3f(-1.0f, -1.0f, 0.0f); /* Bottom Left */
    glEnd(); /* Done Drawing The Quad */

    while (!quit) {
        SDL_Event event;
        int handled;

        // Clear screen
        glClearColor(0.5f, 0.75f, 0.8f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set OpenGL camera
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(40, (double) width / height, 1, 10);
        gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

        // Draw cubes
        for (n = 0; n < numCubes; ++n) {
            double t = 0.05 * n - (double) SDL_GetTicks() / 2000.0;
            double r = 5.0 * n + (double) SDL_GetTicks() / 10.0;
            float c = (float) n / numCubes;

            // Set cube position
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslated(0.6 * cos(ka * t), 0.6 * cos(kb * t), 0.6 * sin(kc * t));
            glRotated(r, 0.2, 0.7, 0.2);
            glScaled(0.1, 0.1, 0.1);
            glTranslated(-0.5, -0.5, -0.5);

            // Set cube color
            glColor3f((1.0f - c) * color0[0] + c * color1[0], (1.0f - c) * color0[1] + c * color1[1], (1.0f - c) * color0[2] + c * color1[2]);

            // Draw cube
            glBegin(GL_QUADS);
            glNormal3f(0, 0, -1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(1, 0, 0); // front face
            glNormal3f(0, 0, +1);
            glVertex3f(0, 0, 1);
            glVertex3f(1, 0, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(0, 1, 1); // back face
            glNormal3f(-1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glVertex3f(0, 1, 1);
            glVertex3f(0, 1, 0); // left face
            glNormal3f(+1, 0, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(1, 1, 1);
            glVertex3f(1, 0, 1); // right face
            glNormal3f(0, -1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(1, 0, 1);
            glVertex3f(0, 0, 1); // bottom face  
            glNormal3f(0, +1, 0);
            glVertex3f(0, 1, 0);
            glVertex3f(0, 1, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(1, 1, 0); // top face
            glEnd();
        }

        // Draw tweak bars
        TwDraw();

        // Present frame buffer
        SDL_GL_SwapWindow(displayWindow);

        // Process incoming events
        while (SDL_PollEvent(&event)) {
            // Send event to AntTweakBar
            handled = TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);

            // If event has not been handled by AntTweakBar, process it
            if (!handled) {
                switch (event.type) {
                    case SDL_QUIT: // Window is closed
                        quit = 1;
                        break;
                }
            }
        }
        SDL_RenderPresent(displayRenderer);
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    initGL();
    setViewport(640, 480);
    initAntweakBar();
    render();
    SDL_Quit();

    return 0;
}

#include "plot.h"
#include <stdio.h>
#include <float.h>

#include <SDL/SDL.h>
#include <GL/gl.h>

/* TODO: change? */
#define MIN_PLOT_RESOLUTION 0.0000001

/* TODO: handle better? Probably should be relative to window size and
 * interval size. I mean, do we really need to plot from -11 to 11 for a 
 * -1 -> 1 window with a resolution of 1? */
/* We multiply this number by the plot resolution, then graph that much more
 * off the left and right ends of the x interval */
#define PLOT_OVERFLOW 10

int plot_Width = 800;
int plot_Height = 600;

enum PlotState { PS_NINIT, PS_NRSIZE, PS_GOOD, PS_FAIL } plot_State;

SDL_Surface *plot_Screen;

/* TODO: less exit-y */
int plot_Init() {
	const SDL_VideoInfo *videoInfo;
	int videoFlags;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	videoInfo = SDL_GetVideoInfo();
	if(!videoInfo) {
		fprintf(stderr, "Can't query video info: %s\n", SDL_GetError());
		exit(1);
	}

	videoFlags = SDL_OPENGL;
	videoFlags |= SDL_GL_DOUBLEBUFFER;
	videoFlags |= SDL_HWPALETTE;
	/*videoFlags |= SDL_RESIZABLE; TODO */

	if(videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	if(videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	plot_Screen = SDL_SetVideoMode(plot_Width, plot_Height, 32, videoFlags);
	if(plot_Screen == NULL) {
		fprintf(stderr, "Unable to create plot screen: %s\n", SDL_GetError());
		exit(1);
	}

	/* initialize OpenGL */
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	if(plot_Resize() != 0)
		return 1;

	return 0;
}

/* TODO change this to have 0,0 in the center? */
int plot_Resize() {
	double ratio = plot_Width / plot_Height;

	glViewport(0, 0, plot_Width, plot_Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, plot_Width, plot_Height, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return 0;
}

int plot_CheckState() {
	switch(plot_State) {
		case PS_NINIT:
			return plot_Init();
		case PS_NRSIZE:
			return plot_Resize();
		case PS_GOOD:
			return 0;
		case PS_FAIL: default:
			return 1;
	}
}

void clearPlot() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	SDL_GL_SwapBuffers();
}

/* Plot dimension (width, height setters and getters {{{ */
void setPlotWidth(int pWidth) {
	if(pWidth < 1) return;
	plot_Width = pWidth;
}
void setPlotHeight(int pHeight) {
	if(pHeight < 1) return;
	plot_Height = pHeight;
}
void setPlotDimensions(int pWidth, int pHeight) {
	if((pWidth < 1) || (pHeight < 1)) return;
	plot_Width = pWidth;
	plot_Height = pHeight;
}

int getPlotWidth() {
	return plot_Width;
}
int getPlotHeight() {
	return plot_Height;
}
/* }}} */

void plotDelay(int time) {
	SDL_Delay(time);
}


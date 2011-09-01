#ifndef PLOT_H
#define PLOT_H

int plot_CheckState();
void clearPlot();

void setPlotWidth(int pWidth);
void setPlotHeight(int pHeight);
void setPlotDimensions(int pWidth, int pHeight);

int getPlotWidth();
int getPlotHeight();

void plotDelay(int time);

#endif /* PLOT_H */

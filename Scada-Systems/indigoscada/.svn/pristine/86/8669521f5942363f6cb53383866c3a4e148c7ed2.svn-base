#include <stdlib.h>
#include <math.h>

float randg(float mean, float sigma);

#include "gplot3.c"  /* cut down version of gplot3.c */

float randg(float mean, float sigma)
{
    unsigned i;
    float sum;
    for (sum = 0, i = 0; i < 12; ++i)
        sum += (float) rand() / (float) RAND_MAX;
    return (sum - 6) * sigma + mean;
}


int main()
{
    int i;
    int h1[100];
    float x[100], y[100];


    plot_t *p, *p1;

    p = openPlot("name the plot");
    p1 = openPlot("name of plot2");
    
    for(i=0;i<100;i++) {
	h1[i] = randg(200,20);
	x[i] = randg(200,20);
	y[i] = randg(220,20);
    }

    plotData(h1,100,p,plotInts);

    printf("Press any key to boxes\n");
    getchar();
    setLineType(p,"boxes");
    plotData(h1,100,p,plotInts);
  

    printf("Press any key to plotFloats\n");
    getchar();
    plotData(x,100,p1,plotFloats);
    
    printf("Press any key to plotxy\n");
    getchar();
    plotxy(x,y,100,p1);


    printf("Press any key to line style\n");
    getchar();
    setLineType(p1,"points");
    plotxy(x,y,100,p1);

    printf("Press any key to scatPlot\n");
    getchar();
    scatPlot(p1,2,x,100,y,100);
    
    printf("Press any key to boxPlot\n");
    getchar();
    boxPlot(p1,2,x,100,"x-data",y,100,"y-data");

    printf("Press any key to replot\n");
    getchar();
    replotData(y,100,p,plotFloats);


#if 0
    /* plot stuff not worked out properly yet */
    printf("Press any key to print plot\n");
    getchar();
    printPlot(p);
#endif

    printf("Press any key to  finish\n");
    getchar();

    closePlot(p);
    closePlot(p1);

    return 0;
   
}

    





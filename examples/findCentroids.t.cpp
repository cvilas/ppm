#include <vector>
#include "PPM.hpp"
#include <math.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cout << "usage: findCentroids.t <ppm image>" << endl;
		exit(-1);
	}
	PPM image(argv[1]);
	PPM imageNew(image.getNumRows(), image.getNumColumns());
	
	vector<int> paintedPixels;
	double centroidX, centroidY;
	
	// Find connected regions and their centroids
	for(int r = 0; r < image.getNumRows(); r += 2)
	{
		for(int c = 0; c < image.getNumColumns(); c += 2)
		{
			if (image(r,c) == 255)
			{
				image.paintFill(r, c, 255, 100, paintedPixels);
				image.calculateCentroid(paintedPixels, centroidX, centroidY);
				cout << "(" << centroidX << ", " << centroidY << ")" << endl;
				imageNew(centroidY, centroidX) = 255;
				paintedPixels.clear();
			}
		}
		imageNew.writeToFile("images/centroidsImage.ppm");
	}
	return 0;
}

#include "CCLAlgoFinal.h"

#include "omp.h"



	CCLAlgoFinal::CCLAlgoFinal(int* image, int count, int x, int y, int z) : CCLAlgoBase(imageBuffer, count, x, y, z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;

		ccVec.resize(objCount+1);


	}




	int CCLAlgoFinal::findFinalComponent()
	{
		
		for (int i = 0; i < xSize; i++) {
			for (int j = 0; j < ySize; j++) {
				for (int k = 0; k < ySize; k++) {
					int id = getIdFromPos(i, j, 0);

					if (imageBuffer[id] > 0) {
						return imageBuffer[id];
					}
				}
			}
		}
			printf("Error no final component found\n");
			return 0;

	}


		void CCLAlgoFinal::run()
		{
			int finalComponent = findFinalComponent();
			for (int i = 0; i < xSize; i++) {
				for (int j = 0; j < ySize; j++) {
					for (int k = 0; k < zSize; k++) {
						int id = getIdFromPos(i, j, k);
						if (imageBuffer[id] != finalComponent) {
							imageBuffer[id] = 0;
						}
						else {
							imageBuffer[id] = 1;
						}
					}
				}
			}

		}


	

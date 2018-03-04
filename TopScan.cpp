/*TopScan激光点云滤波程序*/
/*http://www.chenzhaiyu.com*/

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 设置遍历窗口的尺寸
#define maxWindowSize 50
#define minWindowSize 1

using namespace std;

int main()
{
	// 读入数据，建议使用数据量较小的文件
	char *fileName = "samp24.txt";
	FILE *fp = fopen(fileName, "r"); 
	char buffer[1000];
	int dataCountBuffer = 0;
	// 判断文件是否打开成功
	if (!fp) return -1;
	while (fgets(buffer, 1000, fp))
		dataCountBuffer++;

	static int dataCount = dataCountBuffer;
	rewind(fp);
	printf("file: %s\n", fileName);
	printf("------filter processing------\n");

	// 定义点云三维坐标变量
	float *X = (float*)malloc(dataCount * sizeof(float));
	float *Y = (float*)malloc(dataCount * sizeof(float));
	float *height = (float*)malloc(dataCount * sizeof(float));
	int *flag = (int*)malloc(dataCount * sizeof(int));

	// 定义平面坐标最小值变量
	float minX = 10000000;
	float minY = 10000000;
	
	// 定义高差阈值变量
	float threshold;

	// 定义窗口坐标数组，由每个点的格网坐标组成
	int *windowX = (int*)malloc(dataCount * sizeof(int));
	int *windowY = (int*)malloc(dataCount * sizeof(int));

	// 窗口对应的格网坐标最大值
	int maxWindowX = 0;
	int maxWindowY = 0;

	// 定义滤波结果数组，初始化为0
	int *result = (int*)malloc(dataCount * sizeof(int));
	memset(result, 0, sizeof(int)* dataCount);

	// 数据存入变量，并归一化平面坐标
	for (int i = 0; i < dataCount; i++)
	{
		fscanf(fp, "%f%f%f%d", &X[i], &Y[i], &height[i], &flag[i]);
		if (X[i] < minX) minX = X[i];
		if (Y[i] < minY) minY = Y[i];
	}

	// 遍历不同尺度的窗口
	for (int windowSize = maxWindowSize, threshold = (maxWindowSize - minWindowSize) + 1; windowSize >= minWindowSize; windowSize--, threshold--)
	{
		// 计算格网坐标最大值
		for (int i = 0; i < dataCount; i++)
		{
			windowX[i] = (X[i] - minX) / windowSize;
			windowY[i] = (Y[i] - minY) / windowSize;
			if (windowX[i] > maxWindowX) maxWindowX = windowX[i];
			if (windowY[i] > maxWindowY) maxWindowY = windowY[i];
		}

		// 每个格网中高程的最小值，数组
		float minHeightInWindow[500][500];
		for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
			minHeightInWindow[i][j] = 1000;

		// 计算每个格网中高程的最小值 
		for (int j = 0; j < maxWindowX; j++)
		{
			for (int k = 0; k < maxWindowY; k++)
			{
				for (int i = 0; i < dataCount; i++)
				{
					if (windowX[i] == j && windowY[i] == k && height[i] < minHeightInWindow[j][k]) minHeightInWindow[j][k] = height[i];
				}
				// 判别是否为房屋点
				for (int i = 0; i < dataCount; i++)
				{
					// 判断i点属于(j,k)窗口，且窗口不为空集，且与窗口中最低点高差大于阈值，则为房屋点
					if (windowX[i] == j && windowY[i] == k && minHeightInWindow[j][k] < 999 && height[i] - minHeightInWindow[j][k] > threshold) result[i] = 1;
				}
			}
		}
		/*//输出其中最小尺寸窗口的判别结果
		for (int i = 0; i < dataCount; i++)
		if (windowSize == minWindowSize) printf("result for data[%d]: %d\n", i + 1, result[i]);
		*/
	}

	// 滤波精度评定
	int correctCount = 0;
	float accuracy;
	for (int i = 0; i < dataCount; i++)
	{
		if (flag[i] == result[i]) correctCount++;
	}
	accuracy = float(correctCount) / float(dataCount);

	// 输出
	printf("data line count: %d\n", dataCount);
	printf("Window Size range: %d -> %d\n", maxWindowSize, minWindowSize);
	printf("Filter Accuracy: %f\n", accuracy);
	fclose(fp);
	system("pause");
	return 0;
}
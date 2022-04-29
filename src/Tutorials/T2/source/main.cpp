#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <iostream>
int main()
{
    GDALDataset* poDataset;
    GDALAllRegister();
    std::string filename = "../../../../data/1m_DTM.tif";
    poDataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);

    float xright, ybottom, x, y, width, height;
    if (poDataset == NULL)
    {
        std::cout << "Can't read" << std::endl;
        return -1;
    }
    else
    {
        width = GDALGetRasterBandXSize(poDataset);
        height = GDALGetRasterBandYSize(poDataset);
        std::string proj = GDALGetProjectionRef(poDataset);
        std::cout << filename << " read successful!" << std::endl;
        std::cout << "Coordinate System: " << proj << std::endl;
        std::cout << "Data width :" << width << std::endl;
        std::cout << "Data height :" << height << std::endl;
    }

    double adfGeoTransform[6];
    if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
    {
        printf("Origin = (%.6f,%.6f)\n",
            adfGeoTransform[0], adfGeoTransform[3]);

        printf("Pixel Size = (%.6f,%.6f)\n",
            adfGeoTransform[1], adfGeoTransform[5]);
        x = adfGeoTransform[0];
        y = adfGeoTransform[3];
        xright = x + adfGeoTransform[1] * width;
        ybottom = y + adfGeoTransform[5] * height;
        std::cout << "East: " << x << " West: " << xright << " North: " << y << " South: " << ybottom << std::endl;
        std::cout << "Pixel Size(x y): " << adfGeoTransform[1] << " " << adfGeoTransform[5] << std::endl;
    }

    return 0;
}

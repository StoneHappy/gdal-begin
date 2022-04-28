#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <iostream>
int main()
{
    GDALDataset* poDataset;
    GDALAllRegister();
    poDataset = (GDALDataset*)GDALOpen("test.tiff", GA_ReadOnly);
    if (poDataset == NULL)
    {
        std::cout << "Can't read" << std::endl;
    }
    else
    {
        std::cout << "Read Successful" << std::endl;
    }
}

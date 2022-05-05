//#include "gdal_priv.h"
//#include "cpl_conv.h" // for CPLMalloc()
//#include <iostream>
//#include <vector>
//using namespace std;
//
//bool getRawValuesFromFile(string fname, vector<vector<float>>& vecs, float& min, float& max, float& xres, float& yres, string& projection, double& XORIGIN, double& YORIGIN, int& W, int& H);
//void ComputeGeoProperties(GDALDataset* poDataset, int width, int height, double& x, double& y, double& xright, double& ybottom, double& xres, double& yres);
//
//
//int main()
//{
//    
//    std::string filename = "../../../../data/drycreek.tif";
//    vector<vector<float>> vecs;
//    float min, max, xres,yres;
//    string projection;
//    double XORIGIN, YORIGIN;
//    int W, H;
//    getRawValuesFromFile(filename, vecs, min, max, xres, yres, projection, XORIGIN, YORIGIN, W, H);
//
//    return 0;
//}
//
//
//bool getRawValuesFromFile(string fname, vector<vector<float>>& vecs, float& min, float& max, float& xres, float& yres, string& projection, double& XORIGIN, double& YORIGIN, int& W, int& H)
//{
//
//    //vector<float> temp = vector<float>()
//    GDALDataset* poDataset;
//
//
//    // Register all gdal drivers
//    GDALAllRegister();
//
//    // lets load a "dataset" which is gdal terminology for your data
//    poDataset = (GDALDataset*)GDALOpen(fname.c_str(), GA_ReadOnly);
//
//    // error handing!
//    if (poDataset == NULL)
//    {
//        cout << "Failure to load file due to not existing or write permissions!!!" << endl;
//        return false;
//    }
//
//    OGRSpatialReference    oUTM;
//
//    oUTM.SetProjCS("UTM 11 / WGS84");
//    oUTM.SetWellKnownGeogCS("WGS84");
//    oUTM.SetUTM(11);
//
//    projection = string(poDataset->GetProjectionRef());
//
//    // time to find the width of the data and print it for sanity
//    cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;
//
//    // A new term "band" or pertentaining to one layer within a dataset
//    GDALRasterBand* poBand;
//
//    int             nBlockXSize, nBlockYSize;
//    int             bGotMin, bGotMax;
//    double          adfMinMax[2];
//
//    // Get the first band -- that's what this function does to grab one band.
//    poBand = poDataset->GetRasterBand(1);
//
//    // Getting the dimensions of the band... only 2D
//    poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
//
//    // Print some more handy information
//    printf("Block=%dx%d Type=%s, ColorInterp=%s\n",
//        nBlockXSize, nBlockYSize,
//        GDALGetDataTypeName(poBand->GetRasterDataType()),
//        GDALGetColorInterpretationName(
//            poBand->GetColorInterpretation()));
//
//    // Get the min and max
//    min = adfMinMax[0] = poBand->GetMinimum(&bGotMin);
//    max = adfMinMax[1] = poBand->GetMaximum(&bGotMax);
//
//    // this guy will look at your current band and compute min max or you can do the above
//    if (!(bGotMin && bGotMax))
//        GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
//
//    // dimensions of our datasets
//    int width = poBand->GetXSize();
//    int height = poBand->GetYSize();
//    W = width;
//    H = height;
//    double x, y, xright, ybottom, XRES, YRES;
//    // Time to compute the xres and yres
//    ComputeGeoProperties(poDataset, width, height, x, y, xright, ybottom, XRES, YRES);
//    xres = XRES;
//    yres = YRES;
//
//    XORIGIN = x;
//    YORIGIN = y;
//
//    // something extra not need for this simple function but you can get the number
//    // of bands in the dataset
//    int bands = poDataset->GetRasterCount();
//
//    // something to hold our values!!
//    float* pafScanline;
//
//    // printing the min and max of the dataset
//    std::cout << "Before allocation " << "Min: " << min << " Max: " << max << endl;
//
//    // load the data and smile
//    pafScanline = (float*)CPLMalloc(sizeof(float) * width * height);
//
//    // I don't like dynamic float array (that is a lie) just a preference
//    vector<vector<float>> out = vector<vector<float>>(width, vector<float>(height, 0));
//
//    auto err = poBand->RasterIO(GF_Read, 0, 0, width, height, pafScanline, width, height, GDT_Float32, 0, 0);
//    cout << "Loaded data with status " << err << endl;
//
//    // Lets check the status of the loading of this dataset...
//    if (err != 0)
//    {
//        cout << "Dataset fail to load with the following error: ";
//        switch (err)
//        {
//            // Look up CPLErrorHandler for more information
//        case 1:
//            cout << "DEBUG" << endl;
//            break;
//        case 2:
//            cout << "Warning" << endl;
//            break;
//        case 3:
//            cout << "Failure" << endl;
//            break;
//        case 4:
//            cout << "Fatal" << endl;
//            break;
//        }
//    }
//
//    // move everything to a vector -- slow memory but okay for what we are doing
//    for (int i = 0; i < height; i++)
//    {
//        for (int j = 0; j < width; j++)
//        {
//            if (pafScanline[(i)*width + j] > 0)
//                out[j][i] = pafScanline[(i)*width + j];
//            else
//                out[j][i] = 0;
//        }
//    }
//
//    CPLFree(pafScanline);
//
//    cout << "Done Loading" << endl;
//    vecs = out;
//    return true;
//}
//
//void ComputeGeoProperties(GDALDataset* poDataset, int width, int height, double& x, double& y, double& xright, double& ybottom, double& xres, double& yres)
//{
//    double adfGeoTransform[6];
//    if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
//    {
//        printf("Origin = (%.6f,%.6f)\n",
//            adfGeoTransform[0], adfGeoTransform[3]);
//
//        printf("Pixel Size = (%.6f,%.6f)\n",
//            adfGeoTransform[1], adfGeoTransform[5]);
//        x = adfGeoTransform[0];
//        y = adfGeoTransform[3];
//        xright = x + adfGeoTransform[1] * (double)(width);
//        ybottom = y + adfGeoTransform[5] * (double)(height);
//    }
//    else
//    {
//        return;
//    }
//
//    string proj;
//    proj = string(poDataset->GetProjectionRef());
//
//    OGRSpatialReference sr2;
//    char* test = &proj[0];
//    sr2.importFromWkt(&test);
//
//    // Get Geography Coordinate System
//    OGRSpatialReference* geog2 = sr2.CloneGeogCS();
//
//    // Now to create coordinate transform function
//    OGRCoordinateTransformation* poTransform2 = OGRCreateCoordinateTransformation(&sr2, geog2);
//
//    // Compute corners
//    double x2 = 0;
//
//    // Lets compute the resolution -- Despite the one provided by the geotransform
//    double absoluteW = xright - x;
//    double absoluteH = y - ybottom;
//
//    // now lets compute the average resolution of the DEM
//    xres = absoluteW / (width);
//    yres = absoluteH / (height);
//
//};
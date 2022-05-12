#include "TiffMesh.h"
#include <glm/glm.hpp>
#include<vcg/complex/algorithms/create/platonic.h>

#include <Core/Base/macro.h>

namespace Stone
{
    TiffMesh::TiffMesh(const std::string filename)
    {
        std::vector<std::vector<float>> vecs;
        float min, max, xres, yres;
        std::vector<vcg::Point3f> v;
        std::vector<vcg::Point3i> indexVec;
        std::string projection;
        double XORIGIN, YORIGIN;
        int W, H;
        getRawValuesFromFile(filename, vecs, min, max, xres, yres, projection, XORIGIN, YORIGIN, W, H);
        //std::cout << "max = " << max << std::endl;
        // Time to construct a height map based on the xres and yres
        for (int i = 0; i < vecs.size() - 1; i++)
        {
            for (int j = 0; j < vecs[i].size() - 1; j++)
            {

                float UL = (float)(vecs[i][j]) / (float)(max); // Upper left

                float LL = (float)(vecs[i + 1][j]) / (float)(max); // Lower left
                float UR = (float)(vecs[i][j + 1]) / (float)(max); // Upper right
                float LR = (float)(vecs[i + 1][j + 1]) / (float)(max); // Lower right

                if (UL <= 0)
                {
                    UL = 0;
                }
                if (UR <= 0)
                {
                    UR = 0;
                }
                if (LR <= 0)
                {
                    LR = 0;
                }
                if (LL <= 0)
                {
                    LL = 0;
                }

                glm::vec3 ULV = { i * xres, UL * max, j * yres };
                glm::vec3 LLV = { (i + 1) * xres, LL * max, j * yres };
                glm::vec3 URV = { i * xres, UR * max, (j + 1) * yres };
                glm::vec3 LRV = { (i + 1) * xres, LR * max, (j + 1) * yres };



                v.push_back(vcg::Point3f(ULV.x, ULV.y, ULV.z));
                v.push_back(vcg::Point3f(LLV.x, LLV.y, LLV.z));
                v.push_back(vcg::Point3f(URV.x, URV.y, URV.z));
                v.push_back(vcg::Point3f(LRV.x, LRV.y, LRV.z));

                indexVec.push_back(vcg::Point3i(v.size() - 4, v.size() - 1, v.size() - 2));
                indexVec.push_back(vcg::Point3i(v.size() - 4, v.size() - 3, v.size() - 1));
            }
        }

        vcg::tri::BuildMeshFromCoordVectorIndexVector(m_Mesh, v, indexVec);
        update();
    }

    bool TiffMesh::getRawValuesFromFile(std::string fname, std::vector<std::vector<float>>& vecs, float& min, float& max, float& xres, float& yres, std::string& projection, double& XORIGIN, double& YORIGIN, int& W, int& H)
    {

        //vector<float> temp = vector<float>()
        GDALDataset* poDataset;


        // Register all gdal drivers
        GDALAllRegister();

        // lets load a "dataset" which is gdal terminology for your data
        poDataset = (GDALDataset*)GDALOpen(fname.c_str(), GA_ReadOnly);

        // error handing!
        if (poDataset == NULL)
        {
            LOG_ERROR("Failure to load file due to not existing or write permissions!!!");
            return false;
        }

        OGRSpatialReference    oUTM;

        oUTM.SetProjCS("UTM 11 / WGS84");
        oUTM.SetWellKnownGeogCS("WGS84");
        oUTM.SetUTM(11);

        projection = std::string(poDataset->GetProjectionRef());

        // time to find the width of the data and print it for sanity
        /*cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;*/

        // A new term "band" or pertentaining to one layer within a dataset
        GDALRasterBand* poBand;

        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];

        // Get the first band -- that's what this function does to grab one band.
        poBand = poDataset->GetRasterBand(1);

        // Getting the dimensions of the band... only 2D
        poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);

        // Print some more handy information
        /*printf("Block=%dx%d Type=%s, ColorInterp=%s\n",
            nBlockXSize, nBlockYSize,
            GDALGetDataTypeName(poBand->GetRasterDataType()),
            GDALGetColorInterpretationName(
                poBand->GetColorInterpretation()));*/

        // Get the min and max
        min = adfMinMax[0] = poBand->GetMinimum(&bGotMin);
        max = adfMinMax[1] = poBand->GetMaximum(&bGotMax);

        // this guy will look at your current band and compute min max or you can do the above
        if (!(bGotMin && bGotMax))
            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

        // dimensions of our datasets
        int width = poBand->GetXSize();
        int height = poBand->GetYSize();
        W = width;
        H = height;
        double x, y, xright, ybottom, XRES, YRES;
        // Time to compute the xres and yres
        computeGeoProperties(poDataset, width, height, x, y, xright, ybottom, XRES, YRES);
        xres = XRES;
        yres = YRES;

        XORIGIN = x;
        YORIGIN = y;

        // something extra not need for this simple function but you can get the number
        // of bands in the dataset
        int bands = poDataset->GetRasterCount();

        // something to hold our values!!
        float* pafScanline;

        // printing the min and max of the dataset
        //std::cout << "Before allocation " << "Min: " << min << " Max: " << max << endl;

        // load the data and smile
        pafScanline = (float*)CPLMalloc(sizeof(float) * width * height);

        // I don't like dynamic float array (that is a lie) just a preference
        std::vector<std::vector<float>> out = std::vector<std::vector<float>>(width, std::vector<float>(height, 0));

        auto err = poBand->RasterIO(GF_Read, 0, 0, width, height, pafScanline, width, height, GDT_Float32, 0, 0);
        LOG_DEBUG("Loaded data with status {0}", err);

        // Lets check the status of the loading of this dataset...
        if (err != 0)
        {
            std::string ErrorType;
            switch (err)
            {
                // Look up CPLErrorHandler for more information
            case 1:

                ErrorType = "DEBUG";
                break;
            case 2:
                ErrorType = "Warning";
                break;
            case 3:
                ErrorType = "Failure";
                break;
            case 4:
                ErrorType = "Fatal";
                break;
            }
            LOG_ERROR("Dataset fail to load with the following error: {0}", ErrorType);
        }

        // move everything to a vector -- slow memory but okay for what we are doing
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (pafScanline[(i)*width + j] > 0)
                    out[j][i] = pafScanline[(i)*width + j];
                else
                    out[j][i] = 0;
            }
        }

        CPLFree(pafScanline);

        LOG_DEBUG("Done Loading");
        vecs = out;
        return true;
    }

    void TiffMesh::computeGeoProperties(GDALDataset* poDataset, int width, int height, double& x, double& y, double& xright, double& ybottom, double& xres, double& yres)
    {
        double adfGeoTransform[6];
        if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
        {
            /*printf("Origin = (%.6f,%.6f)\n",
                adfGeoTransform[0], adfGeoTransform[3]);

            printf("Pixel Size = (%.6f,%.6f)\n",
                adfGeoTransform[1], adfGeoTransform[5]);*/
            x = adfGeoTransform[0];
            y = adfGeoTransform[3];
            xright = x + adfGeoTransform[1] * (double)(width);
            ybottom = y + adfGeoTransform[5] * (double)(height);
        }
        else
        {
            return;
        }

        std::string proj;
        proj = std::string(poDataset->GetProjectionRef());

        OGRSpatialReference sr2;
        char* test = &proj[0];
        sr2.importFromWkt(&test);

        // Get Geography Coordinate System
        OGRSpatialReference* geog2 = sr2.CloneGeogCS();

        // Now to create coordinate transform function
        OGRCoordinateTransformation* poTransform2 = OGRCreateCoordinateTransformation(&sr2, geog2);

        // Compute corners
        double x2 = 0;

        // Lets compute the resolution -- Despite the one provided by the geotransform
        double absoluteW = xright - x;
        double absoluteH = y - ybottom;

        // now lets compute the average resolution of the DEM
        xres = absoluteW / (width);
        yres = absoluteH / (height);

    }
}

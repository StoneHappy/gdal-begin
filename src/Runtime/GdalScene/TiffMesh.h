#pragma once
#include <Resource/Data/Implement/VCG/VCGMesh.h>
#include <vector>
#include <string>
#include <gdal_priv.h>
#include <cpl_conv.h> // for CPLMalloc()
namespace Stone
{
	class TiffMesh : public VCGMesh
	{
	public:
		TiffMesh(const std::string filename);
	private:
		bool getRawValuesFromFile(std::string fname, std::vector<std::vector<float>>& vecs, float& min, float& max, float& xres, float& yres, std::string& projection, double& XORIGIN, double& YORIGIN, int& W, int& H);
		void computeGeoProperties(GDALDataset* poDataset, int width, int height, double& x, double& y, double& xright, double& ybottom, double& xres, double& yres);
	};
}
#pragma once
#include <Resource/Data/Implement/VCG/VCGMesh.h>
#include <string>
#include <vector>
#include <gdal.h>
#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogrsf_frmts.h>
#include <glm/glm.hpp>
#include "TiffMesh.h"
namespace Stone
{
	class Shape : public VCGMesh
	{
	public:
		Shape(const std::string& filename, TiffMesh* tm);

	private:
		void createMesh(TiffMesh* tm);
		void load(const std::string& filename);

		OGRSpatialReference* sr;
		std::vector<std::vector<glm::vec2>> points;
		int veccount;
	};
}
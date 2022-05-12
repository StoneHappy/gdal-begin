#include "Shape.h"

#include <Core/Base/macro.h>

namespace Stone
{
	Shape::Shape(const std::string& filename, TiffMesh* tm)
	{
        load(filename);
        createMesh(tm);
	}
    void Shape::createMesh(TiffMesh* tm)
    {
        
    }
    void Shape::load(const std::string& filename)
	{
        OGRRegisterAll();
        GDALDataset* poDS;
        poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
        // Now to load in the points .....
        points = std::vector<std::vector<glm::vec2>>();

        // Grab the first layer
        OGRLayer* layer = poDS->GetLayers()[0];

        // Grab the spatial reference and create a coordinate transform function
        sr = layer->GetSpatialRef();

        // Taking from http://www.compsci.wm.edu/SciClone/documentation/software/geo/gdal-1.9.0/html/ogr/ogr_apitut.html
        OGRFeature* poFeature;

        layer->ResetReading();
        while ((poFeature = layer->GetNextFeature()) != NULL)
        {
            OGRFeatureDefn* poFDefn = layer->GetLayerDefn();
            int iField;

            OGRGeometry* poGeometry;

            poGeometry = poFeature->GetGeometryRef();
            if (poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
            {
                OGRPoint* poPoint = (OGRPoint*)poGeometry;
                double x = poPoint->getX();
                double y = poPoint->getY();
                points.push_back(std::vector<glm::vec2>());
                points[points.size() - 1].push_back(glm::vec2(x, y));
            }
            else if (poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
            {
                OGRLineString* ls = (OGRLineString*)poGeometry;
                points.push_back(std::vector<glm::vec2>());
                for (int i = 0; i < ls->getNumPoints(); i++)
                {
                    OGRPoint p;
                    ls->getPoint(i, &p);

                    // This function can transform a larget set of points.....
                    double x = p.getX();
                    double y = p.getY();
                    points[points.size() - 1].push_back(glm::vec2(x, y));
                    //poTransform->Transform (1, &x, &y);
                    //cout << "POINT" << p.getX() << " " << p.getY() << endl;
                }
            }
            else if (poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
            {

                OGRLineString* ls = (OGRLineString*)poGeometry->getBoundary();
                points.push_back(std::vector<glm::vec2>());
                //exit(0);
                for (int i = 0; i < ls->getNumPoints(); i++)
                {
                    OGRPoint p;
                    ls->getPoint(i, &p);

                    // This function can transform a larget set of points.....
                    double x = p.getX();
                    double y = p.getY();
                    points[points.size() - 1].push_back(glm::vec2(x, y));
                    //poTransform->Transform (1, &x, &y);
                }
            }
            OGRFeature::DestroyFeature(poFeature);
        }
	}
}

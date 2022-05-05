#include <Engine.h>
#include <Editor.h>


int main(int argc, char *argv[])
{
    
    /*vector<vector<float>> vecs;
    float min, max, xres, yres;
    string projection;
    double XORIGIN, YORIGIN;
    int W, H;
    getRawValuesFromFile(filename, vecs, min, max, xres, yres, projection, XORIGIN, YORIGIN, W, H);
    vector<int>indicies;
    vector<Vertex> vertexes;
    createMesh(vecs, xres, yres, 10, indicies, vertexes);*/

    //std::cout << "vertexes num " << vertexes.size() << std::endl;
    Stone::PublicSingleton<Stone::Engine>::getInstance().startEngine(argc, argv);
    Stone::PublicSingleton<Stone::Editor>::getInstance().initialize(argc, argv);
    Stone::PublicSingleton<Stone::Editor>::getInstance().run();
    Stone::PublicSingleton<Stone::Engine>::getInstance().shutdownEngine();
}

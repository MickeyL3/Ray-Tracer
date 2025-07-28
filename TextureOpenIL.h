//=====================================================================
// Image loader for ray tracing applications
// Author:
// R. Mukundan, Department of Computer Science and Software Engineering
// University of Canterbury, Christchurch, New Zealand.
//=====================================================================

#if !defined(H_TEXOPENIL)
#define H_TEXOPENIL

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <IL/il.h>
using namespace std;

class TextureOpenIL {
    private:
        int imageWid, imageHgt, imageChnls;  //Width, height, number of channels
        ILubyte* imageData;
        bool loadOpenILImage(const char* string);
    public:
		TextureOpenIL(): imageWid(0), imageHgt(0), imageChnls(0) {}
        TextureOpenIL(const char* string);
        glm::vec3 getColorAt(float s, float t);
};

#endif


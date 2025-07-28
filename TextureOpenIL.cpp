//=====================================================================
// Image loader for ray tracing applications
// Author:
// R. Mukundan, Department of Computer Science and Software Engineering
// University of Canterbury, Christchurch, New Zealand.
//=====================================================================

#include "TextureOpenIL.h"

TextureOpenIL::TextureOpenIL(const char* filename) {
    ilInit();
    ILuint imageId;
    ilGenImages(1, &imageId);				// Create an image id similar to glGenTextures()
    ilBindImage(imageId);					// Binding of DevIL image name
    ilEnable(IL_ORIGIN_SET);				// Enable change of origin of referenence
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    loadOpenILImage(filename);
}

/**
 * Return color at texture coord (s, t) where s and t are in [0,1]
 */
glm::vec3 TextureOpenIL::getColorAt(float s, float t) {
	if(imageWid == 0 || imageHgt == 0) return glm::vec3(0);
    int i = (int) (s * imageWid);  //pixel coordinates
    int j = (int) (t * imageHgt);
	if(i < 0 || i > imageWid-1 || j < 0 || j > imageHgt-1) return glm::vec3(0);
    int index = ((j * imageWid) + i) * imageChnls;

    int r = imageData[index];
    int g = imageData[index + 1];
    int b = imageData[index + 2];

	if(r < 0) r += 255;   //Unsigned byte values
	if(g < 0) g += 255;
	if(b < 0) b += 255;
 
    float rn = (float)r / 255.0;  //Normalized colour values
    float gn = (float)g / 255.0;
    float bn = (float)b / 255.0;
    return glm::vec3(bn, gn, rn);
}

bool TextureOpenIL::loadOpenILImage(const char* filename) {
    if (ilLoadImage((ILstring)filename)) {			//if success
        imageWid = ilGetInteger(IL_IMAGE_WIDTH);
        imageHgt = ilGetInteger(IL_IMAGE_HEIGHT);
        imageChnls = ilGetInteger(IL_IMAGE_BPP);
        cout << "Texture successfully loaded:  Width = " << imageWid << " Height = " << imageHgt << endl;
    } else {
        cout << "Couldn't load texture. " << endl;
    }
    imageData = ilGetData();

    return true;
}

#ifndef TXCANVAS
#define TXCANVAS

#include <vector>
#include <iostream>
//#include <GL/glut.h>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLWidget>
#include "BasicTypes.h"

using namespace std;

class textureTile{
public:
	int width;
	int height;
	GLuint tex_id;
	vector< unsigned char > data;
	textureTile(int w, int h) {
		width = w;
		height = h;
	}
};


class TextureCanvas{
public:
	
	TextureCanvas();
	TextureCanvas(vector<color> pixels, int width);
	~TextureCanvas();
	void display();
    static int const maxSaneWidth = 4000;
    bool useTextures;
    bool useHilbert;
    
private:
    void loadPixelsToCard(vector<color> pixels, int w);
	void createEmptyTiles(int canvas_width, int canvas_height, int max_size);
	void drawTextureSquare();	
	point2D grid_position(int i, int width, int height, int max_size );
	GLuint loadTexture(textureTile& tile);
	
	point get_position(int index);
	void paint_square(point position, color c);
	void textureFreeRender();

    string build_peano_string(int length);
    point peano_position(int& index, string& path, point start);
    point hilbert_position(int index);
    void hilbert_coords(int n, int d, int &x, int &y);
    void rotate_hilbert_coords(int n, int &x, int &y, int rx, int ry);
	
	GLuint displayList;
	int width;
	int height;
	vector< vector< textureTile > > canvas;
    vector<color> colors;
};

#endif


#include "TextureCanvas.h"
#include "BasicTypes.h"
#include <iostream>

using namespace std;

TextureCanvas::TextureCanvas()
{
	useTextures = false;
	canvas.push_back(vector<textureTile>());
    //verify that textures can be allocated
	int max_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
    
	if(max_size > 0) useTextures = true;
}

TextureCanvas::TextureCanvas(vector<color> pixels, int w)
{
	useTextures = false;
	colors = vector<color>(pixels);
	//pad the end with white pixels, background color
	for(int i = 0; i <= w; ++i)
		pixels.push_back( color(128,128,128) );
    
    width = w;
	height = pixels.size() / width;
    
    //verify that textures can be allocated
	int max_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
	if(max_size > 0)
	{
		useTextures = true;
	
		//determine the size of the texture canvas
		int canvas_width = width / max_size + 1; //AKA ciel();
        //cout << "Initial Canvas Width:  " << canvas_width;
        canvas_width = min((maxSaneWidth / max_size + 1),canvas_width);
        //cout << " Sanity Checked:  " << canvas_width << endl;
        
        
		int canvas_height = height / max_size + 1;
		createEmptyTiles(canvas_width, canvas_height, max_size);
		
	
		for(int i = 0; i < pixels.size(); i++)
		{
            
			color c1 = pixels[i];
            
            int x = (i % width) / max_size; //(horizontal Index)
            int y = (i / width) / max_size; //(vertical Index)
            
            if (x >= canvas.size() || y >= canvas.size()) continue;
            
			vector<unsigned char>& current = canvas[x][y].data;
			current.push_back(c1.r);
			current.push_back(c1.g);
			current.push_back(c1.b);
		}
	
		for(unsigned int x = 0; x < canvas.size(); ++x)
		{
			for(unsigned int y = 0; y < canvas[x].size(); ++y)
			{
				loadTexture(canvas[x][y]);//tex_ids.push_back(
				canvas[x][y].data.clear();//the data has been loaded into the graphics card
			}
		}
	}
}

TextureCanvas::~TextureCanvas()
{
	for(unsigned int i = 0; i < canvas.size(); ++i)
		for(unsigned int k = 0; k < canvas[i].size(); ++k)
			glDeleteTextures(1, &canvas[i][k].tex_id );	
}

void TextureCanvas::createEmptyTiles(int canvas_width, int canvas_height, int max_size)
{
    
	for(int x = 0; x < canvas_width ; ++x)//populate canvas
	{
		canvas.push_back( vector< textureTile >() );	
		for(int y = 0; y < canvas_height; ++y)
		{
			int w = min(width - (x * max_size), max_size);
			int h = min(height - (y * max_size), max_size);
			canvas[x].push_back( textureTile(w, h) );
		}
	}	
}

void TextureCanvas::display()
{
	if(	useTextures )
		drawTextureSquare();
	else
		textureFreeRender();
}

void TextureCanvas::drawTextureSquare()//draws from canvas
{	
	int max_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
	glPushMatrix();
	//glTranslated(-1.0, 0.0, 0);
	for(unsigned int x =0; x < canvas.size(); ++x)
	{
		for(unsigned int y = 0; y < canvas[x].size(); ++y)
		{	
			textureTile& tile = canvas[x][y];
			glPushMatrix();
			glTranslated(x * max_size, y*max_size, 0);
			glColor3d(1.0,1.0,1.0);
		    glEnable (GL_TEXTURE_2D); /* enable texture mapping */
		    glBindTexture (GL_TEXTURE_2D, tile.tex_id); /* bind to our texture, has id of 13 */
			
		    glBegin (GL_QUADS);
		        glTexCoord2f (0.0f,0.0f); /* upper left corner of image */
		        glVertex3f (0.0f, 0.0f, 0.0f);
		        glTexCoord2f (1.0f, 0.0f); /* upper right corner of image */
		        glVertex3f (tile.width, 0.0f, 0.0f);
		        glTexCoord2f (1.0f, 1.0f); /* lower right corner of image */
		        glVertex3f (tile.width, tile.height, 0.0f);
		        glTexCoord2f (0.0f, 1.0f); /* lower left corner of image */
		        glVertex3f (0.0f, tile.height, 0.0f);
		    glEnd ();
		
		    glDisable (GL_TEXTURE_2D); /* disable texture mapping */
			glPopMatrix();
		}
	}	
	glPopMatrix();
}

GLuint TextureCanvas::loadTexture(textureTile& tile)
{
	GLuint tex_id;
	glGenTextures( 1, &tex_id );//TODO: could we store this in tile?
    glBindTexture (GL_TEXTURE_2D, tex_id);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_NEAREST
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);               
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, tile.width, tile.height, 0, GL_RGB, GL_UNSIGNED_BYTE, &tile.data[0]);
    
    //cout << "Load Texture: " << (unsigned int) tex_id << endl;
    tile.tex_id = tex_id;
    return tex_id;
}

point TextureCanvas::get_position(int index)
{
	int x = index % width;
	int y = index / width;
	return point(x, y, 0);
}

void TextureCanvas::paint_square(point position, color c)
{	
	glPushMatrix();
    	glColor3d(c.r /255.0, c.g /255.0, c.b /255.0); 
        glTranslated(position.x+1, position.y, position.z);
        glBegin(GL_QUADS);
	        glVertex3d(.0, .0, 0);
	        glVertex3d(-1.0, .0, 0);
	        glVertex3d(-1, -1, 0);
	        glVertex3d(.0, -1, 0);
	    glEnd();
	glPopMatrix();
}

void TextureCanvas::textureFreeRender()
{
    glPushMatrix();
	glTranslated(0,1,0);
		for(int i = 0; i < (int)colors.size(); i++)
		{
			point p1 = get_position( i );
			paint_square(p1, colors[i]);
		}	
	glPopMatrix();
}

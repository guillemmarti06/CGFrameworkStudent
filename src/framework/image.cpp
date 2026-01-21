#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete[] pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixelUnsafe( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		if (pixels) delete[] pixels;
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {
		if (pixels) delete[] pixels;

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "--- File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;

		if (tgainfo->data != NULL)
			delete[] tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete[] pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete[] tgainfo->data;
	delete tgainfo;

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		std::cerr << "--- Failed to save file: " << fullPath.c_str() << std::endl;
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	delete[] bytes;

	std::cout << "+++ File saved: " << fullPath.c_str() << std::endl;

	return true;
}

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete[] pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Function for drawing lines implemented
void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c)
{
    // STEP 1: Compute largest leg on triangle (notice that this distance equals the number of pixels to draw)
    
    int dx = x1 - x0;
    int dy = y1 - y0;

    int d = std::max(std::abs(dx), std::abs(dy)); // given formula in the google slides

    // if the line it's just a single point, we just paint the pixel (strange case)
    if (d == 0)
    {
        if (x0 >= 0 && x0 < (int)width && y0 >= 0 && y0 < (int)height)
            SetPixel((unsigned)x0, (unsigned)y0, c);
        return;
    }
    
    // STEP 2: Compute the direction step vector to advance through all the points (given formula in slides: v = (dx/d, dy/d))
    
    float vx = (float)dx / (float)d;
    float vy = (float)dy / (float)d;

    float x = (float)x0;
    float y = (float)y0;

    for (int i = 0; i <= d; ++i) // STEP 3: Starting in x0,y0 = A and iterate d times:
    {
        int px = (int)std::floor(x); // Paint pixel at [floor(x), floor(y)]
        int py = (int)std::floor(y);

        if (px >= 0 && px < (int)width && py >= 0 && py < (int)height)
            SetPixel((unsigned)px, (unsigned)py, c);

        x += vx;    // increment x and y by v to keep iterating
        y += vy;
    }
}

void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth,
                     bool isFilled, const Color& fillColor){
    // Fill the interior of the rectangle
    if (isFilled)
    {
        // Loop through the inner area to fill the interior pixels (exclude the border, we'll do it later)
        for (int j = y + borderWidth; j < y + h - borderWidth; ++j){
            for (int i = x + borderWidth; i < x + w - borderWidth; ++i){
                // Check framebuffer limits
                if (i >= 0 && i < (int)width && j >= 0 && j < (int)height){
                    SetPixel((unsigned)i, (unsigned)j, fillColor);
                }
            }
        }
    }

    // Now, draw the border with the desired thickness
    for (int b = 0; b < borderWidth; ++b){
        // Top and bottom edges
        for (int i = x; i < x + w; ++i){
            int top = y + b;               // Top border layer
            int bottom = y + h - 1 - b;    // Bottom border layer

            if (i >= 0 && i < (int)width){
                if (top >= 0 && top < (int)height)
                    SetPixel((unsigned)i, (unsigned)top, borderColor);

                if (bottom >= 0 && bottom < (int)height)
                    SetPixel((unsigned)i, (unsigned)bottom, borderColor);
            }
        }

        // Lastly, the left and right edges
        for (int j = y; j < y + h; ++j){
            int left = x + b;              // Left border layer
            int right = x + w - 1 - b;     // Right border layer

            if (j >= 0 && j < (int)height){
                if (left >= 0 && left < (int)width)
                    SetPixel((unsigned)left, (unsigned)j, borderColor);

                if (right >= 0 && right < (int)width)
                    SetPixel((unsigned)right, (unsigned)j, borderColor);
            }
        }
    }
}

// Each row stores the minX and maxX covered by the triangle at that height (struct from slides)
struct Cell{
    int minx = std::numeric_limits<int>::max();
    int maxx = std::numeric_limits<int>::min();
};

// Modified DDA: instead of painting pixels we update table[y].minx / maxx
void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int d = std::max(std::abs(dx), std::abs(dy));
    if (d == 0)
    {
        // edge w/ single point
        if (y0 >= 0 && y0 < (int)table.size())
        {
            table[y0].minx = std::min(table[y0].minx, x0);
            table[y0].maxx = std::max(table[y0].maxx, x0);
        }
        return;
    }

    float vx = (float)dx / (float)d;
    float vy = (float)dy / (float)d;

    float x = (float)x0;
    float y = (float)y0;

    for (int i = 0; i <= d; ++i)
    {
        int px = (int)std::floor(x);
        int py = (int)std::floor(y);

        // Update AET cell for this row (only if it is inside the table)
        if (py >= 0 && py < (int)table.size())
        {
            table[py].minx = std::min(table[py].minx, px);
            table[py].maxx = std::max(table[py].maxx, px);
        }

        x += vx;
        y += vy;
    }
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor){
    // Convert Vector2 to integer pixel coordinates
    int x0 = (int)std::round(p0.x);
    int y0 = (int)std::round(p0.y);
    int x1 = (int)std::round(p1.x);
    int y1 = (int)std::round(p1.y);
    int x2 = (int)std::round(p2.x);
    int y2 = (int)std::round(p2.y);

    // 1- Fill the triangle using AET
    if (isFilled){
        // Create the Active Edge Table, one cell per scanline
        std::vector<Cell> table;
        table.resize(height);

        // Scan the three triangle edges and update the table
        ScanLineDDA(x0, y0, x1, y1, table);
        ScanLineDDA(x1, y1, x2, y2, table);
        ScanLineDDA(x2, y2, x0, y0, table);

        // Fill the triangle row by row using minX & maxX
        for (int y = 0; y < (int)height; ++y){
            int minx = table[y].minx;
            int maxx = table[y].maxx;

            // If intersects the triangle
            if (minx <= maxx){
                // framebuffer limits
                minx = std::max(minx, 0);
                maxx = std::min(maxx, (int)width - 1);

                for (int x = minx; x <= maxx; ++x){
                    SetPixel((unsigned)x, (unsigned)y, fillColor);
                }
            }
        }
    }
    // 2- Draw triangle border (We did it at last because at first, the filling proccess was covering also the border)
    // So we draw the edges AFTER filling so the border is visible
    DrawLineDDA(x0, y0, x1, y1, borderColor);
    DrawLineDDA(x1, y1, x2, y2, borderColor);
    DrawLineDDA(x2, y2, x0, y0, borderColor);
}

// PAINT TOOL:

void Image::DrawImage(const Image& img, int x, int y)
{
    for (int j = 0; j < img.height; ++j){
        for (int i = 0; i < img.width; ++i){
            Color c = img.GetPixel(i, j);
            SetPixel(x + i, y + j, c);
        }
    }
}

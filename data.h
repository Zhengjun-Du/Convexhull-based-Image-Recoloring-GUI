#ifndef DATA_H
#define DATA_H

#include<QString>
#include<QObject>
#include<vector>
#include <QThread>
#include"utility.h"
#include "vec3.h"
#include <string>
using namespace std;

class Data : public QObject
{
    Q_OBJECT

public:
    Data();
    void OpenImage(QString fileName);
    void OpenPalette(string fileName);
    void reset();

	double *GetImage(bool isAfter = true) const { return isAfter ? recolored_image : original_image; }
	Polyhedron GetChangedPalette() { return changed_palette; }
	Polyhedron GetOriginalPalette() { return original_palette; }
	Polyhedron GetPalette() { return is_mvc_calculated ? changed_palette : original_palette; }

	vector<float> ComputeSingleVertexMVCWeights(int vid, const vec3 &x);
	vector<float> ComputeSingleVertexMVCWeightsForceInside(int vid, const vec3 &x);
	const void ComputeMVCWeights();

	vec3 RecolorSinglePixel(int vid, const vec3 &x);
	void Recolor();
	
    int GetFrameWidth() const { return image_width; }
    int GetFrameHeight() const{ return image_height; }
    int GetFrameDepth() const{ return image_depth; }

	void setPaletteColor(int id, QColor c);
	void resetPaletteColor(int id);
	void resetAllPaletteColors();

	void ExportOriginalVideo(string path);
	void ExportRecoloredImage(string path);
	void ExportChangedPalette(string path);
	void ImportChangedPalette(string path);
   
public slots:
signals:
    void updated();

private:
	double* recolored_image = nullptr;
	double* original_image = nullptr;
	
	Polyhedron changed_palette;
	Polyhedron original_palette;

	vector<vector<float> > mvc_weights;
	bool is_mvc_calculated;

	int image_width = 0;
	int image_height = 0;
	int image_depth = 0;
};

#endif // DATA_H
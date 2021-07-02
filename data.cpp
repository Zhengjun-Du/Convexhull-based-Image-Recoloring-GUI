#include "data.h"
#include "utility.h"
#include <QFile>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <QProgressDialog>
#include <QThread>
#include <QMessagebox>
#include <QTime>
#include <omp.h>
#include <map>
#include "my_util.h"
#include <fstream>

using namespace std;

#define M_PI 3.14159265358979323846
#define ESP 1e-6


Data::Data()
{

}

void Data::OpenImage(QString fileName)
{
	QImage image(fileName);
	image_height = image.height();
	image_width = image.width();
	image_depth = 3;

	long long totalSize = image_width * image_height * image_depth;
	if (recolored_image != nullptr)
	{
		delete[] recolored_image;
		recolored_image = nullptr;
	}
	if (original_image != nullptr)
	{
		delete[] original_image;
		original_image = nullptr;
	}

	recolored_image = new double[totalSize];
	original_image = new double[totalSize];

	for (int i = 0; i < image_height; i++) {
		QRgb * line = (QRgb *)image.scanLine(i);
		for (int j = 0; j < image_width; j++) {
			original_image[i*image_width*image_depth + j * image_depth + 0] = qRed(line[j]) / 255.0;
			original_image[i*image_width*image_depth + j * image_depth + 1] = qGreen(line[j]) / 255.0;
			original_image[i*image_width*image_depth + j * image_depth + 2] = qBlue(line[j]) / 255.0;
		}

	}

	memcpy(recolored_image, original_image, sizeof(double)*totalSize);


	emit updated();
}

void Data::OpenPalette(string fileName)
{
	ifstream in(fileName);
	double v1, v2, v3;
	char c;

	vector<vec3> rgb_ch_vertices;
	vector<int3> rgb_ch_faces;
	while (!in.eof()) {
		in >> c;
		if (c == 'v') {
			in >> v1 >> v2 >> v3;
			vec3 vert(v1, v2, v3);
			rgb_ch_vertices.push_back(vert);
		}
		else if (c == 'f') {
			in >> v1 >> v2 >> v3;
			int3 face(v1 - 1, v2 - 1, v3 - 1);
			rgb_ch_faces.push_back(face);
		}
		else
			break;

		c = 'x';
	}

	changed_palette.vertices = rgb_ch_vertices;
	changed_palette.faces = rgb_ch_faces;

	original_palette = changed_palette;

	in.close();
	emit updated();
}

void Data::reset()
{
	mvc_weights.clear();
	mvc_weights.resize(image_width*image_height);
	is_mvc_calculated = false;
}

inline double angle(const myfloat3 & u1, const myfloat3 & u2) {
	float u_norm = (u1 - u2).norm();
	return 2.0 * asin(u_norm / 2.0);
}

void Data::setPaletteColor(int id, QColor c) {
	double r = qRed(c.rgb()) / 255.0;
	double g = qGreen(c.rgb()) / 255.0;
	double b = qBlue(c.rgb()) / 255.0;
	changed_palette.vertices[id] = vec3(r, g, b);
}

vector<float> Data::ComputeSingleVertexMVCWeights(int vid, const vec3 &o) {

	const double eps = 1e-6;

	const Polyhedron &poly = original_palette;
	int vcnt = poly.vertices.size();

	vector<float> d(vcnt);
	vector<vec3> u(vcnt);
	vector<float> weights(vcnt);
	vector<float> w_weights(vcnt);

	for (int v = 0; v < vcnt; v++) {
		d[v] = 0;
		u[v] = vec3(0, 0, 0);
		weights[v] = 0;
	}

	for (int v = 0; v < vcnt; v++) {
		d[v] = (o - poly.vertices[v]).norm();
		if (d[v] < eps) {
			weights[v] = 1.0;
			return weights;
		}
		u[v] = (poly.vertices[v] - o) / d[v];
	}

	float sumWeights = 0.0f;
	vector<float> w(vcnt);

	int fcnt = poly.faces.size();
	for (int i = 0; i < fcnt; i++) {
		int vids[3] = { poly.faces[i].x, poly.faces[i].y, poly.faces[i].z };
		float l[3] = { (u[vids[1]] - u[vids[2]]).norm(), (u[vids[2]] - u[vids[0]]).norm(), (u[vids[0]] - u[vids[1]]).norm() };
		float theta[3] = { 2.0f * asin(l[0] / 2.0f),2.0f * asin(l[1] / 2.0f),2.0f * asin(l[2] / 2.0f) };
		float sintheta[3] = { sin(theta[0]),sin(theta[1]),sin(theta[2]) };

		float h = (theta[0] + theta[1] + theta[2]) / 2.0;
		float sinh = sin(h);

		if (fabs(M_PI - h) < eps * 1000) {
			// x lies inside the triangle t , use 2d barycentric coordinates :
			float w[3];
			for (int i = 0; i < 3; ++i) {
				w[i] = sintheta[i] * d[vids[(i + 2) % 3]] * d[vids[(i + 1) % 3]];
			}
			sumWeights = w[0] + w[1] + w[2];

			weights[vids[0]] = w[0] / sumWeights;
			weights[vids[1]] = w[1] / sumWeights;
			weights[vids[2]] = w[2] / sumWeights;

			return weights;
		}

		float det = dot(u[vids[0]], cross(u[vids[1]], u[vids[2]]));
		float sign_det = signbit(det) ? -1.0f : 1.0f;

		float s[3], c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = 2 * sinh * sin(h - theta[i]) / (sintheta[(i + 1) % 3] * sintheta[(i + 2) % 3]) - 1;
			s[i] = sign_det * sqrt(1 - c[i] * c[i]);
		}

		if (fabs(s[0]) > eps && fabs(s[1]) > eps && fabs(s[2]) > eps)
		{
			for (int i = 0; i < 3; ++i) {
				float w_numerator = theta[i] - c[(i + 1) % 3] * theta[(i + 2) % 3] - c[(i + 2) % 3] * theta[(i + 1) % 3];
				float w_denominator = d[vids[i]] * sintheta[(i + 1) % 3] * s[(i + 2) % 3];
				float w_i = w_numerator / w_denominator;
				sumWeights += w_i;
				w_weights[vids[i]] += w_i;
			}
		}
	}

	for (int v = 0; v < vcnt; ++v)
		weights[v] = w_weights[v] / sumWeights;

	return weights;
}

const void Data::ComputeMVCWeights() {

#pragma omp parallel for num_threads(32)

	for (int i = 0; i < image_width * image_height * image_depth; i += image_depth) {
		vec3 x(
			original_image[i + 0],
			original_image[i + 1],
			original_image[i + 2]
		);

		int vid = i / image_depth;
		vector<float> weights = ComputeSingleVertexMVCWeights(vid, x);

		for (int j = 0; j < weights.size(); j++) {
			if (weights[j] < 0 || weights[j] > 1) {
				weights = ComputeSingleVertexMVCWeightsForceInside(vid, x);
				break;
			}
		}
		mvc_weights[vid] = weights;
	}

	is_mvc_calculated = true;
	QMessageBox::information(NULL, "Info", "MVC finished", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

vec3 Data::RecolorSinglePixel(int vid, const vec3 &x) {
	vec3 ans(0.f, 0.f, 0.f);

	for (int i = 0; i < mvc_weights[vid].size(); i++) {

		float w = mvc_weights[vid][i];
		vec3 v;
		if (is_mvc_calculated)
			v = changed_palette.vertices[i];
		else
			v = original_palette.vertices[i];
		ans = ans + v * w;
	}
	return ans;
}

void Data::Recolor() {
	if (!is_mvc_calculated)
		return;

	QTime time1;
	time1.start();

#pragma omp parallel for num_threads(32)
	for (int i = 0; i < image_width * image_height * image_depth; i += image_depth)
	{
		int frame_pix_offset = image_width * image_height * image_depth;
		vec3 x(
			original_image[i + 0],
			original_image[i + 1],
			original_image[i + 2]
		);

		vec3 y = RecolorSinglePixel(i / image_depth, x);

		recolored_image[i + 0] = y[0];
		recolored_image[i + 1] = y[1];
		recolored_image[i + 2] = y[2];
	}

	qDebug() << "Recolor time: " << time1.elapsed() / 1000.0 << "s";
	emit updated();
}

void Data::ExportOriginalVideo(string path) {
	if (!is_mvc_calculated)
		return;

	QTime time;
	time.start();

	int k = 0;

	QSize size(image_height, image_width);
	QImage image(size, QImage::Format_ARGB32);

#pragma omp parallel for num_threads(32)
	for (int i = 0; i < image_width * image_height * image_depth; i += image_depth)
	{
		vec3 x(
			original_image[i + 0],
			original_image[i + 1],
			original_image[i + 2]
		);


		int row = i / (image_height * image_depth);
		int col = (i - row*image_height * image_depth) / image_depth;
		image.setPixel(col, row, qRgb(x[0] * 255, x[1] * 255, x[2] * 255));

	}
	image.save((path + "/original.png").c_str(), "PNG", 100);
}

void Data::ExportRecoloredImage(string path) {


	uchar *data = new uchar[image_width * image_height * image_depth];

	#pragma omp parallel for num_threads(32)
	for (int i = 0; i < image_width * image_height * image_depth; i++)
	{
		int x = static_cast<int>(recolored_image[i] * 255);
		if (x > 255) x = 255;
		if (x < 0) x = 0;

		data[i] = static_cast<uchar>(x);
	}

	cout << endl;
	for (int i = 0; i < 3; i++)
		cout << (int)data[i * 3] << "," << (int)data[i * 3 + 1] << "," << (int)data[i * 3 + 2] << endl;

	QImage image(data, image_width, image_height, image_width * 3, QImage::Format_RGB888);
	image.save((path + "/recolored.png").c_str(), "PNG", 100);
}


//Yili Wang's code
vector<float> Data::ComputeSingleVertexMVCWeightsForceInside(int vid, const vec3 &x) {

	float min_distance = FLT_MAX;
	vec3 close_point;

	for (int i = 0; i < original_palette.faces.size(); i++)
	{
		int3 triangle = original_palette.faces[i];
		vec3 triangle_vertices[3];

		triangle_vertices[0] = original_palette.vertices[triangle.x];
		triangle_vertices[1] = original_palette.vertices[triangle.y];
		triangle_vertices[2] = original_palette.vertices[triangle.z];
		vec3 my_close_point = closesPointOnTriangle(triangle_vertices, x);
		float my_dis = (x - my_close_point).sqrnorm();

		if (my_dis < min_distance) {
			min_distance = my_dis;
			close_point = my_close_point;
		}
	}

	vec3 projection(close_point[0], close_point[1], close_point[2]);
	return ComputeSingleVertexMVCWeights(vid, projection);
}

void Data::resetPaletteColor(int id) {
	changed_palette.vertices[id] = original_palette.vertices[id];
	Recolor();
	emit updated();
}

void Data::resetAllPaletteColors() {
	for (int i = 0; i < changed_palette.vertices.size(); i++)
		changed_palette.vertices[i] = original_palette.vertices[i];
}

void Data::ImportChangedPalette(string path) {
	ifstream ifs(path);
	int n; ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> changed_palette.vertices[i][0] >> changed_palette.vertices[i][1]
			>> changed_palette.vertices[i][2];
	}
	emit updated();
}


void Data::ExportChangedPalette(string path) {
	ofstream ofs(path);
	ofs << changed_palette.vertices.size() << endl;
	for (int i = 0; i < changed_palette.vertices.size(); i++) {

		ofs << changed_palette.vertices[i][0] << " " <<
			changed_palette.vertices[i][1] << " " <<
			changed_palette.vertices[i][2] << " " << endl;
	}
}
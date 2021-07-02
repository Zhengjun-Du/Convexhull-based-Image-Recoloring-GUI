#include "paletteviewwidget.h"

#include<QTimer>
#include<QDebug>
#include<cmath>
#include<QMouseEvent>
#include<QCryptographicHash>
#include <QColorDialog>
#include <QMenu>
#include "qrgb.h"
#include "math.h"
#include <algorithm>
#include "Qt-Color-Widgets\include\QtColorWidgets\color_dialog.hpp"
using namespace std;
using namespace color_widgets;

// Widget shows a palette-time view

PaletteViewWidget::PaletteViewWidget(QWidget *parent) : OpenGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){ blink = !blink; update(); });
    timer->start(800);

	pMenu = new QMenu(this);
	QAction *pResetTask = new QAction(tr("reset this color"), this);
	QAction *pResetAllTask = new QAction(tr("reset all color"), this);
	pMenu->addAction(pResetTask);
	pMenu->addAction(pResetAllTask);

	connect(pResetTask, SIGNAL(triggered()), this, SLOT(resetPaletteColor()));
	connect(pResetAllTask, SIGNAL(triggered()), this, SLOT(resetAllPaletteColors()));
}

void PaletteViewWidget::setTime(int t) {
	time = t;
	if (selected_vid != -1 ) {
		data->Recolor();
	}
}

void PaletteViewWidget::getColor(QColor c) {
	int r = qRed(c.rgb());
	int g = qGreen(c.rgb());
	int b = qBlue(c.rgb());
	
	if (selected_vid != -1) {
		data->setPaletteColor(selected_vid, c);
		update();
		data->Recolor();
	}
}

void PaletteViewWidget::paintGL()
{
    OpenGLWidget::paintGL();

    glScalef(scale, scale, scale);

	int w = width(), h = height();
	double aspect = w*1.0 / h;

	//绘制白色矩形框
	/*
	glBegin(GL_LINES);

	for (int p = 0; p <= 1; p++)
	{
		double q = p - .5;
		glColor3f(1., 1., 1.);
		glVertex3f(q, -.5, 0.);
		glColor3f(1., 1., 1.);
		glVertex3f(q, .5, 0.);
		glColor3f(1., 1., 1.);
		glVertex3f(-.5, q, 0.);
		glColor3f(1., 1., 1.);
		glVertex3f(.5, q, 0.);
	}
	glEnd();
	*/

	Polyhedron ori_poly = data->GetOriginalPalette();
	Polyhedron cur_poly = data->GetChangedPalette();
	

	float space = 0.02;
	float recw = 0.06;
	float x = -0.5, y = 0.45, y1 = 0.1;
	palette_pos.clear();
	
	for (int i = 0; i < ori_poly.vertices.size(); i++)
	{
		vec3 overt = ori_poly.vertices[i];
		float or = overt[0], og = overt[1], ob = overt[2];
		glColor3f(or , og, ob);
		glRectf(x, y, x + recw, y - recw*aspect);
		
		vec3 cvert = cur_poly.vertices[i];
		float cr = cvert[0], cg = cvert[1], cb = cvert[2];

		if (selected_vid == i) {
			double ex = 0.006;
			glColor3f(0.8, 0, 0);
			//glRectf(x- ex, y1+ ex*aspect, x + recw+ ex, y1 - recw*aspect- ex*aspect);
			glRectf(x, y1-recw*aspect - 0.01*aspect, x + recw, y1 - recw*aspect - 0.02*aspect);

		}

		glColor3f(cr , cg, cb);
		glRectf(x, y1, x + recw, y1 - recw*aspect);

		palette_pos.push_back(vec3(x + recw / 2, y1 - recw*aspect / 2, 0));

		x += recw + space;
	}
}

void PaletteViewWidget::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton) {
		//1. 坐标系变换，将屏幕坐标系 -> opengl坐标系
		int w = width(), h = height();
		double half_w = w / 2.0, half_h = h / 2.0;
		double new_x = (ev->x() - half_w) / half_w / scale;
		double new_y = -(ev->y() - half_h) / half_h / scale;

		Polyhedron cur_poly = data->GetChangedPalette();

		//2. 找到离鼠标点击最近的点
		double aspect = w*1.0 / h;
		float recw = 0.07;
		float rech = recw * aspect;

		for (int i = 0; i < cur_poly.vertices.size(); i++)
		{
			double x = palette_pos[i][0];
			double y = palette_pos[i][1];

			double dis1 = fabs(x - new_x);
			double dis2 = fabs(y - new_y);

			if (dis1 < recw / 2 && dis2 < rech / 2) {
				selected_vid = i;

				double r = cur_poly.vertices[i][0];
				double g = cur_poly.vertices[i][1];
				double b = cur_poly.vertices[i][2];

				int R = r * 255, G = g * 255, B = b * 255;
				QColor c;
				c.setRed(R);
				c.setGreen(G);
				c.setBlue(B);
				Q_EMIT setColor(c);
				break;
			}
		}
	}
	else if (ev->button() == Qt::RightButton) {
		pMenu->exec(cursor().pos());
	}
}

void PaletteViewWidget::resetPaletteColor() {
	if (selected_vid != -1) {
		data->resetPaletteColor(selected_vid);
		emit update();
	}
}
void PaletteViewWidget::resetAllPaletteColors() {
	if (selected_vid != -1) {
		data->resetAllPaletteColors();
		emit update();
	}
}
#ifndef RGBWIDGET_H
#define RGBWIDGET_H

#include<opengl3dwidget.h>
#include"data.h"
#include<QSharedMemory>

class RGBWidget : public OpenGL3DWidget
{
protected:
    void paintGL() Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) override;

public:
    RGBWidget();

private:
	QSharedMemory sharedMemory;
	static double _rotate_x;
	static double _rotate_y;
	static double _rotate_z;
};

#endif // RGBWIDGET_H

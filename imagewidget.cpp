#include "imagewidget.h"
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QPainter>


ImageWidget::ImageWidget(bool _isAfter, QWidget *parent) : QWidget(parent), isAfter(_isAfter)
{
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);

	if (data == nullptr) return;

	int width = data->GetFrameWidth();
	int height = data->GetFrameHeight();
	int depth = data->GetFrameDepth();
	double *video = data->GetImage(isAfter);

	QPainter painter(this);

	uchar *data = new uchar[width * height * depth];

	for (int i = 0; i < width * height * depth; i++)
	{
		int x = static_cast<int>(video[i] * 255);
		if (x > 255) x = 255;
		if (x < 0) x = 0;

		data[i] = static_cast<uchar>(x);
	}

	cout << endl;
	for (int i = 0; i < 3; i++)
		cout << (int)data[i * 3] << "," << (int)data[i * 3 + 1] << "," << (int)data[i * 3 + 2] << endl;

	QImage im(data, width, height, width*3, QImage::Format_RGB888);

	cout << im.height() << "," << im.width() << "," << im.depth() << endl;


	painter.setPen(QPen(Qt::red, 3));

	painter.drawImage(QRectF(0, 0, width, height), im);

	//painter.drawRect(0, 0, width, height);

	painter.end();
	delete[] data;
}

void ImageWidget::setTime(int t)
{
	time = t;

	update();
}

void ImageWidget::update()
{
	QWidget::update();

	int w = data->GetFrameWidth();
	int h = data->GetFrameHeight();

	setMaximumSize(w, h);
	setMinimumSize(w, h);
}

void ImageWidget::setData(Data *value)
{
	data = value;
	connect(value, &Data::updated, this, &ImageWidget::update);
}

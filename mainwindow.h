#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <data.h>
#include <QSlider>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
	QDockWidget *imageBeforeDockWidget = nullptr;
	QDockWidget *imageAfterDockWidget = nullptr;
	void exportImage();
	void importPalette();
	void exportPalette();


    void openFile(bool merge);
    Data *data = nullptr;
    QSlider *slider = nullptr;
    QSlider *mergeStepSlider = nullptr;

   // QString title = "Video Recoloring Tool (Build " __DATE__ " " __TIME__ ")";
	QString title = "Image Recoloring Tool";

	bool isPlaying = false;
};

#endif // MAINWINDOW_H

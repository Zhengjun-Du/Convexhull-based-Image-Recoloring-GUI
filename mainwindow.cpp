#include<QWidget>
#include<QPushButton>
#include<QVBoxLayout>
#include<QSlider>
#include<QLabel>
#include<Q3DSurface>
#include<QtDataVisualization>
#include<QDockWidget>
#include<QTextEdit>
#include<QListWidget>
#include<QFileDialog>
#include<QCheckBox>
#include<QSplitter>
#include<QSizePolicy>
#include<QProgressDialog>
#include <QGroupBox>

#include "data.h"

#include "mainwindow.h"
#include "imagewidget.h"
#include "openglwidget.h"
#include "rgbwidget.h"
#include "paletteviewwidget.h"
#include "Qt-Color-Widgets\include\QtColorWidgets\color_dialog.hpp"

// Setup UI

#include <QColorDialog>

using namespace color_widgets;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(title);
	setGeometry(100, 100, 520, 520);

    data = new Data();

    QWidget *mainWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();

	//top main menu==============================================================================================
    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    QPushButton *openImageAndPaletteBtn = new QPushButton("Open Image and Palette");
    QPushButton *CalcMvcBtn = new QPushButton("Calc MVC ");
	QPushButton *rgbChViewButton = new QPushButton("RGB view");

    firstRowLayout->addWidget(openImageAndPaletteBtn);
    firstRowLayout->addWidget(CalcMvcBtn);
	firstRowLayout->addWidget(rgbChViewButton);
	mainLayout->addLayout(firstRowLayout);
	//top main menu==============================================================================================

	//original video and recolored video=========================================================================
    ImageWidget *recolored_image = new ImageWidget(true);
	recolored_image->setData(data);

    ImageWidget *original_image = new ImageWidget(false);
	original_image->setData(data);

    videoBeforeDockWidget = new QDockWidget();

    videoBeforeDockWidget->setWidget(original_image);
    videoBeforeDockWidget->setWindowTitle("Original Image");
    addDockWidget(Qt::TopDockWidgetArea, videoBeforeDockWidget);
    videoBeforeDockWidget->setFloating(true);
    videoBeforeDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	videoBeforeDockWidget->setGeometry(760, 100, 400, 400);
	videoBeforeDockWidget->hide();

    videoAfterDockWidget = new QDockWidget();
    videoAfterDockWidget->setWidget(recolored_image);
    videoAfterDockWidget->setWindowTitle("Recolored Image");
    videoAfterDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::TopDockWidgetArea, videoAfterDockWidget);
    videoAfterDockWidget->setFloating(true);
	videoAfterDockWidget->setGeometry(760, 100, 400, 400);
	videoAfterDockWidget->hide();
	//original video and recolored video=========================================================================


	//import and export =========================================================================================
	QHBoxLayout *secondRowLayout = new QHBoxLayout();

	QPushButton *exportImageeBtn = new QPushButton("Export Image");
	QPushButton *importPaletteButton = new QPushButton("Import Palette");
	QPushButton *exportPaletteButton = new QPushButton("Export Palette");

	secondRowLayout->addWidget(exportImageeBtn);
	secondRowLayout->addWidget(importPaletteButton);
	secondRowLayout->addWidget(exportPaletteButton);
	//import and export =========================================================================================


	//video play and vis=========================================================================================
    QHBoxLayout *showVideoDataLayout = new QHBoxLayout();
    QCheckBox *showVideoDataCheck= new QCheckBox();
    QLabel *showVideoDataLabel = new QLabel("25.0%");
    QSlider *videoPreviewSlider = new QSlider(Qt::Horizontal);

    showVideoDataCheck->setCheckState(Qt::Checked);
    showVideoDataCheck->setText("Visualize Image Data Points");

    videoPreviewSlider->setMinimum(0);
    videoPreviewSlider->setMaximum(1000);
    videoPreviewSlider->setValue(250);
    showVideoDataLabel->setBuddy(videoPreviewSlider);
    showVideoDataLabel->setWordWrap(false);

    connect(videoPreviewSlider, &QSlider::valueChanged,
            [=](const int &newValue){showVideoDataLabel->setText(QString::number(newValue / 10., 'f', 1) + "%");} );

    showVideoDataLayout->addWidget(showVideoDataCheck);
    showVideoDataLayout->addWidget(videoPreviewSlider);
    showVideoDataLayout->addWidget(showVideoDataLabel);


    QHBoxLayout *timeWindowLayout = new QHBoxLayout();
    QSlider *timeWindowSlider = new QSlider(Qt::Horizontal);
    timeWindowSlider->setMinimum(-3000);
    timeWindowSlider->setMaximum(10000);

    slider = new QSlider(Qt::Horizontal);
    slider->setTickInterval(1);

	QPushButton *autoPlayButton = new QPushButton("Play ");
	QTimer *autoPlayTimer = new QTimer();

	autoPlayTimer->setInterval(80);
	//video play and vis=========================================================================================

	QGroupBox *groupBox = new QGroupBox(tr("Parameters setting"));
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(secondRowLayout);
	vbox->addLayout(showVideoDataLayout);
	groupBox->setLayout(vbox);

    
    QDockWidget *dockRBGWidget = new QDockWidget();
    RGBWidget *rgbWidget = new RGBWidget();
    rgbWidget->setMinimumSize(400, 400);
    rgbWidget->setData(data);

    dockRBGWidget->setWidget(rgbWidget);
    dockRBGWidget->setWindowTitle("RGB");
    addDockWidget(Qt::BottomDockWidgetArea, dockRBGWidget);
    dockRBGWidget->setFloating(true);
	//dockRBGWidget->setGeometry(920,418,500,430);
	dockRBGWidget->hide();

    QDockWidget *dockPaletteWidget = new QDockWidget();
    PaletteViewWidget *paletteWidget = new PaletteViewWidget();
    paletteWidget->setMinimumSize(500, 150);
    paletteWidget->setData(data);

    dockPaletteWidget->setWidget(paletteWidget);
    dockPaletteWidget->setWindowTitle("Palette");
    addDockWidget(Qt::RightDockWidgetArea, dockPaletteWidget);
    dockPaletteWidget->setFloating(true);
	dockPaletteWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

    slider->setMinimum(0);
    slider->setMaximum(0);

	color_widgets::ColorDialog* dialog = new color_widgets::ColorDialog();
	dialog->setWindowTitle("Color picker");

	QDockWidget *dockColorWidget = new QDockWidget();

	dockColorWidget->setWidget(dialog);
	dockColorWidget->setWindowTitle("Color picker");
	addDockWidget(Qt::RightDockWidgetArea, dockColorWidget);
	dockColorWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

	mainLayout->addWidget(groupBox);
	mainLayout->addWidget(dockPaletteWidget);
	mainLayout->addWidget(dockColorWidget);
	
	mainWidget->setLayout(mainLayout);
	this->setCentralWidget(mainWidget);

    connect(slider, &QSlider::valueChanged, original_image, &ImageWidget::setTime);
    connect(slider, &QSlider::valueChanged, recolored_image, &ImageWidget::setTime);
    connect(slider, &QSlider::valueChanged, rgbWidget, &RGBWidget::setTime);
    connect(slider, &QSlider::valueChanged, paletteWidget, &PaletteViewWidget::setTime);

    connect(showVideoDataCheck, &QCheckBox::stateChanged, rgbWidget, &RGBWidget::setShowVideoData);
    connect(showVideoDataCheck, &QCheckBox::stateChanged, paletteWidget, &PaletteViewWidget::setShowVideoData);

    connect(videoPreviewSlider, &QSlider::valueChanged, rgbWidget, &RGBWidget::setPreview);
    connect(videoPreviewSlider, &QSlider::valueChanged, paletteWidget, &PaletteViewWidget::setPreview);

    connect(openImageAndPaletteBtn, &QPushButton::clicked, [=](){this->openFile(true);});
    connect(CalcMvcBtn, &QPushButton::clicked, data, &Data::ComputeMVCWeights);

	connect(dialog, &ColorDialog::colorChanged, paletteWidget, &PaletteViewWidget::getColor);
	connect(paletteWidget, &PaletteViewWidget::setColor, dialog, &ColorDialog::setColor);
	connect(rgbChViewButton, &QPushButton::clicked, [=]() {dockRBGWidget->show(); });

	connect(exportImageeBtn, &QPushButton::clicked, [=]() { this->exportImage(); });
	connect(importPaletteButton, &QPushButton::clicked, [=]() { this->importPalette(); });
	connect(exportPaletteButton, &QPushButton::clicked, [=]() { this->exportPalette(); });
}

MainWindow::~MainWindow()
{
}

// open video & poly file
// TODO: replace input video file with H264 encoded video
void MainWindow::openFile(bool merge)
{
    if(data == nullptr) return;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("*.jpg *.png"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec())
    {
        QStringList fileName = dialog.selectedFiles();

        for(auto s : fileName)
        {
            data->OpenImage(QString(s));
        }
		videoBeforeDockWidget->show();
		videoAfterDockWidget->show();
    }
    else
    {
        return;
    }

    dialog.setNameFilter("*.obj");

    // if shows merge step, open several poly files instead of one
    dialog.setFileMode(merge ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);

    if (dialog.exec())
    {
        QStringList fileName = dialog.selectedFiles();

        fileName.sort();
        data->reset();
        if(mergeStepSlider != nullptr)
            mergeStepSlider->setMaximum(fileName.size() - 1);

        int i = 0;
        for(auto s : fileName)
        {
            data->OpenPalette(s.toStdString());
            i++;
        }
    }
}

void MainWindow::exportImage() {
	if (data == nullptr) return;

	QFileDialog fileDialog(this);
	fileDialog.setFileMode(QFileDialog::DirectoryOnly);
	QStringList dirName;
	if (fileDialog.exec() == QDialog::Accepted)
	{
		dirName = fileDialog.selectedFiles();
		string dirNamestr = dirName[0].toStdString();

		string recolor_dir = dirNamestr;
		data->ExportRecoloredImage(recolor_dir);
	}
}

void MainWindow::importPalette() {
	if (data == nullptr) return;

	QFileDialog fileDialog(this);
	fileDialog.setFileMode(QFileDialog::QFileDialog::ExistingFile);
	fileDialog.setNameFilter(tr("*.txt"));
	fileDialog.setViewMode(QFileDialog::Detail);

	QStringList dirName;
	if (fileDialog.exec() == QDialog::Accepted)
	{
		dirName = fileDialog.selectedFiles();
		string dirNamestr = dirName[0].toStdString();
		data->ImportChangedPalette(dirNamestr);
	}
}

void MainWindow::exportPalette() {
	if (data == nullptr) return;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Data"), ".", tr("txt File (*.txt)"));
	data->ExportChangedPalette(fileName.toStdString());
}
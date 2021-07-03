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

	//original image and recolored image=========================================================================
    ImageWidget *recolored_image = new ImageWidget(true);
	recolored_image->setData(data);

    ImageWidget *original_image = new ImageWidget(false);
	original_image->setData(data);

    imageBeforeDockWidget = new QDockWidget();
    imageBeforeDockWidget->setWidget(original_image);
    imageBeforeDockWidget->setWindowTitle("Original Image");
    addDockWidget(Qt::TopDockWidgetArea, imageBeforeDockWidget);
    imageBeforeDockWidget->setFloating(true);
    imageBeforeDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	imageBeforeDockWidget->setGeometry(760, 100, 400, 400);
	imageBeforeDockWidget->hide();

    imageAfterDockWidget = new QDockWidget();
    imageAfterDockWidget->setWidget(recolored_image);
    imageAfterDockWidget->setWindowTitle("Recolored Image");
    imageAfterDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::TopDockWidgetArea, imageAfterDockWidget);
    imageAfterDockWidget->setFloating(true);
	imageAfterDockWidget->setGeometry(760, 100, 400, 400);
	imageAfterDockWidget->hide();
	//original image and recolored image=========================================================================

	//import and export =========================================================================================
	QHBoxLayout *secondRowLayout = new QHBoxLayout();

	QPushButton *exportImageeBtn = new QPushButton("Export Image");
	QPushButton *importPaletteButton = new QPushButton("Import Palette");
	QPushButton *exportPaletteButton = new QPushButton("Export Palette");

	secondRowLayout->addWidget(exportImageeBtn);
	secondRowLayout->addWidget(importPaletteButton);
	secondRowLayout->addWidget(exportPaletteButton);
	//import and export =========================================================================================

	//image play and vis=========================================================================================
    QHBoxLayout *showImageDataLayout = new QHBoxLayout();
    QCheckBox *showImageDataCheck= new QCheckBox();
    QLabel *showImageDataLabel = new QLabel("25.0%");
    QSlider *imagePreviewSlider = new QSlider(Qt::Horizontal);

    showImageDataCheck->setCheckState(Qt::Checked);
    showImageDataCheck->setText("Visualize Image Data Points");

    imagePreviewSlider->setMinimum(0);
    imagePreviewSlider->setMaximum(1000);
    imagePreviewSlider->setValue(250);
    showImageDataLabel->setBuddy(imagePreviewSlider);
    showImageDataLabel->setWordWrap(false);

    connect(imagePreviewSlider, &QSlider::valueChanged,
            [=](const int &newValue){showImageDataLabel->setText(QString::number(newValue / 10., 'f', 1) + "%");} );

    showImageDataLayout->addWidget(showImageDataCheck);
    showImageDataLayout->addWidget(imagePreviewSlider);
    showImageDataLayout->addWidget(showImageDataLabel);

    QHBoxLayout *timeWindowLayout = new QHBoxLayout();
    QSlider *timeWindowSlider = new QSlider(Qt::Horizontal);
    timeWindowSlider->setMinimum(-3000);
    timeWindowSlider->setMaximum(10000);

    slider = new QSlider(Qt::Horizontal);
    slider->setTickInterval(1);

	QPushButton *autoPlayButton = new QPushButton("Play ");
	QTimer *autoPlayTimer = new QTimer();

	autoPlayTimer->setInterval(80);
	//image play and vis=========================================================================================

	QGroupBox *groupBox = new QGroupBox(tr("Parameters setting"));
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(secondRowLayout);
	vbox->addLayout(showImageDataLayout);
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

    connect(showImageDataCheck, &QCheckBox::stateChanged, rgbWidget, &RGBWidget::setShowImageData);
    connect(showImageDataCheck, &QCheckBox::stateChanged, paletteWidget, &PaletteViewWidget::setShowImageData);

    connect(imagePreviewSlider, &QSlider::valueChanged, rgbWidget, &RGBWidget::setPreview);
    connect(imagePreviewSlider, &QSlider::valueChanged, paletteWidget, &PaletteViewWidget::setPreview);

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

// open image & poly file
// TODO: replace input image file with H264 encoded image
void MainWindow::openFile(bool merge){
    if(data == nullptr) return;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("*.jpg *.png"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()){
        QStringList fileName = dialog.selectedFiles();

        for(auto s : fileName){
            data->OpenImage(QString(s));
        }
		imageBeforeDockWidget->show();
		imageAfterDockWidget->show();
    }
    else{
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
	if (fileDialog.exec() == QDialog::Accepted){
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
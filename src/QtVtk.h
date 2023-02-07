#pragma once

#include "stdafx.h"
#include "QtVtk.h"
#include <QtWidgets/QApplication>
#include <vtkoutputwindow.h>
#include<windows.h>
#include<Leap.h>

#include <QtWidgets/QMainWindow>
#include "ui_QtVtk.h"

#include <vtkSmartPointer.h>
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkOutlineFilter.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkDataSet.h>
#include <vtkTransform.h>
#include <vtkAxesActor.h>
#include <vtkAutoInit.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <VtkCell.h>
#include <vtkLine.h>
#include <vtkTriangle.h>
#include <vtkTriangleStrip.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkPolygon.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include<vtkBMPReader.h>
#include<vtkTexture.h>
#include<vtkWindowToImageFilter.h>
#include<vtkBMPWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include<vtkInteractorStyleTrackballCamera.h>



class QtVtk : public QMainWindow
{
	Q_OBJECT
	
public:
	QtVtk(QWidget *parent = Q_NULLPTR);


private:
	Ui::QtVtkClass ui;

private slots:
	void QtVtk::setCamera();
	void QtVtk::setpts();
	void QtVtk::clouds_actor();
	void QtVtk::lines_actor();
	void QtVtk::shelter_actor();
	void QtVtk::texture_actor();
	void Menu_Action(QAction*);
	void LM();
	void display_clouds();
	void display_lines();
	void display_shelter();
	void display_texture();
	void Pattern_Select();
	void QtVtk::Leap();
};

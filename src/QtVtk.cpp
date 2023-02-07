#include "stdafx.h"
#include "QtVtk.h"
#include <QMessageBox>
#include "LEAPClass.h"
#include <QFileDialog>
#include <QDir>

#define ang1 80
#define ang2 150
#define ang3 211
#define ang4 285
#define ang5 345
//角度对应五份纹理图像的拍摄方向，不同方向的纹理图不同
#define pi 3.1415926
#pragma execution_character_set("utf-8")//设置字码，不然中文消息报错
int Pattern = 4;//初始化LM模式，无响应
extern bool flag = false;
DWORD _stdcall LEAP(LPVOID lpParameter);

float points[9527][3];
float points_center[3];
int triangles[18940][3];
float texture1[9527][3];
float texture2[9527][3];
float texture3[9527][3];
float texture4[9527][3];
float texture5[9527][3];
int triangles_number = 0;
int points_number = 0;
bool cloud_flag = false;
bool line_flag = false;
bool shelter_flag = false;
bool texture_flag = false;
bool data_flag = false;

vtkSmartPointer<vtkPoints> pts;
vtkSmartPointer<vtkActor>tri_actor;
vtkSmartPointer<vtkActor>cloud_actor;
vtkSmartPointer<vtkActor>line_actor;
vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkCamera> camera;
vtkSmartPointer<vtkActor> texture_actor1;
vtkSmartPointer<vtkActor> texture_actor2;
vtkSmartPointer<vtkActor> texture_actor3;
vtkSmartPointer<vtkActor> texture_actor4;
vtkSmartPointer<vtkActor> texture_actor5;
void Reader_TXT();
void Texture_Generate(int Serial_Number, vtkSmartPointer<vtkActor> texture_actor);
double Get_PointTheta(double point_coord_UV[3]);

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);


QtVtk::QtVtk(QWidget *parent)
	: QMainWindow(parent)
{

	//parent->setWindowState(Qt::WindowMaximized);
	ui.setupUi(this);
	this->setWindowState(Qt::WindowMaximized);
	ui.label->setVisible(0);
	ui.Cursor_Button->setVisible(0);
	//ui.Cursor_Button->setChecked(1);
	ui.Zoom_Button->setVisible(0);
	ui.Rotate_Button->setVisible(0);
	ui.Pause_Button->setVisible(0);
	ui.Mix_Button->setVisible(0);
	ui.Pause_Button->setChecked(1);
	//QTextCodec *codec = QTextCodec::codecForName("GB2312");

	renderer = vtkSmartPointer<vtkRenderer>::New();
	//Load();
	renderer->SetBackground(0, 0, 0);
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

}

void Reader_TXT()
{

	FILE *fp;
	if ((fp = fopen("data.txt", "r")) == NULL)
	{
		printf("打开失败!");
		return;
	}

	fscanf(fp, "%i", &points_number);//读取点的个数

	float sum[3];
	sum[0] = 0;
	sum[1] = 0;
	sum[2] = 0;

	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fscanf(fp, "%f", &points[i][j]);
			sum[j] += points[i][j];
		}
	}
	points_center[0] = sum[0] / points_number;
	points_center[1] = sum[1] / points_number;
	points_center[2] = sum[2] / points_number;
	//读取点的坐标数据并储存,并计算点云的中心坐标

	fscanf(fp, "%i", &triangles_number);//读取三角形的个数

	for (int i = 0; i < triangles_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%i", &triangles[i][j]);
	}
	//读取三角形的点ID数据

	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%f", &texture1[i][j]);
	}
	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%f", &texture2[i][j]);
	}
	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%f", &texture3[i][j]);
	}
	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%f", &texture4[i][j]);
	}
	for (int i = 0; i < points_number; i++)
	{
		for (int j = 0; j < 3; j++)
			fscanf(fp, "%f", &texture5[i][j]);
	}
	//读取五份纹理的点对应的图像坐标信息
	fclose(fp);
	return;
}
void Texture_Generate(int Serial_Number, vtkSmartPointer<vtkActor> texture_actor)
{
	vtkSmartPointer<vtkFloatArray> Texture_array = vtkSmartPointer<vtkFloatArray>::New();
	Texture_array->SetNumberOfComponents(2);
	//映射纹理坐标
	vtkSmartPointer<vtkBMPReader>texReader = vtkSmartPointer<vtkBMPReader>::New();
	switch (Serial_Number)
	{
	case 1:
		texReader->SetFileName("11.bmp");
		for (int ID = 0; ID < pts->GetNumberOfPoints(); ID++)
			Texture_array->InsertNextTuple2(texture1[ID][1] / 1280, 1 - texture1[ID][2] / 1024);
		break;
	case 2:
		texReader->SetFileName("22.bmp");
		for (int ID = 0; ID < pts->GetNumberOfPoints(); ID++)
			Texture_array->InsertNextTuple2(texture2[ID][1] / 1280, 1 - texture2[ID][2] / 1024);
		break;
	case 3:
		texReader->SetFileName("33.bmp");
		for (int ID = 0; ID < pts->GetNumberOfPoints(); ID++)
			Texture_array->InsertNextTuple2(texture3[ID][1] / 1280, 1 - texture3[ID][2] / 1024);
		break;
	case 4:
		texReader->SetFileName("44.bmp");
		for (int ID = 0; ID < pts->GetNumberOfPoints(); ID++)
			Texture_array->InsertNextTuple2(texture4[ID][1] / 1280, 1 - texture4[ID][2] / 1024);
		break;
	case 5:
		texReader->SetFileName("55.bmp");
		for (int ID = 0; ID < pts->GetNumberOfPoints(); ID++)
			Texture_array->InsertNextTuple2(texture5[ID][1] / 1280, 1 - texture5[ID][2] / 1024);
		break;

	}

	texReader->GetOutput();
	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(texReader->GetOutputPort());
	texture->InterpolateOn();//插值开启
	texture->SetMipmap(1);//mipmap针对的是纹理贴图
	//当我们对这个贴图使用了MipMap技术之后，贴图会根据摄像机距离模型的远近而调整不同的不同质量的贴图显示

	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < 9527; i++)
	{
		vertices->InsertNextCell(1);		//_加入细胞顶点信息----用于渲染点集
		vertices->InsertCellPoint(i);
	}
	vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
	vtkSmartPointer<vtkCellArray>strips = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < triangles_number; i++)//生成对应的纹理三角片
	{
		double d0 = Get_PointTheta(pts->GetPoint(triangles[i][0]));
		double d1 = Get_PointTheta(pts->GetPoint(triangles[i][1]));
		double d2 = Get_PointTheta(pts->GetPoint(triangles[i][2]));
		switch (Serial_Number)
		{
		case 1:
			if ((d0 < pi*ang5 / 180 || d1 < pi*ang5 / 180 || d2 < pi*ang5 / 180)
				&& (d0 >= pi * ang4 / 180 && d1 >= pi * ang4 / 180 && d2 >= pi * ang4 / 180))
			{
				triangle->GetPointIds()->SetId(0, triangles[i][0]);
				triangle->GetPointIds()->SetId(1, triangles[i][1]);
				triangle->GetPointIds()->SetId(2, triangles[i][2]);
				strips->InsertNextCell(triangle);
			}
			break;
		case 2:
			if (((d0 < pi*(ang1) / 180 || d1 < pi*(ang1) / 180 || d2 < pi*(ang1) / 180)
				|| (d0 > pi*ang5 / 180 || d1 > pi*ang5 / 180 || d2 > pi*ang5 / 180)))
			{
				triangle->GetPointIds()->SetId(0, triangles[i][0]);
				triangle->GetPointIds()->SetId(1, triangles[i][1]);
				triangle->GetPointIds()->SetId(2, triangles[i][2]);
				strips->InsertNextCell(triangle);
			}
			break;
		case 3:
			if ((d0 > pi*ang1 / 180 || d1 > pi*ang1 / 180 || d2 > pi*ang1 / 180)
				&& (d0 <= pi * ang2 / 180 || d0 <= pi * ang2 / 180 || d0 <= pi * ang2 / 180))
			{
				triangle->GetPointIds()->SetId(0, triangles[i][0]);
				triangle->GetPointIds()->SetId(1, triangles[i][1]);
				triangle->GetPointIds()->SetId(2, triangles[i][2]);
				strips->InsertNextCell(triangle);
			}
			break;
		case 4:
			if ((d0 >= pi * ang3 / 180 || d1 >= pi * ang3 / 180 || d2 >= pi * ang3 / 180)
				&& (d0 <= pi * ang4 / 180 || d1 <= pi * ang4 / 180 || d2 <= pi * ang4 / 180))
			{
				triangle->GetPointIds()->SetId(0, triangles[i][0]);
				triangle->GetPointIds()->SetId(1, triangles[i][1]);
				triangle->GetPointIds()->SetId(2, triangles[i][2]);
				strips->InsertNextCell(triangle);
			}
			break;
		case 5:
			if ((d0 >= pi * ang2 / 180 || d1 >= pi * ang2 / 180 || d2 >= pi * ang2 / 180)
				&& (d0 <= pi * ang3 / 180 || d1 <= pi * ang3 / 180 || d2 <= pi * ang3 / 180))
			{
				triangle->GetPointIds()->SetId(0, triangles[i][0]);
				triangle->GetPointIds()->SetId(1, triangles[i][1]);
				triangle->GetPointIds()->SetId(2, triangles[i][2]);
				strips->InsertNextCell(triangle);
			}
			break;

		}

	}
	vtkSmartPointer<vtkPolyData>texture_data = vtkSmartPointer<vtkPolyData>::New();
	texture_data->SetPoints(pts);//添加点坐标
	texture_data->SetStrips(strips);
	//texture_data->SetVerts(vertices);
	texture_data->GetPointData()->SetTCoords(Texture_array);

	vtkSmartPointer<vtkPolyDataMapper> Texture_mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	Texture_mapper->SetInputData(texture_data);

	//Texture_actor = vtkSmartPointer<vtkActor>::New();
	texture_actor->SetMapper(Texture_mapper);
	texture_actor->SetTexture(texture);
	//renderer->AddActor(texture_actor);
}
double Get_PointTheta(double point_coord_UV[3])
{
	double point_coord[3];
	for (int i = 0; i < 3; i++)
		point_coord[i] = point_coord_UV[i] - points_center[i];
	if (point_coord[0] >= 0 && point_coord[2] >= 0)
		return atan(point_coord[0] / point_coord[2]);
	if (point_coord[0] >= 0 && point_coord[2] <= 0)
		return  pi + atan(point_coord[0] / point_coord[2]);
	if (point_coord[0] <= 0 && point_coord[2] <= 0)
		return  pi + atan(point_coord[0] / point_coord[2]);
	if (point_coord[0] <= 0 && point_coord[2] >= 0)
		return  2 * pi + atan(point_coord[0] / point_coord[2]);
	return 0;
}

void QtVtk::setCamera()
{
	camera = vtkSmartPointer<vtkCamera>::New();
	camera->SetPosition(500, 500, 1000);
	camera->SetViewAngle(0);
	camera = renderer->GetActiveCamera();
	camera->SetFocalPoint(102, 246, 100);
	camera->SetPosition(0, 0, 500);
}
void QtVtk::setpts()
{
	pts = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < points_number; i++)
		pts->InsertNextPoint((double)points[i][0], (double)points[i][1], (double)points[i][2]);
}
void QtVtk::clouds_actor()
{
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < 9527; i++)
	{
		vertices->InsertNextCell(1);		//_加入细胞顶点信息----用于渲染点集
		vertices->InsertCellPoint(i);
	}
	vtkSmartPointer<vtkPolyData>cloud_data = vtkSmartPointer<vtkPolyData>::New();
	cloud_data->SetPoints(pts);
	cloud_data->SetVerts(vertices);

	vtkSmartPointer<vtkPolyDataMapper>cloud_Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cloud_Mapper->SetInputData(cloud_data);


	cloud_actor = vtkSmartPointer<vtkActor>::New();

	cloud_actor->SetMapper(cloud_Mapper);
	cloud_actor->GetProperty()->SetAmbient(3);
	cloud_actor->GetProperty()->SetPointSize(1.5);

	cloud_actor->SetPosition(0, 0, 0);
	cloud_actor->GetProperty()->SetDiffuseColor(1, 0, 0);//表面漫反射颜色
	cloud_actor->GetProperty()->SetSpecular(0.1);//镜反射率
	cloud_actor->GetProperty()->SetSpecularPower(10);//表面镜反射光强
	cloud_actor->GetProperty()->SetColor(1, 0, 0);//表面颜色
	cloud_actor->GetProperty()->SetEdgeColor(0, 0, 1);//边界颜色
	cloud_actor->GetProperty()->SetEdgeVisibility(1);//开启边界
	while (1)
	{
		renderer->GetRenderWindow()->Render();

	}
}
void QtVtk::lines_actor()
{
	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	vtkSmartPointer<vtkCellArray>lines = vtkSmartPointer<vtkCellArray>::New();

	for (int i = 0; i < triangles_number; i++)
	{

		line->GetPointIds()->SetId(0, triangles[i][0]);
		line->GetPointIds()->SetId(1, triangles[i][1]);
		lines->InsertNextCell(line);
		line->GetPointIds()->SetId(0, triangles[i][1]);
		line->GetPointIds()->SetId(1, triangles[i][2]);
		lines->InsertNextCell(line);
		line->GetPointIds()->SetId(0, triangles[i][2]);
		line->GetPointIds()->SetId(1, triangles[i][0]);
		lines->InsertNextCell(line);
	}
	vtkSmartPointer<vtkPolyData>lines_data = vtkSmartPointer<vtkPolyData>::New();
	lines_data->SetPoints(pts);
	lines_data->SetPolys(lines);


	vtkSmartPointer<vtkPolyDataMapper>lines_Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	lines_Mapper->SetInputData(lines_data);
	/*
	vtkSmartPointer<vtkSmoothPolyDataFilter> smoothfilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	smoothfilter->SetInputConnection(lines_Mapper->GetOutputPort());
	//smoothfilter->SetNumberOfIterations(200);
	vtkSmartPointer<vtkPolyDataMapper>smooth_Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	smooth_Mapper->SetInputData(smoothfilter->GetOutput());
	smooth_Mapper->Update();
	*/
	line_actor = vtkSmartPointer<vtkActor>::New();

	line_actor->SetMapper(lines_Mapper);

	line_actor->GetProperty()->SetAmbient(5);
	line_actor->GetProperty()->SetPointSize(2);

	line_actor->SetPosition(0, 0, 0);
	line_actor->GetProperty()->SetDiffuseColor(0, 0, 0.8);//表面漫反射颜色
	line_actor->GetProperty()->SetSpecular(0.5);//镜反射率
	line_actor->GetProperty()->SetSpecularPower(10);//表面镜反射光强
	line_actor->GetProperty()->SetColor(0, 0, 0.8);//表面颜色
	line_actor->GetProperty()->SetEdgeColor(0, 0, 1);//边界颜色
	line_actor->GetProperty()->SetEdgeVisibility(1);//开启边界*/
}
void QtVtk::shelter_actor()
{
	vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
	vtkSmartPointer<vtkCellArray>strips = vtkSmartPointer<vtkCellArray>::New();

	for (int i = 0; i < triangles_number; i++)
	{
		triangle->GetPointIds()->SetId(0, triangles[i][0]);
		triangle->GetPointIds()->SetId(1, triangles[i][1]);
		triangle->GetPointIds()->SetId(2, triangles[i][2]);
		strips->InsertNextCell(triangle);
	}

	vtkSmartPointer<vtkPolyData>tri_data = vtkSmartPointer<vtkPolyData>::New();
	tri_data->SetPoints(pts);
	tri_data->SetPolys(strips);

	vtkSmartPointer<vtkPolyDataMapper>tri_Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	tri_Mapper->SetInputData(tri_data);

	tri_actor = vtkSmartPointer<vtkActor>::New();
	tri_actor->SetMapper(tri_Mapper);
	tri_actor->SetPosition(0, 0, 0);
	tri_actor->GetProperty()->SetDiffuseColor(0, 0, 1);//表面漫反射颜色
	tri_actor->GetProperty()->SetSpecular(0);//镜反射率
	tri_actor->GetProperty()->SetSpecularPower(0);//表面镜反射光强
	tri_actor->GetProperty()->SetColor(0, 0, 0);//表面颜色
	tri_actor->GetProperty()->SetEdgeColor(0, 0, 1);//边界颜色
	tri_actor->GetProperty()->SetEdgeVisibility(1);//开启边界
}
void QtVtk::texture_actor()
{
	//依次生成五份纹理对应的Actor
	texture_actor1 = vtkSmartPointer<vtkActor>::New();
	texture_actor2 = vtkSmartPointer<vtkActor>::New();
	texture_actor3 = vtkSmartPointer<vtkActor>::New();
	texture_actor4 = vtkSmartPointer<vtkActor>::New();
	texture_actor5 = vtkSmartPointer<vtkActor>::New();
	Texture_Generate(1, texture_actor1);
	Texture_Generate(2, texture_actor2);
	Texture_Generate(3, texture_actor3);
	Texture_Generate(4, texture_actor4);
	Texture_Generate(5, texture_actor5);
}
void QtVtk::display_clouds()
{
	if (data_flag == false)
	{
		ui.pushButton_cloud->setChecked(0);
		QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件未加载"));
		box->show();
		QTimer::singleShot(1500, box, SLOT(accept()));
		return;
	}
	if (cloud_flag)
	{
		renderer->RemoveActor(cloud_actor);
		cloud_flag = false;
	}
	else
	{
		renderer->AddActor(cloud_actor);
		cloud_flag = true;
	}
	
}
void QtVtk::display_lines()
{
	if (data_flag == false)
	{
		QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件未加载"));
		box->show();
		QTimer::singleShot(1500, box, SLOT(accept()));
		ui.pushButton_line->setChecked(0);
		return;
	}
	if (shelter_flag&&line_flag)
	{

		renderer->RemoveActor(tri_actor);
		tri_actor->GetProperty()->SetEdgeVisibility(0);
		renderer->AddActor(tri_actor);
		line_flag = false;
	}
	else if (!shelter_flag && line_flag)
	{
		renderer->RemoveActor(line_actor);
		line_flag = false;
	}
	else if (!shelter_flag && !line_flag)
	{
		renderer->AddActor(line_actor);
		line_flag = true;
	}
	else
	{
		renderer->RemoveActor(tri_actor);
		tri_actor->GetProperty()->SetEdgeVisibility(1);//开启边界
		renderer->AddActor(tri_actor);
		line_flag = true;
	}

}
void QtVtk::display_shelter()
{
	if (data_flag == false)
	{
		QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件未加载"));
		box->show();
		QTimer::singleShot(1500, box, SLOT(accept()));
		ui.pushButton_shelter->setChecked(0);
		return;
	}
	if (shelter_flag&&line_flag)
	{
		renderer->RemoveActor(tri_actor);
		renderer->AddActor(line_actor);
		shelter_flag = false;
	}
	else if (shelter_flag && !line_flag)
	{
		renderer->RemoveActor(tri_actor);
		shelter_flag = false;
	}
	else if (!shelter_flag && line_flag)
	{
		renderer->RemoveActor(line_actor);
		renderer->RemoveActor(tri_actor);
		tri_actor->GetProperty()->SetEdgeVisibility(1);//开启边界
		renderer->AddActor(tri_actor);
		shelter_flag = true;
	}
	else
	{
		tri_actor->GetProperty()->SetEdgeVisibility(0);//开启边界
		renderer->RemoveActor(line_actor);
		renderer->AddActor(tri_actor);
		shelter_flag = true;
	}
}
void QtVtk::display_texture()
{

	if (data_flag == false)
	{
		QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件未加载"));
		box->show();
		QTimer::singleShot(1500, box, SLOT(accept()));
		ui.pushButton_texture->setChecked(0);
		return;
	}
	if (texture_flag)
	{
		renderer->RemoveActor(texture_actor1);
		renderer->RemoveActor(texture_actor2);
		renderer->RemoveActor(texture_actor3);
		renderer->RemoveActor(texture_actor4);
		renderer->RemoveActor(texture_actor5);
		texture_flag = false;
		ui.pushButton_line->setEnabled(1);
		ui.pushButton_cloud->setEnabled(1);
		ui.pushButton_shelter->setEnabled(1);
	}
	else
	{

		renderer->AddActor(texture_actor1);
		renderer->AddActor(texture_actor3);
		renderer->AddActor(texture_actor4);
		renderer->AddActor(texture_actor5);
		renderer->AddActor(texture_actor2);
		texture_flag = true;

		/***********开启纹理时，关闭其他显示方式*********/
		if (line_flag == true)
		{
			ui.pushButton_line->setChecked(0);
			ui.pushButton_line->clicked();
		}
		if (shelter_flag == true)
		{
			ui.pushButton_shelter->setChecked(0);
			ui.pushButton_shelter->clicked();
		}
		if (cloud_flag = true)
		{
			ui.pushButton_cloud->setChecked(0);
			ui.pushButton_cloud->clicked();
		}
		ui.pushButton_line->setEnabled(0);
		ui.pushButton_cloud->setEnabled(0);
		ui.pushButton_shelter->setEnabled(0);
	}

}
void QtVtk::LM()
{
	//ui.label->setVisible(1);
	ui.Cursor_Button->setVisible(1);
	ui.Zoom_Button->setVisible(1);
	ui.Rotate_Button->setVisible(1);
	ui.Pause_Button->setVisible(1);
	ui.Mix_Button->setVisible(1);
	
	//ui.pushButton_LeapMotion->setVisible(0);
	//renderer->GetRenderWindow()->Render();
	CreateThread(NULL, 0, LEAP, NULL, 0, NULL);
	while (1)
	{
		renderer->GetRenderWindow()->Render();
		Sleep(25);
		if (flag)
			break;
	}
}
void QtVtk::Pattern_Select()
{
	QObject* sender = QObject::sender();//获取信号的发生者，选择相应的模式
	if (sender == ui.Cursor_Button)
		Pattern = 0;
	else if (sender == ui.Zoom_Button)
		Pattern = 1;
	else if (sender == ui.Rotate_Button)
		Pattern = 2;
	else if (sender == ui.Mix_Button)
		Pattern = 3;
	else if (sender == ui.Pause_Button)
		Pattern = 4;
}
void QtVtk::Menu_Action(QAction* act)
{
	if (act->text() == "图像保存")
	{
		time_t setTime;
		time(&setTime);
		tm* ptm = localtime(&setTime);
		std::string time = (std::to_string(ptm->tm_mon + 1)
			+ '_' + std::to_string(ptm->tm_mday)
			+ '_' + std::to_string(ptm->tm_hour)
			+ '_' + std::to_string(ptm->tm_min) 
			+ '_' + std::to_string(ptm->tm_sec)
			+ ".bmp");
		const char *name = time.c_str();
		vtkSmartPointer<vtkWindowToImageFilter>  wif = vtkSmartPointer<vtkWindowToImageFilter>::New();
		vtkSmartPointer<vtkBMPWriter> bmpw = vtkSmartPointer<vtkBMPWriter>::New();
		wif->SetInput(renderer->GetVTKWindow());
		bmpw->SetInputConnection(wif->GetOutputPort());
		bmpw->SetFileName(name);
		bmpw->SetFilePattern("bmp");
		bmpw->Write();
		QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("图像保存成功"));
		box->show();
		QTimer::singleShot(1500, box, SLOT(accept()));
	}
	else if (act->text() == "文件加载")
	{
		if (data_flag == false)
		{
			try
			{
				Reader_TXT();
				setCamera();
				setpts();
				clouds_actor();
				lines_actor();
				shelter_actor();
				texture_actor();
				QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件加载成功"));
				box->show();
				QTimer::singleShot(1500, box, SLOT(accept()));
				data_flag = true;
			}
			catch (...)//捕获所有异常
			{
				//QMessageBox::information(this, "信息", "文件加载成功");
				QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件加载失败"));
				box->show();
				//QTimer::singleShot(1500, box, SLOT(accept()));
			}
		}
		else
		{
			QMessageBox *box = new QMessageBox(QMessageBox::Information, QString::fromUtf8("消息"), QString::fromUtf8("文件已加载完成"));
			box->show();
			QTimer::singleShot(1500, box, SLOT(accept()));
		}
	}
	//QTimer::singleShot(1500, box, SLOT(accept()));

}


DWORD _stdcall LEAP(LPVOID lpParameter)//线程执行函数
{
	
	MouseListener listener;
	Controller controller;
	controller.addListener(listener);
	while (1)
	{
		camera->Elevation(1);
		camera->OrthogonalizeViewUp();
	}
	controller.removeListener(listener);
	return 0;
}

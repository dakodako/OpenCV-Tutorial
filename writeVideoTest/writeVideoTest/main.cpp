#include "globalInclude.h"
using namespace cv;
using namespace std;
typedef struct correctParameters
{
	Mat imgOrg;
	Point2i center;
	int radius;
	double w_longtitude;
	double w_latitude;
	distMapMode distMap;
	double theta_left;
	double phi_up;
	double camerFieldAngle;
	camMode camProjMode;
	CorrectType typeOfCorrect;
}correctParams;

double func(double l, double phi)
{
	double result = (l / 2 - 1)*cos(phi) - sin(phi) + 1;
	return result;
}
double getPhi(double l)
{
	double head = 0;
	double tail = 0;
	double mid = 0;
	double result = 0;
	if (l >= 0 && l < 2)
	{
		head = 0;
		tail = PI / 2;
		mid = head;
		result = func(l, mid);
		while (abs(result)>LIMIT)
		{
			mid = (tail + head) / 2;
			result = func(l, mid);

			if (result > 0)
			{
				head = mid;
			}
			else
			{
				tail = mid;
			}
		}
	}
	else
	{
		head = PI / 2;
		tail = PI;
		mid = tail;
		result = func(l, mid);
		while (abs(result) > LIMIT)
		{
			mid = (tail + head) / 2;
			result = func(l, mid);
			if (result < 0)
			{
				head = mid;
			}
			else
			{
				tail = mid;
			}
		}
	}
	return mid;
}
Mat correct(Mat img, Point2i center, int radius, double phi_up, double cameraFieldAngle)
{
	double rateOfWindow = 0.9;

	int width = max(img.cols, img.rows);
	int height = width;
	//	int height = img.rows;
	Size imgSize(width, height);
	int center_x = imgSize.width / 2;
	int center_y = imgSize.height / 2;
	//int center_y = imgSize.width / 2;
	Mat retImg(imgSize, CV_8UC3, Scalar(0, 0, 0));

	double dx = cameraFieldAngle / imgSize.width;
	double dy = cameraFieldAngle / imgSize.height;

	double latitude;
	double longitude;

	double x, y, z, r;

	double theta_sphere;
	double phi_sphere;

	double p, theta;

	double x_cart, y_cart;

	double u, v;
	Point pt;

	int u_latitude, v_latitude;
	Rect imgArea(0, 0, img.cols, img.rows);


	double foval = 0.0;// focus
	Mat_<Vec3b> _retImg = retImg;
	Mat_<Vec3b> _img = img;
	for (int j = 0; j < imgSize.height; j++)
	{
		for (int i = 0; i < imgSize.width; i++)
		{

			latitude = getPhi((double)j * 4.0 / imgSize.height);
			longitude = getPhi((double)i * 4 / imgSize.width);
			//	latitude = (PI - cameraFieldAngle) / 2 + j * dy;
			//  longitude = (PI - cameraFieldAngle) / 2 + i * dx;
			//Convert from latitude cooradinate to the sphere cooradinate
			x = -sin(latitude)*cos(longitude);
			y = cos(latitude);
			z = sin(latitude)*sin(longitude);
			theta_sphere = acos(z);
			phi_sphere = cvFastArctan(y, x);//return value in Angle
			phi_sphere = phi_sphere*PI / 180;//Convert from Angle to Radian

			foval = radius / (cameraFieldAngle / 2);
			p = foval*theta_sphere;
			theta = phi_sphere;

			//Convert from fish-eye polar cooradinate to cartesian cooradinate
			x_cart = p*cos(theta);
			y_cart = p*sin(theta);

			//double R = radius / sin(camerFieldAngle / 2);

			//Convert from cartesian cooradinate to image cooradinate
			u = x_cart + center.x;
			v = -y_cart + center.y;

			pt = Point(u, v);

			if (!pt.inside(imgArea))
			{
				continue;
			}

			_retImg.at<Vec3b>(j, i) = _img.at<Vec3b>(pt);
		}
	}
	return retImg;
}
int main(int argc, char **argv)
{
	const string source = "D:\\Documents\\Movies\\out4.avi";
	

	VideoCapture inputVideo(source);
	
	if (!(inputVideo.isOpened()))// && inputVideo2.isOpened() && inputVideo3.isOpened() && inputVideo4.isOpened()))
	{
		cout << "Could not open the input video" << endl;
		return -1;
	}


	string::size_type pAt = source.find_last_not_of('.');
	const string NAME = source.substr(0, pAt)+"corrected" + ".avi";

	int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));
	//cout << ex << endl;
	char EXT[] = { (char)(ex & 0XFF),(char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF00000) >> 24) ,0 };
	Size S = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
	Size S_correction = Size(640, 640);
	VideoWriter outputVideo;
	//outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);
	outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S_correction, true);
	
	if (!(outputVideo.isOpened()))// && outputVideo2.isOpened() && outputVideo3.isOpened() && outputVideo4.isOpened()))
	{
		cout << "cannot open the output video for write" << source << endl;
		return -1;
	}
	//cout << "kkkkk" <<endl;
	cout << "Input frame resolution: Width = " << S.width << "Height = " << S.height << endl;
	//cout << "Outout frame resolution: Width = " << S_final.width << "Height = " << S_final.height << endl;

	Mat src, src2, src3, src4;
	vector<Mat> spl;
	Mat res;
//	Mat res(S_final, CV_8UC3, Scalar(0, 0, 0));
	//int n = 0;
	correctParameters params;
	params.center.x = S.width / 2;
	params.center.y = S.height / 2;
	params.radius = S.width / 2 + 20;
	params.w_longtitude = PI / 2;
	params.w_latitude = PI / 2;
	params.distMap = LATITUDE_LONGTITUDE;
	params.theta_left = 0;
	params.phi_up = 0;
	params.camerFieldAngle = PI;
	params.camProjMode = EQUIDISTANCE;
	params.typeOfCorrect = Reverse;
	for (;;)
	{
		inputVideo >> src;
		
		if (src.empty())// || src2.empty() || src3.empty() || src4.empty())
		{
			break;
		}
		params.imgOrg = src;
		src = correct(params.imgOrg, params.center, params.radius, params.phi_up, params.camerFieldAngle);
		split(src, spl);
		merge(spl, res);
		
	
		outputVideo << res;
	}
	cout << "kkk" << endl;
	return 0;
}
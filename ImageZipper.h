/**
@brief  Depth Zipper Writer/Reader Class
		Image will be compressed into a zip file without loss.
@author zhu-ty
@date Apr 25, 2019
*/


#ifndef _DEPTH_ZIPPER_DEPTH_ZIPPER_H_
#define _DEPTH_ZIPPER_DEPTH_ZIPPER_H_

#include <opencv2/opencv.hpp>
#define MAX_FRAME_BUFFER_SIZE 5 // 10 ^ size , 5 means 10000

//#define IMAGE_ZIPPER_IMAGE_FORMAT std::string(".tiff")
#define IMAGE_ZIPPER_IMAGE_FORMAT std::string(".png")
class ImageZipperWriter
{
public:
	ImageZipperWriter() {};
	~ImageZipperWriter() {};
	enum class WriteType
	{
		Create = 0, 
		Append = 1
	};

	int init(std::string fileName, WriteType type = WriteType::Create);
	int append(std::vector<cv::Mat> &imgs, int threadCount = 1);
	int append(cv::Mat &img);
	int release();

	int setVerbose(bool verbose);
	//No = 0, Default = 6, Best = 9
	int setZipCompressLevel(int level);
	int setImgCompressionParam(std::vector<int> param = std::vector<int>());
private:
	int _frame; //Next frame to write
	//zip_t *_zip = nullptr;
	void *_zip = nullptr;
	bool _verbose = true;
	int _comLevel = 6;

	int64_t partTimeCost = 0;

	std::vector<int> _param;
	int switchEndian16(cv::Mat &m);
	void compressThread(std::vector<cv::Mat> &imgs, std::vector<std::vector<uchar>> &imgdatas, int startID, int stopID);
};

class ImageZipperReader
{
public:
	ImageZipperReader() {};
	~ImageZipperReader() {};
	int init(std::string fileName);
	int init(std::string fileName, int &maxFrameNum);
	int getMaxFrameNum();
	cv::Mat read(int frameNum);
	cv::Mat read(int frameNum, std::string &fileName);
	int release();

private:
	void *_zip = nullptr;
	std::vector<std::string> fileList;

};


#endif //_DEPTH_ZIPPER_DEPTH_ZIPPER_H_
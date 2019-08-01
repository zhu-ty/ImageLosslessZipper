#include "ImageZipper.h"
#include "zip.h"
#include "SKCommon.hpp"

int ImageZipperWriter::init(std::string fileName, WriteType type, int startIdx)
{
	release();
	_frame = startIdx;
	if (type == WriteType::Append)
	{
		zip_t *tmpZip = zip_open(fileName.c_str(), 0, 'r');
		int i, n = zip_total_entries(tmpZip);
		for (i = 0; i < n; ++i) 
		{
			zip_entry_openbyindex(tmpZip, i);
			const char *name_ = zip_entry_name(tmpZip);
			int isdir = zip_entry_isdir(tmpZip);
			if (!isdir)
			{
				std::string name(name_);
				int num = atoi(name.substr(0, name.find("..")).c_str());
				if (_frame < (num + 1))
					_frame = num + 1;
			}
			zip_entry_close(tmpZip);
		}
		zip_close(tmpZip);
	}
	//else
	//	_frame = 0;
	_zip = zip_open(fileName.c_str(), _comLevel, (type == WriteType::Create) ? 'w' : 'a');
	return 0;
}

int ImageZipperWriter::append(std::vector<cv::Mat> &imgs, int threadCount)
{
	if (_zip == nullptr)
	{
		SKCommon::errorOutput("Init DepthZipper First!");
		return -1;
	}
	std::vector<std::vector<uchar>> imgdatas;
	imgdatas.resize(imgs.size());
	if (threadCount > imgs.size())
		threadCount = imgs.size();
	if(threadCount > 1)
		SKCommon::infoOutput("Using %d threads to compress the image", threadCount);
	std::vector<std::thread> ths(threadCount);
	int64_t _stat_timer = SKCommon::getCurrentTimeMicroSecond();
	SKCommon::infoOutput("Start Compression ...");
	int start = 0;
	for (int i = 0; i < threadCount; i++)
	{
		int st, ed;
		st = start;
		ed = MIN(imgs.size(), start + imgs.size() / threadCount);
		if (i == threadCount - 1)
			ed = imgs.size();
		start = ed;
		ths[i] = std::thread(&ImageZipperWriter::compressThread, this, std::ref(imgs), std::ref(imgdatas), st, ed);
	}
	for (int i = 0; i < threadCount; i++)
	{
		ths[i].join();
	}
	SKCommon::infoOutput("Compression done! Cost : %d ms", (SKCommon::getCurrentTimeMicroSecond() - _stat_timer) / 1000);
	SKCommon::infoOutput("Start Zipping ...");
	float _stat_ = 0.1f;
	_stat_timer = SKCommon::getCurrentTimeMicroSecond();
	for (int i = 0; i < imgs.size(); i++)
	{
		if ((float)i / imgs.size() > _stat_)
		{
			SKCommon::infoOutput("Zipped images %d%% percent", (int)(_stat_ * 100));
			_stat_ += 0.1f;
		}
		std::string formats, file;
		int type = imgs[i].type();
		uchar depth = type & CV_MAT_DEPTH_MASK;
		uchar chans = 1 + (type >> CV_CN_SHIFT);
		if (depth == CV_32F && chans == 1)
			formats = SKCommon::format("%%0%dd..32fc1" + IMAGE_ZIPPER_IMAGE_FORMAT, MAX_FRAME_BUFFER_SIZE);
		else
			formats = SKCommon::format("%%0%dd." + IMAGE_ZIPPER_IMAGE_FORMAT, MAX_FRAME_BUFFER_SIZE);
		file = SKCommon::format(formats.c_str(), _frame);
		if (_frame > pow(10, MAX_FRAME_BUFFER_SIZE))
			SKCommon::warningOutput(SKCommon::format("max frame size reached, maxnimum = %d, frame count = %d", pow(10, MAX_FRAME_BUFFER_SIZE), _frame));
		zip_entry_open((zip_t*)_zip, file.c_str());
		zip_entry_write((zip_t*)_zip, imgdatas[i].data(), imgdatas[i].size());
		zip_entry_close((zip_t*)_zip);
		if (_verbose)
			SKCommon::infoOutput(file + " ziped.");
		_frame++;
	}
	SKCommon::infoOutput("Zipping done! Zipped: %d images, Cost : %d ms", imgs.size(), (SKCommon::getCurrentTimeMicroSecond() - _stat_timer) / 1000);
	return 0;
}

int ImageZipperWriter::append(cv::Mat &img)
{
	return append(std::vector<cv::Mat>{img});
}

int ImageZipperWriter::release()
{
	if (_zip != nullptr)
	{
		if (_verbose)
			SKCommon::infoOutput("Encoder cost %d ms (in multiple threads)", partTimeCost / 1000);
		zip_close((zip_t*)_zip);
	}
	_zip = nullptr;
	return 0;
}

int ImageZipperWriter::setVerbose(bool verbose)
{
	_verbose = verbose;
	return 0;
}

int ImageZipperWriter::setZipCompressLevel(int level)
{
	if (level == 0)
		SKCommon::warningOutput("ImageZipperWriter set to no compression.");
	_comLevel = level;
	return 0;
}

int ImageZipperWriter::setImgCompressionParam(std::vector<int> param)
{
	_param = param;
	return 0;
}

#define __bswap_constant_16(x)                                        \
  ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

int ImageZipperWriter::switchEndian16(cv::Mat & m)
{
	for(int i = 0;i < m.rows;i ++)
		for (int j = 0; j < m.cols; j++)
		{
			ushort *p = (ushort *)(m.data) + i * m.cols + j;
			*p = __bswap_constant_16(*p);
		}
	return 0;
}

void ImageZipperWriter::compressThread(std::vector<cv::Mat> &imgs, std::vector<std::vector<uchar>> &imgdatas, int startID, int stopID)
{	
	for (int i = startID; i < stopID; i++)
	{
		int type = imgs[i].type();
		uchar depth = type & CV_MAT_DEPTH_MASK;
		uchar chans = 1 + (type >> CV_CN_SHIFT);
		cv::Mat TheImage;
		if (depth == CV_32F && chans == 1)
			TheImage = cv::Mat(imgs[i].rows, imgs[i].cols, CV_8UC4, (cv::Vec4b*)imgs[i].data);
		else
			TheImage = imgs[i];
		int64_t _stat_start = SKCommon::getCurrentTimeMicroSecond();
		cv::imencode(IMAGE_ZIPPER_IMAGE_FORMAT, TheImage, imgdatas[i], _param);
		partTimeCost += SKCommon::getCurrentTimeMicroSecond() - _stat_start;
	}

}





int ImageZipperReader::init(std::string fileName)
{
	int t;
	init(fileName, t);
	return 0;
}

int ImageZipperReader::init(std::string fileName, int &maxFrameNum)
{
	release();
	int _maxFrame = 0;
	_zip = zip_open(fileName.c_str(), 0, 'r');
	int i, n = zip_total_entries((zip_t*)_zip);
	for (i = 0; i < n; ++i) 
	{
		zip_entry_openbyindex((zip_t*)_zip, i);
		const char *name_ = zip_entry_name((zip_t*)_zip);
		std::string name(name_);
		int isdir = zip_entry_isdir((zip_t*)_zip);
		if (!isdir && name.substr(name.find_last_of(".")) == IMAGE_ZIPPER_IMAGE_FORMAT)
		{
			int num = atoi(name.substr(0, name.find("..")).c_str());
			if (_maxFrame < (num + 1))
				_maxFrame = num + 1;
		}
		zip_entry_close((zip_t*)_zip);
	}
	maxFrameNum = _maxFrame;
	fileList.resize(_maxFrame);
	for (i = 0; i < n; ++i)
	{
		zip_entry_openbyindex((zip_t*)_zip, i);
		const char *name_ = zip_entry_name((zip_t*)_zip);
		std::string name(name_);
		int isdir = zip_entry_isdir((zip_t*)_zip);
		if (!isdir && name.substr(name.find_last_of(".")) == IMAGE_ZIPPER_IMAGE_FORMAT)
		{
			int num = atoi(name.substr(0, name.find("..")).c_str());
			fileList[num] = name;
		}
		zip_entry_close((zip_t*)_zip);
	}
	return 0;
}

int ImageZipperReader::getMaxFrameNum()
{
	return fileList.size();
}

cv::Mat ImageZipperReader::read(int frameNum)
{
	std::string t;
	return read(frameNum, t);
}

cv::Mat ImageZipperReader::read(int frameNum, std::string & fileName)
{
	if (_zip == nullptr && frameNum >= fileList.size())
	{
		SKCommon::errorOutput("Invalid zip file or frameNum");
		return cv::Mat();
	}
	fileName = fileList[frameNum];
	if (fileName == "")
	{
		SKCommon::warningOutput("framenum = %d does not exist!", frameNum);
		return cv::Mat();
	}
	void *buf;
	size_t bufsize;
	std::string file = fileList[frameNum];
	zip_entry_open((zip_t*)_zip, file.c_str());
	zip_entry_read((zip_t*)_zip, &buf, &bufsize);
	zip_entry_close((zip_t*)_zip);
	SKCommon::infoOutput(file + " read.");
	std::vector<uchar> bufs(bufsize);
	for (int i = 0; i < bufsize; i++)
		bufs[i] = ((uchar*)buf)[i];

	int st = fileList[frameNum].find("..") + 2;
	int ed = fileList[frameNum].find_last_of(".");
	if (ed - st <= 0)
		return cv::imdecode(bufs, cv::IMREAD_UNCHANGED);
	else if (SKCommon::toLower(fileList[frameNum].substr(st, ed - st)) == "32fc1")
	{
		cv::Mat TheImage = cv::imdecode(bufs, cv::IMREAD_UNCHANGED);
		cv::Mat floatImage(TheImage.rows, TheImage.cols, CV_32FC1, (float*)TheImage.data);
		return floatImage.clone();
	}
	else
		return cv::imdecode(bufs, cv::IMREAD_UNCHANGED);
}

int ImageZipperReader::release()
{
	if (_zip != nullptr)
		zip_close((zip_t*)_zip);
	_zip = nullptr;
	fileList.clear();
	return 0;
}

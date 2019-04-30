#include "SKCommon.hpp"
#include "ImageZipper.h"

std::vector<std::string> CollectFiles(std::string dir, std::vector<std::string> allowedExtensions)
{
	std::vector<std::string> ret;
	//std::vector<std::string> allowedExtensions = { ".jpg", ".png" ,".jpeg" };
	for (int i = 0; i < allowedExtensions.size(); i++) {
		std::vector<cv::String> imageNamesCurrentExtension;
		cv::glob(
			dir + "/*" + allowedExtensions[i],
			imageNamesCurrentExtension,
			true
		);
		ret.insert(
			ret.end(),
			imageNamesCurrentExtension.begin(),
			imageNamesCurrentExtension.end()
		);
	}
	return ret;
}

int main(int argc, char *argv[])
{
	//cv::Mat t0,t1,t3;
	//t0 = imread("CUCAU1731017_CUCAU1724010.tiff", cv::IMREAD_UNCHANGED);
	//t1 = imread("00002..32fc1.png", cv::IMREAD_UNCHANGED);
	//cv::Mat floatImage(t1.rows, t1.cols, CV_32FC1, (float*)t1.data);
	//t3 = t0 - floatImage;


	if (argc < 4)
	{
		SKCommon::infoOutput("Usage : ./ImageLosslessZipper [C,A,R] [folder] [zipFile]");
		return 0;
	}
	if (SKCommon::toLower(argv[1]) == "c" || SKCommon::toLower(argv[1]) == "a")
	{
		auto files = CollectFiles(argv[2], std::vector<std::string>{".jpg", ".png", ".tiff"});
		ImageZipperWriter dzw;
		dzw.init(argv[3], (SKCommon::toLower(argv[1]) == "c") ? ImageZipperWriter::WriteType::Create : ImageZipperWriter::WriteType::Append);
		for (int i = 0; i < files.size(); i++)
		{
			auto mat = cv::imread(files[i], cv::IMREAD_UNCHANGED);
			dzw.append(mat);
		}
		dzw.release();
	}
	else if (SKCommon::toLower(argv[1]) == "r")
	{
		SKCommon::mkdir(argv[2]);
		ImageZipperReader dzr;
		int count;
		dzr.init(argv[3], count);
		for (int i = 0; i < count; i++)
		{
			std::string name;
			cv::Mat tmp = dzr.read(i, name);
			uchar depth = tmp.type() & CV_MAT_DEPTH_MASK;
			if (depth == CV_32F)
			{
				cv::Mat pngImage = cv::Mat(tmp.rows, tmp.cols, CV_8UC4, (cv::Vec4b*)tmp.data);
				cv::imwrite(cv::format("%s/%s", argv[2], name.c_str()), pngImage);
			}
			else
				cv::imwrite(cv::format("%s/%s", argv[2], name.c_str()), tmp);
		}
		dzr.release();
	}
	else
	{
		SKCommon::infoOutput("Usage : ./DepthZipper [C,A,R] [folder] [zipFile]");
		return 0;
	}
	return 0;
}
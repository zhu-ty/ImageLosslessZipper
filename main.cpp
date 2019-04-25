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
	if (argc < 4)
	{
		SKCommon::infoOutput("Usage : ./DepthZipper [C,A,R] [folder] [zipFile]");
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
			cv::imwrite(cv::format("%s/%05d.png", argv[2], i), dzr.read(i));
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
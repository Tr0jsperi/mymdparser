#include<fstream> //用于输入输出流
#include<iostream>
#include<string>//处理字符串
#include<sstream>//
#include<vector>
#include<stack>//堆栈
#include<queue>
#include<regex>
#include"md_parser.h"

MdParser::MdParser(std::string position_of_file)//构造函数
{
	file_position = position_of_file;
}

MdParser::~MdParser()//析构函数
{
}

std::string MdParser::GetContents()
{
	std::string html_outfile = "";//定义输出html文件
	std::ifstream md_infile;
	md_infile.open(file_position);
	std::queue<char> charqueue;//辨认转义字符用队列

	//判断文件路径有效性
	if (!md_infile.is_open())
	{
		std::cout << "无法找到文件，请检查文件路径后重试"<<std::endl;
		html_outfile = u8"无法找到文件，请检查文件路径后重试";
	}
	else
	{
		md_infile.seekg(0, std::ios::end);//取文件指针到文件流末尾
		int length = md_infile.tellg();//返回取文件指针在文件流中的位置
		md_infile.seekg(0, std::ios::beg);//指针复位

		std::cout <<"文件导入成功!"<<'\n'
			<<"文件大小:"<<length<<"字节"<<'\n'
			<<"解析中..."<<std::endl;
	}
	return html_outfile;
}



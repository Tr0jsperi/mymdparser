#include<fstream>
#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<stack>
#include<queue>
#include<regex>
#include "md_parser.h"
#include<time.h>//使用clock函数计时

int main() {
	//计时开始
	clock_t time_start, time_end;
	time_start = clock();
	//打开md文件
	MdParser test_parser("C:\\Users\\troy\\Desktop\\MarkdownTest.md");

	std::string contents = test_parser.GetContents();

	//html文件头尾信息
	std::string head = "<!DOCTYPE html><html><head>\
        <meta charset=\"utf-8\">\
        <title>Markdown</title>\
        <link rel=\"stylesheet\" href=\"my-markdown.css\">\
        </head><body><article class=\"markdown-body\">";
	std::string end = "</article></body></html>";

	//写入html文件
	std::ofstream out;
	out.open("E:\\index.html");
	out << head + contents + end;
	out.close();
	time_end = clock();
	double exe_time = (double)(time_end - time_start) / CLOCKS_PER_SEC;
	std::cout << "解析完成，共耗时" << exe_time << "s";
	return 0;
}
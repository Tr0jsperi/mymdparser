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
	/*<link rel=\"stylesheet\" href=\"my-markdown.css\">\n\*/
	std::string head = "<!DOCTYPE html>\n\
</link><html>\n\
<head>\n\
<meta charset=\"utf-8\">\n\
<title>Markdown</title>\n\
<link href=\"https://raw.githubusercontent.com/troywmz/modest/master/css/modest.css\" rel=\"stylesheet\"></link>\n\
<link href=\"http://cdn.bootcss.com/highlight.js/9.0.0/styles/monokai_sublime.min.css\" rel=\"stylesheet\">\n\
<script type =\"text/javascript\" src=\"http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=default\"></script>\n\
</head>\n\
<body>\n\
<article class=\"markdown-body\">";
	std::string end = "</article>\n\
<script src=\"http://cdn.bootcss.com/highlight.js/9.0.0/highlight.min.js\"></script>\n\
<script >hljs.initHighlightingOnLoad(); </script>\n\
</body>\n\
</html>";

	
	//写入html文件
	std::ofstream out;
	out.open("E:\\index.html");
	out << head +'\n'+contents + '\n' + end;
	out.close();

	//计时结束，输出时间
	time_end = clock();
	double exe_time = (double)(time_end - time_start) / CLOCKS_PER_SEC;
	std::cout << "解析完成，共耗时" << exe_time << "s"<<'\n';	
	return 0;
}
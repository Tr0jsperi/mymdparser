#include<fstream> //用于输入输出流
#include<iostream>
#include<string>//处理字符串
#include<sstream>//
#include<vector>
#include<stack>//堆栈
#include<queue>
#include<regex>//正则
#include"md_parser.h"

MdParser::MdParser(std::string position_of_file)//构造函数
{
	file_position = position_of_file;//使用position_of_file处的文件初始化对象
}

MdParser::~MdParser()//析构函数
{
}

std::string MdParser::GetContents()
{
	std::string html_outfile = "";//定义输出html文件
	std::ifstream md_infile;//定义读入文件变量
	md_infile.open(file_position);
	std::queue<char> charqueue;//辨认转义字符用队列

	//判断文件路径有效性
	if (!md_infile.is_open())
	{
		//报错提示
		std::cout << "无法找到文件，请检查文件路径后重试" << std::endl;
		html_outfile = u8"无法找到文件，请检查文件路径后重试";
	}
	else
	{
		//计算文件大小
		md_infile.seekg(0, std::ios::end);//取文件指针到文件流末尾
		int length = md_infile.tellg();//返回取文件指针在文件流中的位置
		md_infile.seekg(0, std::ios::beg);//指针复位

		//导入成功提示
		std::cout << "文件导入成功!" << '\n'
			<< "文件大小:" << length << "字节" << '\n'
			<< "解析中..." << std::endl;

		//开始读取
		std::string get_oneline;
		std::vector<std::string> totalstring;
		while (getline(md_infile, get_oneline))//连续取md_file中的一行
		{
			totalstring.push_back(get_oneline);
		}
		auto vit = totalstring.begin();//定位首行
		process_code(vit, totalstring);
		//process_escape_first(vit, totalstring, charqueue);
		//process_escape_last(vit, totalstring, charqueue);
		for (auto ptr = totalstring.begin(); ptr != totalstring.end(); ptr++)
		{
			html_outfile += (*ptr) + '\n';
		}
		md_infile.close();
	}
	return html_outfile;
}
//code
//处理代码块和行内代码
void MdParser::process_code(std::vector<std::string>::iterator & viter, std::vector<std::string>& totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator in_viter = (*viter).begin();
		//处理代码块
		if ((*viter).size() >= 3 && (*in_viter) == '`' && (*(in_viter + 1)) == '`' && (*(in_viter + 2)) == '`')
		{
			std::vector<std::string>::iterator sec_viter = viter;//用于寻找匹配另一行
			std::string codemark = *viter;//暂存代码块标识符，出现落单标识符时能恢复
			std::string lan="";//记录语言类型
			in_viter += 3;
			for (; in_viter != (*viter).end(); in_viter++)
			{
				lan = lan + *in_viter;
			}
			*viter = "<pre><code class=\""+lan+"\">";
			lan = "";
			for (; sec_viter != totalstr.end(); sec_viter++)
			{
				in_viter = (*sec_viter).begin();
				if ((*sec_viter).size() >= 3 && (*in_viter) == '`' && (*(in_viter + 1)) == '`' && (*(in_viter + 2)) == '`')
				{
					viter = sec_viter;
					*sec_viter = "</code></pre>";					
					break;
				}
				else continue;
			}
			if (sec_viter == totalstr.end())
			{
				int linenum = viter - initialviter + 1;
				std::cout << "出现奇数代码块标记"<<'\n'<<"未匹配代码块标记位于第"<<linenum<<"行"<<'\n';				
				*viter = codemark;
			}
		}
		//处理行内代码
		else
		{
			std::stack<char> stack_seq,stack_rev;//存储单行字符
			std::string newstr;
			if ((*viter).empty()) continue;
			int bs_counter=0, total_bs_counter=0;//定义backstick计数器
			for (; in_viter != (*viter).end(); in_viter++)
			{
				if ((*in_viter) == '`'&&(stack_rev.empty()||stack_rev.top()!='\\'||bs_counter%2==1)) bs_counter++;
				stack_rev.push(*in_viter);
			}			
			if (bs_counter % 2 == 1)
			{
				bs_counter--;//减去多余的'`'号避免判断<code>首尾时出错
				int linenum = viter - initialviter+1;
				std::cout << "第" << linenum << "行存在奇数个反引号，请检查后修正。问题行已在转换文件行首注明"<<'\n';
				newstr=u8"<error>本行有奇数个反引号错误，请检查后修正</error>";
			}
			
			total_bs_counter = bs_counter;
			while (!stack_rev.empty())
			{
				stack_seq.push(stack_rev.top());
				stack_rev.pop();
			}
			while (!stack_seq.empty())
			{
				if (stack_seq.top() == '`')
				{
					if (bs_counter == 0 ||(!newstr.empty())&&*(newstr.end() - 1) == '\\' && (total_bs_counter - bs_counter) % 2 == 0)//如果为最后落单或转义'`'，则保留
					{
						newstr += '`';
					}
					else if (newstr.empty()||*(newstr.end()-1)!='\\'&&(total_bs_counter - bs_counter) % 2 == 0)//前<code>
					{
						newstr += "<code>";
						bs_counter--;
					}
					else if ((total_bs_counter - bs_counter) % 2 == 1)//后</code>
					{
						newstr += "</code>";
						bs_counter--;
					}
					stack_seq.pop();
				}
				else
				{
					newstr += stack_seq.top();
					stack_seq.pop();
				}
			}
			*viter = newstr;
			newstr = "";
		}
	}
	viter = initialviter;
}

//escape
//处理转义字符，基于windows
//提取转义字符入charqueue保存，只保留符号'/'作为位置标记
void MdParser::process_escape_first(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{		
		if ((*viter).empty()) continue;
		if ((*viter).size() >= 10 && !((*viter).compare(0, 10, "<pre><code")))//跳过代码段
		{
			while (viter != totalstr.end()&&((*viter).size() < 13||(*viter).compare(0, 13, "</code></pre>")))
			{
				viter++;
			}
		}
		for (std::string::iterator in_viter = (*viter).begin(); in_viter != (*viter).end();)
		{				
			auto code_pos = (*viter).find("<code>");//标记<code>标识符位置
			if (code_pos!=std::string::npos && in_viter - (*viter).begin() == code_pos)
			{
				auto code_end_pos = (*viter).find("</code>");
				in_viter = in_viter + code_end_pos - code_pos + 6;
			}
			if((*in_viter) == '\\'&&in_viter != ((*viter).end() - 1))//防止行末'\'符号导致之后+1迭代器错误
			{
				charqueue.push(*(in_viter + 1));
				in_viter++;
				int position = in_viter - (*viter).begin();
				(*viter).erase(in_viter);
				in_viter = (*viter).begin() + position;
			}
			else
			{
				in_viter++;
			}
		}
	}
	viter = initialviter;
}

//恢复被删除的转义字符，替换原位置标记'/'为其后的字符
void MdParser::process_escape_last(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		if ((*viter).empty()) continue;
		if ((*viter).size() >= 10 && !((*viter).compare(0, 10, "<pre><code")))//跳过代码段
		{
			while (viter != totalstr.end() && ((*viter).size() < 13 || (*viter).compare(0, 13, "</code></pre>")))
			{
				viter++;
			}
		}
		for (std::string::iterator in_viter = (*viter).begin(); in_viter != (*viter).end();in_viter++)
		{
			if (*in_viter == '\\'&&!charqueue.empty())
			{
				*in_viter=charqueue.front();
				charqueue.pop();
			}
		}
	}
	viter = initialviter;
}




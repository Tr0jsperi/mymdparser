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

std::string MdParser::trans_str(char ch)
{
	std::stringstream ss;
	ss << ch;
	std::string str = ss.str();
	return str;
}

std::string MdParser::trans_str(int num)
{
	std::stringstream ss;
	ss << num;
	std::string str = ss.str();
	return str;
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
		auto length = md_infile.tellg();//返回取文件指针在文件流中的位置
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
		process_escape_first(vit, totalstring,charqueue);		
		process_title(vit, totalstring);
		process_refer(vit, totalstring);
		process_linebreak(vit, totalstring);
		process_escape_last(vit, totalstring, charqueue);
		for (auto ptr = totalstring.begin(); ptr != totalstring.end(); ptr++)
		{
			html_outfile += (*ptr) + '\n';
		}
		md_infile.close();
	}
	return html_outfile;
}

//spcial symbol
//处理特殊字符  还没找到好办法规避html及code中无需转义的字符  暂时不启用
void MdParser::process_spc_sym(std::vector<std::string>::iterator & viter, std::vector<std::string>& totalstr)
{

	std::string spc_sym = "&<>";//存储特殊符号，防止html文件产生错误
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{
		std::string::iterator in_viter = (*viter).begin();
		for (std::string::iterator in_viter = (*viter).begin(); in_viter != (*viter).end();in_viter++)
		{
			if (spc_sym.find(*(in_viter)) != std::string::npos)
			{
				auto position = in_viter - (*viter).begin();
				switch (*in_viter)
				{
				case '&':
				{
					(*viter).replace(in_viter, in_viter + 1, "&amp");
					in_viter = (*viter).begin() + position + 4;
					break;
				}
				case '<':
				{
					(*viter).replace(in_viter, in_viter + 1, "&lt");
					in_viter = (*viter).begin() + position + 4;
					break;
				}
				}
				if (in_viter == (*viter).end()) break;
			}
		}
	}	
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
				auto linenum = viter - initialviter + 1;//用于错误提示
				std::cout << "出现奇数代码块标记"<<'\n'<<"未匹配代码块标记位于第"<<linenum<<"行"<<'\n';				
				*viter = codemark;
			}
		}
		//处理行内代码
		else
		{
			std::stack<char> stack_seq,stack_rev;//存储单行字符
			std::string newstr;//暂存处理后的整行文本
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
				auto linenum = viter - initialviter+1;//用于错误提示
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
//标记每行<code>标签起始位置
void MdParser::process_code_marker(std::vector<std::string>::iterator & viter, std::queue<std::size_t>& q_code_pos, std::queue<std::size_t>& q_code_end)
{
	std::size_t code_pos = 0, code_end_pos = 0;
	while ((*viter).find("<code>", code_pos) != std::string::npos)
	{
		code_pos = (*viter).find("<code>", code_pos);
		q_code_pos.push(code_pos);
		code_pos++;
		code_end_pos = (*viter).find("</code>", code_pos);
		q_code_end.push(code_end_pos);
	}
}
//跳过行内代码
void MdParser::process_inlinecode_skip(std::vector<std::string>::iterator& viter, std::string::iterator& in_viter, std::queue<std::size_t>& q_code_pos, std::queue<std::size_t>& q_code_end)
{
	if (!q_code_pos.empty() && in_viter - (*viter).begin() == q_code_pos.front())
	{
		in_viter = (*viter).begin() + q_code_end.front() + 6;
		q_code_pos.pop();
		q_code_end.pop();
	}
}


//escape
//提取转义字符暂存，原处用<escape>标记
void MdParser::process_escape_first(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::vector<std::string>::iterator initialviter = viter;
	std::string escape = "\\`*_{}[]()#+-.!";//存储转义字符，双斜杠避免将'\'错认为转义标志
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
			if (viter == totalstr.end() - 1)
			{
				break;//如果停在最后一行，结束处理
			}
			else
			{
				viter++;
			}
			//结束后viter指向</code>行下一行
		}
		if ((*viter).find("<code>") != std::string::npos)
		{
			process_code_marker(viter, q_code_pos, q_code_end);//向队列添加<code>起始标志位置，以便后续处理
		}	
		for (std::string::iterator in_viter = (*viter).begin(); in_viter != (*viter).end();)
		{				
			//判别是否到达行内代码标记，跳过行内代码
			process_inlinecode_skip(viter, in_viter, q_code_pos, q_code_end);
			//处理转义字符
			if((*in_viter) == '\\' && in_viter != ((*viter).end() - 1))//防止行末'\'符号导致之后+1迭代器错误
			{
				auto es_in = escape.find(*(in_viter + 1));//判断反斜杠后字符是否属于转义字符
				if (es_in != std::string::npos)
				{
					charqueue.push(*(in_viter + 1));
					auto pos = in_viter - (*viter).begin();//记录replace前指针位置
					*viter = (*viter).replace(in_viter,in_viter+2, "<escape>");
					in_viter = (*viter).begin() + pos + 8;
				}
				else
				{
					in_viter++;
				}
				
			}
			else
			{
				in_viter++;
			}
		}		
	}
	viter = initialviter;
}


//归还转义字符
void MdParser::process_escape_last(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr, std::queue<char>& charqueue)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end()) return;
	for (; viter != totalstr.end(); viter++)
	{
		if ((*viter).empty()) continue;
		auto pos = (*viter).find("<escape>");//记录<escape>标签位置
		while(pos!=(*viter).end()-(*viter).begin()&&pos!= (*viter).npos)
		{
			(*viter) = (*viter).replace(pos, 8, trans_str(charqueue.front()));
			charqueue.pop();
			pos = (*viter).find("<escape>", pos+1);
		}
	}
	viter = initialviter;
}


//linebreak
//行末两空格换行
void MdParser::process_linebreak(std::vector<std::string>::iterator&viter, std::vector<std::string>&totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end();viter++)
	{
		if ((*viter).empty())
		{
			*viter = *viter + "<br>";
		}
		if ((*viter).size() >= 10 && !((*viter).compare(0, 10, "<pre><code")))//跳过代码段
		{
			while (viter != totalstr.end() && ((*viter).size() < 13 || (*viter).compare(0, 13, "</code></pre>")))
			{
				viter++;
			}
			if (viter == totalstr.end() - 1)
			{
				break;//如果停在最后一行，结束处理
			}
			else
			{
				viter++;
			}//结束后viter指向</code>行下一行
		}
		else
		{
			std::string::iterator in_viter = (*viter).end() - 1;
			if ((*viter).size() > 1 && *(in_viter) == ' '&&*(in_viter - 1) == ' ')
			{
				*viter = *viter + "<br>";
			}
		}
	}
	viter = initialviter;
}

//list
//修改列表，共 个函数
void MdParser::process_list(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end())
	{
		return;
	}
	for (; viter != totalstr.end(); viter++)
	{

	}
}

//处理引用
void MdParser::process_refer(std::vector<std::string>::iterator &viter, std::vector<std::string> &totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end()) return;
	bool inblk = 0;//判定是否在引用块内，便于处理行首无'>'符号但属于引用块的情况
	for (; viter != totalstr.end(); viter++)
	{
		if ((*viter).empty())
		{
			inblk = 0;
			continue;
		}
		if (*(*viter).begin() == '>'||inblk==1)
		{
			inblk = 1;
			int line_qnum = 0;//记录单行引用块级数
			if (*(*viter).begin() != '>')
			{
				(*viter) += "</blockquote>";
				line_qnum += 1;
			}
			while (*(*viter).begin() == '>')
			{
				line_qnum++;
				(*viter).erase((*viter).begin());
				(*viter) += "</blockquote>";
			}			
			while (line_qnum)
			{
				(*viter) = "<blockquote>" + (*viter);
				line_qnum--;
			}
		}
	}
	viter = initialviter;
}
//处理标题
void MdParser::process_title(std::vector<std::string>::iterator & viter, std::vector<std::string> & totalstr)
{
	std::vector<std::string>::iterator initialviter = viter;
	if (viter == totalstr.end()) return;

	//处理标题	
	for (; viter != totalstr.end();viter++)
	{
		if ((*viter).empty()) continue;
		if ((*((*viter).begin())) == '#')
		{
			int hash = 0;//记录标题层级，最大为6
			std::string temp_str = (*viter);//暂存去头去尾整行文本
			while ((!temp_str.empty())&&(*(temp_str.begin())) == '#')
			{
				hash++;
				(temp_str).erase(temp_str.begin());
			}
			while ((!temp_str.empty()) && (*(temp_str.end()-1)) == '#')
			{
				(temp_str).erase(temp_str.end() - 1);
			}
			if (hash > 6) hash = 6;
			//整型转string
			std::stringstream ss;
			ss << hash;
			std::string hnum='h'+ss.str();
			(*viter) = '<' + hnum + '>' + temp_str + "</" + hnum + '>';
		}
		if ((*((*viter).begin())) == '>')
		{
			std::string temp_str = (*viter);//暂存去头去尾整行文本
			std::string::iterator temp_in_viter = (temp_str).begin();
			std::string quo_sym = "";//保存代码块'>'标志，处理完标题后还原
			while ((!temp_str.empty()) && (*(temp_str.begin()) == '>'))
			{
				quo_sym += '>';
				(temp_str).erase(temp_str.begin());
			}
			int hash = 0;//记录标题层级，最大为6
			while ((!temp_str.empty()) && (*(temp_str.begin())) == '#')
			{
				hash++;
				(temp_str).erase(temp_str.begin());
			}
			while ((!temp_str.empty()) && (*(temp_str.end() - 1)) == '#')
			{
				(temp_str).erase(temp_str.end() - 1);
			}
			if (hash > 6)
			{
				hash = 6;
			}
			if (hash > 0)
			{
				//整型转string
				std::string hnum = 'h' + trans_str(hash);
				(*viter) = quo_sym+'<' + hnum + '>' + temp_str + "</" + hnum + '>';
			}			
		}
	}
	viter = initialviter;
}
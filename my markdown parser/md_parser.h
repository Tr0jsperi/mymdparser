#ifndef MD_PARSER_H
#define MD_PARSER_H

class MdParser
{
public:
	MdParser(std::string);
	~MdParser();
	std::string GetContents();
	void process_spc_sym(std::vector<std::string>::iterator & viter, std::vector<std::string>& totalstr);
	void process_escape_first(std::vector<std::string>::iterator &, std::vector<std::string>&, std::queue<char>&);
	void process_escape_last(std::vector<std::string>::iterator &, std::vector<std::string>&, std::queue<char>&);
	void process_code(std::vector<std::string>::iterator &, std::vector<std::string>&);
	void process_linebreak(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void process_list(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_title(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void process_refer(std::vector<std::string>::iterator &, std::vector<std::string> &);
	
private:
	std::string file_position;
	std::queue<std::size_t> q_code_pos, q_code_end;

	void process_code_marker(std::vector<std::string>::iterator &, std::queue<std::size_t>&, std::queue<std::size_t>&);
	void process_inlinecode_skip(std::vector<std::string>::iterator&, std::string::iterator&, std::queue<std::size_t>&, std::queue<std::size_t>&);
	std::string trans_str(char);
	std::string trans_str(int);
};

#endif

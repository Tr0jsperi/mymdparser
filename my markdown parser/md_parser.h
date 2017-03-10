#ifndef MD_PARSER_H
#define MD_PARSER_H

class MdParser
{
public:
	MdParser(std::string);
	~MdParser();
	std::string GetContents();
	void process_escape(std::vector<std::string>::iterator &, std::vector<std::string>&, std::queue<char>&);
	void process_code(std::vector<std::string>::iterator &, std::vector<std::string>&);
	void process_code_marker(std::vector<std::string>::iterator &, std::queue<std::size_t>&, std::queue<std::size_t>&);
	void process_inlinecode_skip(std::vector<std::string>::iterator& viter, std::string::iterator& in_viter, std::queue<std::size_t>& q_code_pos, std::queue<std::size_t>& q_code_end);
	
private:
	std::string file_position;
	std::queue<std::size_t> q_code_pos, q_code_end;
};

#endif

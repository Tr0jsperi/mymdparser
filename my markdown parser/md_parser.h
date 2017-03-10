#ifndef MD_PARSER_H
#define MD_PARSER_H

class MdParser
{
public:
	MdParser(std::string);
	~MdParser();
	std::string GetContents();
	void process_escape_first(std::vector<std::string>::iterator &, std::vector<std::string>&, std::queue<char>&);
	void process_escape_last(std::vector<std::string>::iterator &, std::vector<std::string>&, std::queue<char>&);
	void process_code(std::vector<std::string>::iterator &, std::vector<std::string>&);
	
private:
	std::string file_position;
};

#endif

#ifndef MD_PARSER_H
#define MD_PARSER_H

class MdParser
{
public:
	MdParser(std::string);
	~MdParser();
	std::string GetContents();
	
private:
	std::string file_position;
};

#endif

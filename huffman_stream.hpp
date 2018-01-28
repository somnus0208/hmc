#ifndef _HUFFMANOUTPUTSTREAM_
#define _HUFFMANOUTPUTSTREAM_
#include <ostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "huffman_tree.hpp"

class HuffmanStream
{
public:
	enum class CompressionMode
	{
		Compress,
		Decompress
	};
public:
	HuffmanStream(std::ostream* outputStream,CompressionMode mode);
public:
	bool operator << (std::ifstream& inputStream);
	~HuffmanStream();
private:
	HuffmanStream();
	std::vector<bool> GetCryptedCode(char c);
	
private:
	CompressionMode _mode;
	std::ostream* _sourceStream;
	HuffmanTree* _tree;
	std::map<char,std::vector<bool>> _codeMap;
};

#endif //_HUFFMANOUTPUTSTREAM_


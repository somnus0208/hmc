#include <istream>
#include <ostream>


#include "huffman_tree_item.hpp"

using std::istream;
using std::ostream;


HuffmanTreeItem::HuffmanTreeItem()
{
	_content.c = -1;
	_content.type = NodeType::DEFAULT;
	_content.percent = 0;
	_left = _right = _parent = nullptr;

}

HuffmanTreeItem::HuffmanTreeItem(NodeContent content)
{
	_content = content;
}

HuffmanTreeItem::HuffmanTreeItem( HuffmanTreeItem& item )
{

}

void HuffmanTreeItem::SetParent(HuffmanTreeItem* parent)
{
	_parent = parent;
}

void HuffmanTreeItem::SetLeft(HuffmanTreeItem* left)
{
	_left = left;
}

void HuffmanTreeItem::SetRight(HuffmanTreeItem* right)
{
	_right = right;
}

HuffmanTreeItem::NodeContent HuffmanTreeItem::GetContent() 
{
	return _content;
}

HuffmanTreeItem* HuffmanTreeItem::GetParent()
{
	return _parent;
}

HuffmanTreeItem* HuffmanTreeItem::GetLeft()
{
	return _left;
}

HuffmanTreeItem* HuffmanTreeItem::GetRight()
{
	return _right;
}

ostream& operator << (ostream& os,HuffmanTreeItem* const item)
{
	os << item->_content;
	return os;
}

istream& operator >> (istream& is,HuffmanTreeItem* const item)
{
	is >> item->_content;
	return is;
}

ostream& operator << (ostream& os,HuffmanTreeItem::NodeContent& content)
{
	//the char value
	int c = content.c;
	os.put(c);
	// the percent
	//Warning:this behavior is no endianness
	double percent = content.percent;
	char* bytes_percent = reinterpret_cast<char*>(&percent);
	for (auto i = 0;i<sizeof(percent);i++)
	{
		os.put(*bytes_percent);
		++bytes_percent;		

	}
	return os;

}

istream& operator >> (istream& is, HuffmanTreeItem::NodeContent& content)
{
	content.c = is.get();
	char bytes_percent[sizeof(content.percent)];
	is.read(bytes_percent,sizeof(content.percent));
	double* percent = reinterpret_cast<double*>(bytes_percent);
	content.percent = *percent;
	content.type = HuffmanTreeItem::NodeType::LEAF;
	return is;
}

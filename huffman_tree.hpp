#ifndef _BINARYTREE_
#define _BINARYTREE_

#include <ostream>
#include <istream>
#include <vector>

#include "huffman_tree_item.hpp"

using std::vector;
using std::ostream;
using std::istream;

class HuffmanTree
{
	friend ostream& operator << (ostream &os,HuffmanTree* const tree);
	friend istream& operator >> (istream &os,HuffmanTree* const tree);
	friend class HuffmanStream;
public:
	static HuffmanTree* Parse(std::istream& input); 

public:
	HuffmanTreeItem* GetRootItem() const;
	HuffmanTreeItem* GetTerminalItem(char c) const;

public:


private:
	HuffmanTree();
	HuffmanTree(HuffmanTree& binaryTree);
	~HuffmanTree();
private:

	void MakeChildItem();

private:
	HuffmanTreeItem* _rootNode;
	vector<HuffmanTreeItem*> _terminalNodes;
	vector<HuffmanTreeItem*> _innerNodes;
	

};

ostream& operator << (ostream &os,HuffmanTree* const tree);
istream& operator >> (istream &is,HuffmanTree* const tree);

#endif // !_BINARYTREE_
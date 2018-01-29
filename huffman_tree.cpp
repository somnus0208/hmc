#include <map>
#include <vector>
#include <ostream>
#include <istream>
#include <memory>
#include <algorithm>
#include <utility>
#include <iostream>

#include <cmath>

#include "huffman_tree.hpp"
#include "huffman_tree_item.hpp"


using std::map;
using std::vector;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::char_traits;
using std::pair;
using std::shared_ptr;
using std::unique_ptr;

using std::for_each;
using std::find_if;
using std::make_shared;
using std::make_pair;


HuffmanTree* HuffmanTree::Parse(istream& input)
{
    input.seekg(0,input.end);
    long long process = input.tellg();
    input.seekg(0,input.beg);

    //Create a map to store the counter for each character
    auto byteMap = make_shared<map<char,long>>();
    //verify the input stream
    auto c = input.get();
    if (c == char_traits<char>::eof()) return nullptr;
    //The global character counter

    double length = 0;
    //Read and count not until the input stream ends
    while (!input.eof())
    {
        auto emplace_result = byteMap->emplace(c,0L);
        if ( emplace_result.second == false) (*emplace_result.first).second++;
        length++;
        c = input.get();

    }
    //Initialize the return tree
    auto tree = new HuffmanTree();
    //Traversing the initial map to store the tree's terminal node
    for (auto it = byteMap->begin();it!=byteMap->end();it++)
    {
        HuffmanTreeItem::NodeContent content;
        content.c = it->first;
        content.percent = it->second / length;
        content.type = HuffmanTreeItem::NodeType::LEAF;
        HuffmanTreeItem* item = new HuffmanTreeItem(content);
        //Store terminal tree node for the return tree
        tree->_terminalNodes.push_back(item);
    }

    tree->MakeChildItem();

    return tree;
}

HuffmanTreeItem* HuffmanTree::GetRootItem() const
{
    return _rootNode;
}

HuffmanTreeItem* HuffmanTree::GetTerminalItem( char c ) const
{
    auto it = find_if(_terminalNodes.begin(),_terminalNodes.end(),[=](HuffmanTreeItem* item) -> bool 
    {
        return item->GetContent().c == c;
    });
    return it == _terminalNodes.end() ? nullptr : *it;
    
}

HuffmanTree::HuffmanTree()
{

}

void HuffmanTree::MakeChildItem()
{
    //the temporary collection for Huffman binary tree item generation
    auto collection = make_shared<vector<HuffmanTreeItem*>>();
    for_each(begin(_terminalNodes),end(_terminalNodes),[=](HuffmanTreeItem* item) -> void 
    {
        collection->push_back(item);
    });
    while (collection->size() != 1)
    {
        //sort by the ascending order
        sort(collection->begin(),collection->end(),[](HuffmanTreeItem* left,HuffmanTreeItem* right) -> bool
        {
            return left->GetContent().percent < right->GetContent().percent;
        });
        auto first = collection->at(0);
        auto second = collection->at(1);

        HuffmanTreeItem::NodeContent content;
        content.type = HuffmanTreeItem::NodeType::MIDDLE;
        content.percent = first->GetContent().percent + second->GetContent().percent;
        //char value -1 stands for no character
        content.c = -1;    

        HuffmanTreeItem* item = new HuffmanTreeItem(content);
        //Set the parent-child relations
        first->SetParent(item);
        second->SetParent(item);
        item->SetLeft(first);
        item->SetRight(second);
        //Remove these two nodes having been handled
        collection->erase(collection->begin());
        collection->erase(collection->begin());
        //Add the middle tree node which has just been created
        collection->push_back(item);
        //set the return tree
        if (collection->size() != 1)
            _innerNodes.push_back(item);
        else
            _rootNode = item;
    }
}

HuffmanTree::~HuffmanTree()
{
    delete _rootNode;
    for_each(_terminalNodes.begin(),_terminalNodes.end(),[] (HuffmanTreeItem* item) -> void 
    {
        delete item;
    });
    for_each(_innerNodes.begin(),_innerNodes.end(),[] (HuffmanTreeItem* item) -> void 
    {
        delete item;
    });

}

ostream& operator << (ostream &os,HuffmanTree* const tree)
{
    
    //Store the size of the terminal node
    size_t size = tree->_terminalNodes.size();
    char bytes[sizeof(size)];
    for (auto i = 0;i<sizeof(size);i++)
    {
        bytes[i] = size & 0xFF;
        size >>= 8;
    }
    os.write(bytes,1);
    for_each(begin(tree->_terminalNodes),end(tree->_terminalNodes),[&](HuffmanTreeItem* item) -> void
    {
        os << item;
    });
    return os;
}

istream& operator >> (istream& is,HuffmanTree* const tree)
{

    //Read the quantity of terminal nodes
    char size_c = 0;
    is.get(size_c);
    unsigned char size = *reinterpret_cast<unsigned char*>(&size_c);
    //De serialize each terminal node
    for (auto i = 0; i < size; ++i)
    {
        HuffmanTreeItem* item = new HuffmanTreeItem(); 
        is >> item;
        tree->_terminalNodes.push_back(item);
    }
    tree->MakeChildItem();
    return is;
}





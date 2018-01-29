#ifndef _BINARYITEM_
#define _BINARYITEM_
#include <istream>
#include <ostream>

class HuffmanTreeItem
{
    friend std::ostream& operator << (std::ostream& os,HuffmanTreeItem* item);
    friend std::istream& operator >> (std::istream& is,HuffmanTreeItem* item);
    
public:
    enum class NodeType{
        LEAF,
        MIDDLE,
        DEFAULT
    };

    struct NodeContent 
    {
        double percent;
        char c;
        NodeType type;
    } ;

public:

    HuffmanTreeItem();
    HuffmanTreeItem(NodeContent content);
    void SetParent(HuffmanTreeItem* parent);

    void SetLeft(HuffmanTreeItem* left);
    void SetRight(HuffmanTreeItem* right);
    NodeContent GetContent() ;
    HuffmanTreeItem* GetParent();
    HuffmanTreeItem* GetLeft();
    HuffmanTreeItem* GetRight();

private:
    HuffmanTreeItem(HuffmanTreeItem& item);

private:

    HuffmanTreeItem* _left;
    HuffmanTreeItem* _right;
    HuffmanTreeItem* _parent;

private:
    NodeContent _content;
};

std::ostream& operator << (std::ostream& os,HuffmanTreeItem* const item);
std::istream& operator >> (std::istream& is,HuffmanTreeItem* const item);
std::ostream& operator << (std::ostream& os,HuffmanTreeItem::NodeContent& content);
std::istream& operator >> (std::istream& is,HuffmanTreeItem::NodeContent& content);

#endif //!_BINARYITEM_
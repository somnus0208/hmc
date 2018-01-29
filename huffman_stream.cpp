#include <iostream>
#include <fstream>
#include <map>
#include <forward_list>
#include <list>
#include <utility>
#include <algorithm>
#include <queue>

#include <omp.h>

#include <ctime>

#include "huffman_stream.hpp"

using std::ostream;
using std::ifstream;
using std::forward_list;
using std::streamoff;
using std::list;
using std::map;
using std::pair;

using std::for_each;
using std::make_pair;
using std::begin;
using std::end;
using std::copy;


HuffmanStream::HuffmanStream( ostream* outputStream,CompressionMode mode )
{
    HuffmanStream();
    _sourceStream = outputStream;
    _mode = mode;

}

HuffmanStream::~HuffmanStream()
{

};

HuffmanStream::HuffmanStream()
{
    _sourceStream = nullptr;
    _tree = nullptr;
}

bool HuffmanStream::operator<<(ifstream& inputStream )
{
    /*For OpenMP use*/
    int coreNum = omp_get_num_procs();    
    int charCacheSize = 1024; 

    if (!inputStream.is_open())
    {
        std::cerr<<"read input file failure";
        exit(EXIT_FAILURE);
    }
    if (_mode == CompressionMode::Compress)
    {
        //Write the magic number for this kind of Encoding
        unsigned char magic[] = {0xBE,0xCC};    //Abbreviation for the "Bad Experience with College in China"
        char* magic_bytes = reinterpret_cast<char*>(magic);
        _sourceStream->write(magic_bytes,sizeof(magic));

        //Save the tree
        _tree = HuffmanTree::Parse(inputStream);
        (*_sourceStream)<<_tree;

        //Save the length of the input
        inputStream.clear();
        streamoff length = inputStream.tellg();
        char* bytes_length = reinterpret_cast<char*>(&length);
        _sourceStream->write(bytes_length,sizeof(streamoff));

        //Store the code for each character
        inputStream.seekg(0,inputStream.beg);
        
        //Use OpenMP to calculate each character code 
        //Map
        map<char,vector<bool>>* eachMap = new map<char,vector<bool>> [coreNum];
        size_t size = _tree->_terminalNodes.size();
        #pragma omp parallel for
        for (auto i=0;i<size;i++)
        {
            forward_list<bool> list;
            char c = _tree->_terminalNodes[i]->GetContent().c;
            int k = omp_get_thread_num();
            HuffmanTreeItem* terminal = _tree->GetTerminalItem(c);
            HuffmanTreeItem* item = terminal;
            while (item != _tree->GetRootItem())
            {
                if (item->GetParent()->GetLeft() == item)
                    list.push_front(0);
                else
                    list.push_front(1);
                item = item->GetParent();
            }
            eachMap[k].insert(make_pair(c,vector<bool>(list.begin(),list.end())));
        }
        //Reduce
        for (auto i=0;i<coreNum;i++)
        {
            for_each(eachMap[i].begin(),eachMap[i].end(),[&](pair<char,vector<bool>> p) -> void
            {
                _codeMap.insert(p);
            });


        }
        //Encode the file using map-reduce method
        time_t t1 = clock();
        vector<bool>* allcodes = new vector<bool>();
        vector<char>* cache = new vector<char>[coreNum];
        vector<bool>* cacheCode = new vector<bool>[coreNum];
        while (!inputStream.eof())
        {
            int endTaskNum = coreNum;
            bool isFileEnd = false;
            for (auto i=0; i<coreNum;i++)
            {
                if (!cache[i].empty()) cache[i].erase(cache[i].begin(),cache[i].end());

                for (auto j=0;j<charCacheSize;j++)
                {
                    char c = inputStream.get();
                    if (inputStream.eof()) 
                    {
                        endTaskNum = i + 1;
                        isFileEnd = true;
                        break;
                    }
                    cache[i].push_back(c);
                }
                if (isFileEnd) break;
            }
#pragma omp parallel num_threads(endTaskNum)
            {
                int k = omp_get_thread_num();
                for (auto i=0;i<cache[k].size();i++)
                {
                    auto codes = _codeMap[cache[k].at(i)];
                    cacheCode[k].resize(cacheCode[k].size() + codes.size());
                    copy(codes.begin(),codes.end(),cacheCode[k].begin()+cacheCode[k].size()-codes.size());
                }
            }

            for (auto i = 0;i < endTaskNum;i++)
            {
                allcodes->resize(allcodes->size()+cacheCode[i].size());
                copy(cacheCode[i].begin(),cacheCode[i].end(),allcodes->begin()+allcodes->size()-cacheCode[i].size());
                cacheCode[i].erase(cacheCode[i].begin(),cacheCode[i].end());
            }

        }
        time_t t2 = clock();
        std::cout<<"generate code using openMP:";
        std::cout << t2-t1 <<"ms"<<std::endl;
        time_t t3 = clock();
        inputStream.clear();
        inputStream.seekg(0,inputStream.beg);
        vector<bool>* allcodes2 = new vector<bool>();
        char c=inputStream.get();
        while (!inputStream.eof())
        {
            auto codes = _codeMap[c];
            allcodes2->resize(allcodes2->size()+codes.size());
            copy(codes.begin(),codes.end(),allcodes->begin()+allcodes->size()-codes.size());
            c = inputStream.get();
        }
        //time_t t4 = clock();
        //std::cout<<"generate codes using sequential method:";
        //std::cout<<t4-t3<<"ms"<<std::endl;
        //Convert to bytes and Write the code to stream
        unsigned char c_byte = 0x00;
        for (vector<bool>::size_type i=0;i<allcodes->size();i++)
        {
            c_byte <<= 1;
            c_byte += (allcodes->at(i) & 0x01 );
            if ((i+1) % 8 ==0)
            {
                _sourceStream->put(c_byte);
                c_byte = 0x00;
            }
        } 
        //Check the last bits to form a byte
        int left = allcodes->size() % 8;
        if (left != 0)
        {
            c_byte <<= (8-left);
            _sourceStream->put(c_byte);
        }
        //Garbage collection
        delete allcodes;
        delete _tree;
        return true;
    }
    else
    {
        //Check the magic number
        unsigned char magic[] = {0x00,0x00};
        inputStream.read(reinterpret_cast<char*>(magic),2);
        if (!(magic[0] == 0xBE && magic[1] == 0xCC))
        {
            std::cerr << "input file is not a valid compressed file.";
            std::cerr << std::endl;
            exit(0);
        }
        _tree = new HuffmanTree();
        inputStream >> _tree;
        //Read the length of the original file
        char  bytes_length[sizeof(streamoff)];
        inputStream.read(bytes_length,sizeof(streamoff));
        streamoff length = *reinterpret_cast<streamoff*>(bytes_length);
        //Get the bits data
        list<bool>* allCodes = new list<bool>();
        streamoff now = inputStream.tellg();
        while (!inputStream.eof())
        {
            char bytes = inputStream.get();
            for (auto i = 0;i<sizeof(char) * 8;i++)
            {
                allCodes->push_back(((bytes & 0x80) >> 7) & 1);
                bytes <<= 1;
            }
        }
        streamoff lengthit = 0;
        HuffmanTreeItem* item = _tree->GetRootItem();
        while (lengthit < length)
        {
            bool bit = allCodes->front();
            allCodes->pop_front();
            if (bit == 0) item = item->GetLeft();
            else item = item -> GetRight();
            if (item->GetContent().type == HuffmanTreeItem::NodeType::LEAF)
            {
                _sourceStream->put(item->GetContent().c);
                item = _tree->GetRootItem();
                lengthit ++;
            }
        }
        //Garbage Collection
        delete allCodes;
        delete _tree;
        return true;
    }
    return false;
}

std::vector<bool> HuffmanStream::GetCryptedCode( char c )
{
    if (_codeMap.find(c) != _codeMap.end()) return _codeMap[c];
    forward_list<bool> list;
    HuffmanTreeItem* terminal = _tree->GetTerminalItem(c);
    HuffmanTreeItem* item = terminal;
    while (item != _tree->GetRootItem())
    {
        if (item->GetParent()->GetLeft() == item)
            list.push_front(0);
        else
            list.push_front(1);
        item = item->GetParent();
    }
    _codeMap.insert(make_pair(c,vector<bool>(list.begin(),list.end())));
    return _codeMap[c];
}




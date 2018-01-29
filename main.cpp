#include <iostream>
#include <fstream>
#include <ctime>
#include "huffman_stream.hpp"

using namespace std;

void PrintDefalutMessage()
{
    cout << "You must specify one of the \'-dc\' options." << endl;
    cout << "Try with -h for more information." << endl;
}

void PrintHelpMessage()
{
    cout << "usage: hmc [OPTIONS...] [FILE]" << endl;
    cout << endl;
    cout << "examples:"<<endl;
    cout << "  hmc -c archive           #Create compressed file from archive"<<endl;
    cout << "  hmc -d archive.huf       #Decompress archive.huf to current directory" <<endl;
    cout << endl;
    cout << " main operation mode:"<<endl;
    cout << "  -c                       compression mode"<<endl;
    cout << "  -d                       decompression mode"<<endl;
    cout <<endl;
    cout << "Report bugs to somnus0208@gmail.com"<<endl; 
}

void Decompress(string filename)
{
    string exportFilename = filename.substr(0,filename.find_last_of('.'));
    ofstream out(exportFilename,ios::binary);
    HuffmanStream output(&out,HuffmanStream::CompressionMode::Decompress);
    ifstream in(filename,ios::binary);
    output << in;
}

void Compress(string filename)
{
    string exportFilename = filename +".huf";
    ofstream out(exportFilename,ios::binary);
    ifstream in(filename,ios::binary);
    HuffmanStream output(&out,HuffmanStream::CompressionMode::Compress);
    output << in;
    out.clear();
    in.clear();
    out.seekp(0,out.end);
    in.seekg(0,in.end);
    streamoff rate = out.tellp() * 100 / in.tellg();
    cout << "compressed rate:"<<rate<<"%."<<endl;


}

int main(int argc,char* argv[])
{
    string* options = new string[argc];
    time_t t1;
    time_t t2;
    for (int i=0;i<argc;i++)
        options[i] = argv[i];

    switch (argc)
    {
    case 2:
        if (options[1] == "-h") PrintHelpMessage();
        break;

    case 3:
        if (options[1] == "-d")
        {
            t1 = clock();
            Decompress(options[2]);
            t2 = clock();
            cout<<"Decompressed Success.counts "<<t2-t1<<"ms."<<endl;
            cout<<endl;
        }

        if (options[1] == "-c")
        {
            t1 = clock();
            Compress(options[2]);
            t2 = clock();
            cout<<"Compressed Success.counts "<<t2-t1<<"ms."<<endl;
            cout<<endl;
        }
        break;

    default:
        PrintDefalutMessage();
        break;
    }
}


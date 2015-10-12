#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <regex>

using namespace std;

int main(int argc, char * argv[])
{
    ifstream fin(argv[1]);
    if(!fin)
    {
        cerr << "Can't open " << argv[1] << endl;
        return -1;
    }

    regex version(R"(GL_VERSION_(\d+)_(\d+))");
    regex type(R"(\(APIENTRYP\s+([^\)]+))");
    regex name(R"(APIENTRY\s+(\S+))");
    regex endif(R"(#endif)");

    int MajorVersion = -1;
    int MinorVersion = -1;

    ofstream hout("glLoadAddress.h");
    ofstream fout("glLoadAddress.cpp");

    vector<pair<int, int>> Versions;
    map<pair<int, int>, vector<string>> VersionToTypes;
    map<pair<int, int>, vector<string>> VersionToNames;
    pair<int, int> CurrentVersion;
    vector<string> types;
    vector<string> names;

    string line;
    while(getline(fin, line))
    {
        smatch match;
        if(regex_search(line, match, version))
        {
            if(regex_search(line, endif))
            {
                if(MajorVersion >= 0 && MinorVersion >= 0)
                {
                    VersionToTypes[CurrentVersion] = types;
                    VersionToNames[CurrentVersion] = names;
                }

                MajorVersion = -1;
                MinorVersion = -1;
            }
            else
            {
                int major = stoi(match[1].str());
                int minor = stoi(match[2].str());

                if(major <= 1 && minor <= 1)
                {
                    continue ;
                }

                if(major > MajorVersion || minor > MinorVersion)
                {
                    MajorVersion = major;
                    MinorVersion = minor;

                    CurrentVersion = make_pair(MajorVersion, MinorVersion);

                    Versions.push_back(CurrentVersion);
                    types.clear();
                    names.clear();

                }
            }
        }
        else if(regex_search(line, match, type))
        {
            types.push_back(match[1]);
        }
        else if(regex_search(line, match, name))
        {
            if(types.size())
            {
                names.push_back(match[1]);
            }
        }
    }

    hout << "#pragma once" << endl;
    hout << "#ifndef GL_LOAD_ADDRESS_H_INCLUDED" << endl;
    hout << "#define GL_LOAD_ADDRESS_H_INCLUDED" << endl;
    hout << endl;
    hout << "#include \"glcorearb.h\"" << endl;
    hout << endl;
    hout << "extern \"C\" {" << endl;
    hout << endl;
    hout << "void glLoadAddress(int MajorVersion, int MinorVersion);" << endl;
    for(auto & v : Versions)
    {
        auto & t = VersionToTypes[v];
        auto & n = VersionToNames[v];

        for(size_t i = 0; i < t.size(); ++i)
        {
            hout << "extern " << t[i] << " " << n[i] << ";" << endl;
        }
    }
    hout << "}" << endl;
    hout << "#endif /* GL_LOAD_ADDRESS_H_INCLUDED */" << endl;

    fout << "#include \"glLoadAddress.h\"" << endl;
    fout << endl;
    fout << "extern \"C\" {" << endl;
    for(auto & v : Versions)
    {
        auto & t = VersionToTypes[v];
        auto & n = VersionToNames[v];

        for(size_t i = 0; i < t.size(); ++i)
        {
            fout << t[i] << " " << n[i] << " = nullptr;" << endl;
        }
    }

    fout << endl;

    fout << "void glLoadAddress(int MajorVersion, int MinorVersion)" << endl;
    fout << "{" << endl;
    for(auto & v : Versions)
    {
        auto & t = VersionToTypes[v];
        auto & n = VersionToNames[v];

        fout << "\tif(MajorVersion > " << v.first << " || (MajorVersion >= " << v.first << " && MinorVersion >= " << v.second << "))" << endl;
        fout << "\t{" << endl;
        for(size_t i = 0; i < t.size(); ++i)
        {
            fout << "\t\t" << n[i] << " = reinterpret_cast<" << t[i] << ">(wglGetProcAddress(\"" << n[i] << "\"));" << endl;
        }
        fout << "\t}" << endl;
        fout << endl;
    }
    fout << "}" << endl;
    fout << endl;
    fout << "} /* extern \"C\" */" << endl;

    return 0;
}
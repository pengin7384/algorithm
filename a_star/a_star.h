#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <map>

#define EOFILE 1
#define ENEW 2
#define EUNK 3

namespace a_star
{

class AStar
{
    std::vector< std::vector<bool> > map;
    std::vector< std::vector<int> > path;

    int ans = -1;
    int s_x, s_y, d_x, d_y;
    int m_x, m_y;
    int d[8][2] = {
        {0, -1}, {0, 1},
        {-1, 0}, {1, 0},
        {-1, -1}, {1, -1},
        {1, 1}, {-1, 1}};

    struct Pos
    {
        bool open;
        int x, y;
        int f, g, h;
    };


    struct Cmp
    {
        bool operator()(const Pos *a, const Pos *b) const
        {
            return a->f > b->f;
        }
    };

public:
    AStar();

    ~AStar();

    void freeMap();

    void createMap();

    int loadMap(std::ifstream &file);

    int loadFile(const char *file_path);

    template <class Q, class M>
    void pushStartPos(Q &q, M &m);

    template <class Q, class M>
    void pushPos(Q &q, M &m, Pos *pos, int direction);

    template <class Q, class M>
    bool search(Q &q, M &m, Pos *pos);
    
    void calculate();

    void calculatePath();

    int saveFile(const char *file_path);

    void saveResult(const char *file_path);

    template <class Q>
    void clear(Q &q);

    void getSize(std::ifstream &file);

    int getH(int x, int y);

    int getKey(int x, int y);
};

} 

#include "a_star.hpp"

namespace a_star
{
AStar::AStar() {}

AStar::~AStar()
{
    freeMap();
}

void AStar::freeMap()
{
    for (int i = 0; i < map.size(); i++) {
        map[i].clear();
        path[i].clear();
    }
    map.clear();
    path.clear();
}

void AStar::createMap()
{
    map.assign(m_y, std::vector<bool>(m_x, false));
    path.assign(m_y, std::vector<int>(m_x, -1));
}

int AStar::loadMap(std::ifstream &file)
{
    std::string line;
    int y = 0;

    while (getline(file, line)) {

        if (y >= m_y) {
            return -EUNK;
        }

        for (int x = 0; x < m_x; x++) {
            switch (line.at(x)) {
            case '.':
                map[y][x] = false;
                break;
            case 'o':
                map[y][x] = true;
                break;
            case 'S':
                s_x = x;
                s_y = y;
                break;
            case 'F':
                d_x = x;
                d_y = y;
                break;
            default:
                return -EUNK;
            }
        }
        y++;
    }
    return 0;
}

int AStar::loadFile(const char *file_path)
{
    std::ifstream file(file_path);

    if (!file.is_open())
        return -EOFILE;

    getSize(file);
    createMap();
    loadMap(file);

    return 0;
}

template <class Q, class M>
void AStar::pushStartPos(Q &q, M &m)
{
    Pos *s_pos = new Pos();
    s_pos->open = true;
    s_pos->x = s_x;
    s_pos->y = s_y;
    s_pos->f = s_pos->g = s_pos->h = 0;

    q.push(s_pos);
    m[getKey(s_x, s_y)] = s_pos;
}

template <class Q, class M>
void AStar::pushPos(Q &q, M &m, Pos *pos, int direction) 
{
    q.push(pos);
    m[getKey(pos->x, pos->y)] = pos;
    path[pos->y][pos->x] = direction;
}

template <class Q, class M>
bool AStar::search(Q &q, M &m, Pos *pos)
{
    /* Already searched */
    if (map[pos->y][pos->x] || !pos->open) {
        delete pos;
        return true;
    }

    if (pos->x == d_x && pos->y == d_y) {
        ans = pos->g;
        clear(q);
        return false;
    }

    map[pos->y][pos->x] = true;

    for (int i = 0; i < 8; i++) {
        int dx = pos->x + d[i][0], dy = pos->y + d[i][1];

        if (dx < 0 || dx >= m_x || dy < 0 || dy >= m_y) 
            continue;
        if (map[dy][dx]) 
            continue;                    
            
        Pos *n_pos = new Pos;
        n_pos->open = true;
        n_pos->x = dx;
        n_pos->y = dy;
        n_pos->h = getH(dx, dy);

        /* ↑ ↓ ← → */
        if (i < 4)
            n_pos->g = pos->g + 10;
            
        /* ↖ ↗ ↘ ↙ */
        else
            n_pos->g = pos->g + 14;
            
        n_pos->f = n_pos->g + n_pos->h;


        int n_key = getKey(n_pos->x, n_pos->y);
        if (m.find(n_key) != m.end()) {
            if (n_pos->g < m[n_key]->g) {

                Pos *tmp = m[n_key];
                tmp->open = false;    
                m.erase(n_key);
                pushPos(q, m, n_pos, i);
    
            }
        } else {
            pushPos(q, m, n_pos, i);
        }
    }

    delete pos;

    return true;
}

void AStar::calculate()
{
    std::priority_queue<Pos*, std::vector<Pos*>, Cmp> q;
    std::map<int, Pos*> m;
     
    pushStartPos(q, m);

    while (!q.empty()) {
    
        Pos *pos = q.top();
        q.pop();

        if (!search(q, m, pos)) {
            break;
        }

    }
}


void AStar::calculatePath()
{
    int x = d_x, y = d_y;
        
    while (x != s_x || y != d_y) {
        if (path[y][x] == -1) {
            break;
        }
            
        int dx = x - d[path[y][x]][0], dy = y - d[path[y][x]][1];
        path[y][x] = 9;
        x = dx;
        y = dy;
    }

    path[s_y][s_x] = 10;
    path[d_y][d_x] = 11;
}

int AStar::saveFile(const char *file_path) 
{
    std::ofstream file(file_path);
    if (file.is_open()) {
        for (int y = 0; y < m_y; y++) {
            for (int x = 0; x < m_x; x++) {

                switch (path[y][x]) {
                case -1:
                    if (map[y][x])  file << "■";
                    else            file << "＊";
                    break; 
                case 0:     file << "↑"; break;
                case 1:     file << "↓"; break;
                case 2:     file << "←"; break;
                case 3:     file << "→"; break;
                case 4:     file << "↖"; break;
                case 5:     file << "↗"; break;
                case 6:     file << "↘"; break;
                case 7:     file << "↙"; break; 
                case 9:     file << "◆"; break; 
                case 10:    file << "☆"; break; 
                case 11:    file << "★"; break; 
                default:   
                    return -EUNK;
                }
                file << "";
            }
            file << "\n";
        }  
        return 0;
    }
    return -EOFILE;
}

void AStar::saveResult(const char *file_path)
{
    if (ans != -1) {
        printf("Total cost : %d\n", ans); 
        calculatePath();
        if (saveFile(file_path) == 0) 
            printf("Saved successfully! (%s)\n", file_path);
            
    } else {
        puts("Can't find path!");
    }
}

template <class Q>
void AStar::clear(Q &q)
{
    while (!q.empty()) {
        Pos *tmp = q.top();
        delete tmp;
        q.pop();
    }
}


void AStar::getSize(std::ifstream &file)
{
    std::string line;
    int y = 0;

    while (getline(file, line)) {
        if (y == 0)
            m_x = line.length() - 1; 
        y++;
    }
    m_y = y;

    file.clear();
    file.seekg(0, std::ios::beg);
}

int AStar::getH(int x, int y)
{
    int tot = abs(d_x - x);
    tot += abs(d_y - y);

    return 10 * tot;
}

int AStar::getKey(int x, int y)
{
    return x + y * m_x;
}

}
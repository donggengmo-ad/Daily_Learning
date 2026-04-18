#include <iostream>
#include <vector>
using namespace std;

struct Point{
    int x, y;
    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
    bool equal(Point p){
        return this->x == p.x && this->y == p.y;
    }
    char getDir(Point p){// p is the next point
        if(p.x == this->x + 1 && p.y == this->y) return 'D';
        if(p.x == this->x - 1 && p.y == this->y) return 'U';
        if(p.x == this->x && p.y == this->y + 1) return 'R';
        if(p.x == this->x && p.y == this->y - 1) return 'L';
        return 'X';
    }
    bool isin(vector<Point> path){
        for(Point p : path){
            if(this->equal(p)) return true;
        }
        return false;
    }
};

int n;
Point start(1, 1), target(0, 0), block(0, 0);

bool dfs(Point now, vector<Point> &path, int step, bool found = false){
    if(found) return true;
    if(step > n * n - 2) return false;
    if(now.equal(target) && step == n * n - 2){
        cout << "Yes" << endl;
        for(int i = 0; i < path.size() - 1; i++){
            cout << path[i].getDir(path[i+1]);
        }
        cout << endl;
        return true;
    }
    if(now.x > 1){
        Point next(now.x - 1, now.y);
        if(!next.equal(block) && !next.equal(start) && !next.isin(path)){
            path.push_back(next);
            found = dfs(next, path, step + 1, found) || found;
            path.pop_back();
        }
    }
    if(now.x < n){
        Point next(now.x + 1, now.y);
        if(!next.equal(block) && !next.equal(start) && !next.isin(path)){
            path.push_back(next);
            found = dfs(next, path, step + 1, found) || found;
            path.pop_back();
        }
    }
    if(now.y > 1){
        Point next(now.x, now.y - 1);
        if(!next.equal(block) && !next.equal(start) && !next.isin(path)){
            path.push_back(next);
            found = dfs(next, path, step + 1, found) || found;
            path.pop_back();
        }
    }
    if(now.y < n){
        Point next(now.x, now.y + 1);
        if(!next.equal(block) && !next.equal(start) && !next.isin(path)){
            path.push_back(next);
            found = dfs(next, path, step + 1, found) || found;
            path.pop_back();
        }
    }
    return found;
}

int main(){
    int t;
    cin >> t;
    while(t--){
        int a, b;
        cin >> n >> a >> b;
        target = Point(n, n);
        block = Point(a, b);

        vector<Point> path;
        path.push_back(start);

        bool found = dfs(start, path, 0);
        if(!found) cout << "No" << endl;
    }

    return 0;
}
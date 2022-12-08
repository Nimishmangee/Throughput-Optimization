#include<iostream>
#include<vector>
#include<climits>
#include<set>
#include<list>
#include<queue>
#include<unordered_map>
#include <unordered_set>
#include "numeric"
using namespace std;

class Graph{
  
public:
    int V;
    list<pair<int, int>> *l;
    vector<vector<int>> demandTable;
    
    Graph(int v){
        V=v;
        l=new list<pair<int,int>>[V];
        demandTable={{4,6,8},
                {6,7,9},
                {5,11,4},
                {9,10,4}
        };
        
    }
    
    void addEdgeUser(int i, int j, int wt){
        
        l[i].push_back({wt,j});
        l[j].push_back({wt,i});
    }
    
    void addEdgeUAV(int i,int j, int wt){
        l[i].push_back({wt,j});
    }
    
    int fulfillDemand(int s,int d, int demand){
        int demandFulf=checkDemand(s, d, demand);
        vector<int> pathFinder;
        getPath(s, d, pathFinder);
        list<pair<int,int>>::iterator i,k;
        int j=0;

        while(j<pathFinder.size()-1){
            i=l[pathFinder[j]].begin();
            j++;
            
            while((*i).second!=pathFinder[j])
                i++;
            
            (*i).first-=demandFulf;
            
            k=l[(*i).second].begin();
            while((*k).second!=pathFinder[j-1])
                k++;
            (*k).first-=demandFulf;
            
        }
        return demandFulf;
    }

    
    int checkDemand(int s, int d, int demand){
        vector<int> pathFinder;
        getPath(s, d, pathFinder);
        list<pair<int,int>>::iterator i;
        int j=0;

        while(j<pathFinder.size()-1){
            i=l[pathFinder[j]].begin();
            j++;
            
            while((*i).second!=pathFinder[j])
                i++;
                
            if((*i).first<demand){
                int a=(*i).first;
                while(a!=demand)
                    demand--;
            }
        }
        
        return demand;
    }
    
    void getPath(int s, int d, vector<int>& pathFinder){
        vector<bool> visited(V,false);
        int* path = new int[V];

        printAllPathsUtil(s, d,visited,path,0,pathFinder);
    }
    
    void printAllPathsUtil(int u, int d, vector<bool> visited, int path[], int path_index, vector<int>& pathFinder){
        visited[u] = true;
        path[path_index] = u;
        path_index++;
        
        if (u == d)
            for (int i = 0; i < path_index; i++)
                pathFinder.push_back(path[i]);
        
        else {
            list<pair<int,int>>::iterator i;
            for (i=l[u].begin();i!=l[u].end();i++)
                if (!visited[(*i).second])
                    printAllPathsUtil((*i).second, d, visited, path, path_index,pathFinder);
        }
        path_index--;
        visited[u] = false;
    }
};


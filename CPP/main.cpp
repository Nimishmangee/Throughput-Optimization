#include<iostream>
#include<vector>
#include<list>
#include <unordered_map>
#include "sys.hpp"
#include "graph.hpp"

using namespace std;

vector<vector<pair<int,int>>> uavToUav(4,vector<pair<int,int>>(4,{0,0}));
vector<vector<pair<int,int>>> uavToUser(4,vector<pair<int,int>>(8,{0,0}));

void displayGrid(vector<vector<string>> grid){
    for(auto g:grid){
        for(auto u:g)
            cout<<u<<" ";
        cout<<endl;
    }
}

void initialize(Graph &g){
    
    g.l=new list<pair<int,int>>[12];
    for(int i=0;i<uavToUav.size();i++){
        for(int j=0;j<uavToUav[i].size();j++){
            if(uavToUav[i][j].first==1)
                g.addEdgeUAV(i, j, uavToUav[i][j].second);
        }
        for(int j=0;j<uavToUser[i].size();j++){
            if(uavToUser[i][j].first==1)
                g.addEdgeUser(i, j+4, uavToUser[i][j].second);
        }
    }
}

int sumOfDemandFulfilled(Graph &g, System &ob){
    int ans=0;
    for(auto x:g.demandTable){
        int demandFulf=g.fulfillDemand(x[0], x[1], x[2]);
        ans+=demandFulf;
    }
    initialize(g);
    return ans;
}

int avThroughput(int uavSelected, System &ob, Graph &g ){
    int res=0;
    pair<int,int> currentPos=ob.uavs[uavSelected];
    
    for(auto cord:ob.permCords[uavSelected]){
        ob.uavs[uavSelected]=cord;
        res+=sumOfDemandFulfilled(g,ob);
    }
    
    ob.uavs[uavSelected]=currentPos;
    return res/ob.permCords[uavSelected].size();
}

int maxThroughput(System &ob, Graph &g){
    int maxi=0;
    for(auto demandPair:g.demandTable)
        maxi+=demandPair[2];

    return maxi;
}

float fitnessFunction(int currThroughput, int Tavg, int Tmax){
    if(currThroughput>=Tavg)
        return 0.5+0.5*pow(((currThroughput-Tavg)/(Tmax-Tavg)),2);
    
    return 0.5*pow((currThroughput/Tavg),2);
}

void checkFitness(int uavSelected, Graph &g, System &ob){
//    for(auto cord:ob.permCords[uavSelected]){
//        ob.uavs[uavSelected]=cord;
//        int avThr=avThroughput(uavSelected, ob, g);
//        int maxThr=maxThroughput(ob, g);
//        int fitness=fitnessFunction(sumOfDemandFulfilled(g, ob),avThr,maxThr);
//
//        initialize(g);
//        if(fitness>0.9)
//            break;
//
//        if(sumOfDemandFulfilled(g, ob)==19)
//            break;
//    }
    
    ob.uavs[uavSelected]={4,15};
    ob.initialize(4,8,uavToUav,uavToUser);
    initialize(g);
}

int main(){
    int numOfUav=4, i=0,a=0;
    int numOfUser=8;
    
    vector<vector<string>> grid(30,vector<string>(30,"."));
    
    System ob(numOfUav,numOfUser,uavToUav,uavToUser);
    Graph g(12);
    
    for(auto x:ob.users)
        grid[x.first][x.second]='*';
    
    for(auto x:ob.uavs)
        grid[x.first][x.second]="V"+to_string(i++);
    
    initialize(g);
    
    checkFitness(0, g, ob);

    for(auto x:g.demandTable){
        int demandFulf=g.fulfillDemand(x[0], x[1], x[2]);
        g.demandTable[a++].push_back(demandFulf);

    }
    
    for(auto x:g.demandTable)
        cout<<x[0]<<"->"<<x[1]<<" "<<x[2]<<" "<<x[3]<<endl;
    
}


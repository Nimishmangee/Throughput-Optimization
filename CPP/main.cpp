#include<iostream>
#include<vector>
#include<list>
#include <unordered_map>
#include<random>
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
        ob.initialize(4, 8, uavToUav, uavToUser);
        initialize(g);
        res+=sumOfDemandFulfilled(g,ob);
    }
    
    ob.uavs[uavSelected]=currentPos;
    ob.initialize(4, 8, uavToUav, uavToUser);
    initialize(g);

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

int countBits(u_long n){
    
    if(n==0)
        return 0;
    int i=31;
    int mask=1<<i;
    
    while(!(n&mask)){
        i--;
        mask=1<<i;
    }
    return i+1;
}

int getithBit(long n, int i){
    return (n&(1<<i)) >0 ? 1 : 0;
}

void mutation(int uavSelected, System &ob, Graph &g){
    
    srand( static_cast<unsigned int>(time(nullptr)));
    auto size=ob.permCords[uavSelected].size();
    int bits=countBits(size-1);
    int x=(rand()%(bits));
    long indexOfCurrCord = 0;
    pair<int,int> cord=ob.uavs[uavSelected];
    auto it=find(ob.permCords[uavSelected].begin(),ob.permCords[uavSelected].end(),cord);
    if(it!=ob.permCords[uavSelected].end())
        indexOfCurrCord=it-ob.permCords[uavSelected].begin();

    int bitToBeSet=!(getithBit(indexOfCurrCord, x));

    int mask = 1 << x;
    
    long cordf=((indexOfCurrCord & ~mask) | (bitToBeSet << x));
    
    if(cordf<size)
        ob.uavs[uavSelected]=(ob.permCords[uavSelected])[cordf];
    
    ob.initialize(4, 8, uavToUav, uavToUser);
    initialize(g);

}

void checkFitness(int uavSelected, Graph &g, System &ob){
    int minFitness=0.5;
    pair<int,int> p;
    for(auto cord:ob.permCords[uavSelected]){
        ob.uavs[uavSelected]=cord;
        ob.initialize(4, 8, uavToUav, uavToUser);
        initialize(g);

        int avThr=avThroughput(uavSelected, ob, g);
        int maxThr=maxThroughput(ob, g);
        int demandFulfilled=sumOfDemandFulfilled(g, ob);
        
        float fitness=fitnessFunction(demandFulfilled, avThr, maxThr);
        
        if(fitness>=minFitness){
            minFitness=fitness;
            p=cord;
//            break;
        }
    }
    ob.uavs[uavSelected]=p;
    ob.initialize(4, 8, uavToUav, uavToUser);
    initialize(g);
}

int main(){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
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
    
    set<pair<int,int>> checked;
    for(int p=0;p<g.demandTable.size();p++){
        auto x=g.demandTable[p];
        int demandFulf=g.fulfillDemand(x[0], x[1], x[2]);
        
        if(demandFulf<x[2] and checked.find({x[0],x[1]})==checked.end()){
            checked.insert({x[0],x[1]});
            vector<int> pathFinder;
            g.getPath(x[0], x[1], pathFinder);
            
            for(auto y:pathFinder){
                if(y<4){
                    checkFitness(y, g, ob);
                    if(dis(gen) < 0.1)
                        mutation(y, ob, g);
                    
                }
            }
            p=-1;
        }
    }
    
    ob.initialize(4, 8, uavToUav, uavToUser);
    initialize(g);

    
    for(auto x:g.demandTable){
        int demandFulf=g.fulfillDemand(x[0], x[1], x[2]);
        g.demandTable[a++].push_back(demandFulf);

    }
    for(auto x:g.demandTable)
        cout<<x[0]<<"->"<<x[1]<<" "<<x[2]<<" "<<x[3]<<endl;
    
}


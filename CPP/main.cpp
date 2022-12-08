#include<iostream>
#include<vector>
#include<list>
#include <unordered_map>
#include<random>
#include<fstream>
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

class genetic{
public:
    System *ob2;
    Graph *g2;
    genetic(System &ob, Graph &g){
        ob2=&ob;
        g2=&g;
    }
    
    void initialize(){
        
        g2->l=new list<pair<int,int>>[12];
        for(int i=0;i<uavToUav.size();i++){
            for(int j=0;j<uavToUav[i].size();j++){
                if(uavToUav[i][j].first==1)
                    g2->addEdgeUAV(i, j, uavToUav[i][j].second);
            }
            for(int j=0;j<uavToUser[i].size();j++){
                if(uavToUser[i][j].first==1)
                    g2->addEdgeUser(i, j+4, uavToUser[i][j].second);
            }
        }
    }

    int sumOfDemandFulfilled(){
        int ans=0;
        for(auto x:g2->demandTable){
            int demandFulf=g2->fulfillDemand(x[0], x[1], x[2]);
            ans+=demandFulf;
        }
        initialize();
        return ans;
    }

    int avThroughput(int uavSelected){
        int res=0;
        pair<int,int> currentPos=ob2->uavs[uavSelected];
        
        for(auto cord:ob2->permCords[uavSelected]){
            ob2->uavs[uavSelected]=cord;
            ob2->initialize(4, 8, uavToUav, uavToUser);
            initialize();
            res+=sumOfDemandFulfilled();
        }
        
        ob2->uavs[uavSelected]=currentPos;
        ob2->initialize(4, 8, uavToUav, uavToUser);
        initialize();

        return res/ob2->permCords[uavSelected].size();
    }

    int maxThroughput(){
        int maxi=0;
        for(auto demandPair:g2->demandTable)
            maxi+=demandPair[2];

        return maxi;
    }
    
    float fitnessFunction(int currThroughput, int Tavg, int Tmax){
        if(currThroughput>=Tavg)
            return 0.5+0.5*pow(((currThroughput-Tavg)/(Tmax-Tavg)),2);
        
        return 0.5*pow(((currThroughput-Tavg)/(Tmax-Tavg)),2);
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

    void mutation(int uavSelected){
        
        srand( static_cast<unsigned int>(time(nullptr)));
        auto size=ob2->permCords[uavSelected].size();
        int bits=countBits(size-1);
        int x=(rand()%(bits));
        long indexOfCurrCord = 0;
        pair<int,int> cord=ob2->uavs[uavSelected];
        auto it=find(ob2->permCords[uavSelected].begin(),ob2->permCords[uavSelected].end(),cord);
        if(it!=ob2->permCords[uavSelected].end())
            indexOfCurrCord=it-ob2->permCords[uavSelected].begin();

        int bitToBeSet=!(getithBit(indexOfCurrCord, x));

        int mask = 1 << x;
        
        long cordf=((indexOfCurrCord & ~mask) | (bitToBeSet << x));
        
        if(cordf<size)
            ob2->uavs[uavSelected]=(ob2->permCords[uavSelected])[cordf];
        
        ob2->initialize(4, 8, uavToUav, uavToUser);
        initialize();

    }
    
    bool checkObstacle(pair<int,int> initialCord, pair<int,int> currCord){
        
        //Straight line equation
        float x1=initialCord.first;
        float y1=initialCord.second;
        float x2=currCord.first;
        float y2=currCord.second;
        float m=(y2-y2)/(x2-x1);
        for(auto obstacle:ob2->obstacles){
            int x=obstacle.first;
            int y=obstacle.second;
            
            if((y-y1)-m*(x-x1)==0)
                return 1;
        }
        return 0;
    }
    
    void checkFitness(int uavSelected){
        int minFitness=0.5;
        pair<int,int> p;
        pair<int,int> initialCord=ob2->uavs[uavSelected];
        for(auto cord:ob2->permCords[uavSelected]){
            ob2->uavs[uavSelected]=cord;
            ob2->initialize(4, 8, uavToUav, uavToUser);
            initialize();

            int avThr=avThroughput(uavSelected);
            int maxThr=maxThroughput();
            int demandFulfilled=sumOfDemandFulfilled();
            
            float fitness=fitnessFunction(demandFulfilled, avThr, maxThr);
            
            if(checkObstacle(initialCord, cord))
                fitness-=0.1;
            
            if(fitness>=minFitness){
                minFitness=fitness;
                p=cord;
    //            break;
            }
        }
        ob2->uavs[uavSelected]=p;
        ob2->initialize(4, 8, uavToUav, uavToUser);
        initialize();
    }

};



int main(){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    int numOfUav=4, i=0,a=0;
    int numOfUser=8;
    

    vector<vector<string>> grid(30,vector<string>(30,"."));
    
    System ob(numOfUav,numOfUser,uavToUav,uavToUser);
    Graph g(12);
    genetic gt(ob,g);
    
    for(auto x:ob.users)
        grid[x.first][x.second]='*';
    
    for(auto x:ob.uavs)
        grid[x.first][x.second]="V"+to_string(i++);
    
    for(auto x:ob.obstacles)
        grid[x.first][x.second]='O';
    
    gt.initialize();
    
    //File Handling Initial coord
    fstream fout;

    fout.open("cord.txt",ios::app);

    if (fout){
        fout<<"Initial position"<<endl;
        for(int i=0;i<numOfUav;i++)
            fout<<i<<":"<<ob.uavs[i].first<<","<<ob.uavs[i].second<<endl;

        for(int i=0;i<numOfUser;i++)
            fout<<(i+4)<<":"<<ob.users[i].first<<","<<ob.users[i].second<<endl;

        cout<<endl;
        fout.close();
    }
    
    
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
                    gt.checkFitness(y);
                    if(dis(gen) < 0.1)
                        gt.mutation(y);
                    
                }
            }
            p=-1;
        }
    }
    
    ob.initialize(4, 8, uavToUav, uavToUser);
    gt.initialize();
    
    //Final coord
    fout.open("cord.txt",ios::app);
    if (fout){
        fout<<"Final position"<<endl;
        for(int i=0;i<numOfUav;i++)
            fout<<i<<":"<<ob.uavs[i].first<<","<<ob.uavs[i].second<<endl;

        for(int i=0;i<numOfUser;i++)
            fout<<(i+4)<<":"<<ob.users[i].first<<","<<ob.users[i].second<<endl;

        fout.close();
    }

    
    for(auto x:g.demandTable){
        int demandFulf=g.fulfillDemand(x[0], x[1], x[2]);
        g.demandTable[a++].push_back(demandFulf);

    }

    cout<<"Pair"<<" "<<"Demand"<<" Fulfilled"<<endl;
    for(auto x:g.demandTable)
        cout<<x[0]<<"->"<<x[1]<<"   "<<x[2]<<"    "<<x[3]<<endl;
    
}


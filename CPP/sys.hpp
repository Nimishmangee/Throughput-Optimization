#include<iostream>
#include<vector>
#include<climits>
#include<set>
#include<list>
#include<queue>
#include<deque>
#include <unordered_map>
#include<stack>


using namespace std;
class System
{
public:
    //Constants
    int numOfUav;
    int numOfUser;
    int scale=100;
    int Rv2v=1000;
    int Rv2c=400;
    vector<pair<int,int>> uavs={{2,15},{8,22},{15,15},{17,23}};
    vector<pair<int,int>> users={{2,18},{4,11},{10,25},{15,16},{17,14},{19,15},{21,23},{20,24}};
    vector<pair<int,int>> obstacles;
    vector<vector<pair<int,int>>> uavToUav;
    vector<vector<pair<int,int>>> uavToUser;

    unordered_map<int,vector<pair<int, pair<int,int>>>> m;  //Every UAV ke connections UAV/user
    unordered_map<int, vector<pair<int,int>>> permCords;    //Perm coords for every UAV
    
    
    double linkCapacity(double d, double Pt=0.001, double w=0.15, double T=300.24, double BW=3000000){
                        //*******Pt =3mW==0.003W, d(distance), w(wavelength)=0.1m*****
        
        double Pr = Pt * pow(w/(4*3.14*d), 2);  // Gt = Gr = 1 [Friis Transmission Equation]
        long double N = 1.38e-23 * T * BW;      // Thermal Noise = kTB (boltzmann's const, temp, bandwidth)
        double SNR = Pr/N;                      // In Shannon-Hartley Equation, SNR is expressed as a linear power ratio
        double linkCap = BW * log2(1+SNR);      // Shannon-Hartley Equation

        return linkCap/1000000;                 // Mbps
    }

    float distance(pair<int,int> p1, pair<int,int> p2){
        return sqrt(pow(p2.first-p1.first, 2) + pow(p2.second-p1.second, 2)) * scale;
    }
    
    bool checkInsideCircle(pair<int,int> center, pair<int,int> point, int r){
        return distance(center, point)<=r;
    }
    
    void helper(pair<int,int> point){
        int flag;
        for(auto &x:m){
            flag=1;
            for(auto ele:x.second){
                if(!checkInsideCircle(ele.second, point, ele.first)){
                    flag=0;
                    break;
                }
            }
            if(flag)
                permCords[x.first].push_back(point);
        }
    }
    
    void gridTraversal(){
        for(int i=0;i<30;i++){
            for(int j=0;j<30;j++){
                helper({i,j});
            }
        }
    }
    
    void printMatrix(vector<vector<pair<int,int>>> matrix){
        for(int i=0;i<matrix.size();i++){
            for(int j=0;j<matrix[i].size();j++){
                cout<<matrix[i][j].first<<","<<matrix[i][j].second<<"  ";
            }
            cout<<endl<<endl;
        }
    }
    
    int maxThroughput(){
        int res=INT_MIN;
        for(auto x:uavToUav){
            for(auto y:x)
                if(y.first==1)
                    res=max(res,y.second);
        }
        
        return res;
    }
    
    int avThroughput(){
        int res=0;
        for(auto x:uavToUav){
            for(auto y:x)
                if(y.first==1)
                    res+=y.second;
        }
        return res/4;
    }
    
    void initialize(int numberOfUav,int numberOfUser,vector<vector<pair<int,int>>> &uavToUav, vector<vector<pair<int,int>>> &uavToUser){
        uavToUav[0][1]={1,linkCapacity(distance(uavs[0], uavs[1]))};
        uavToUav[1][0]={1,linkCapacity(distance(uavs[1], uavs[0]))};
        uavToUav[1][2]={1,linkCapacity(distance(uavs[1], uavs[2]))};
        uavToUav[1][3]={1,linkCapacity(distance(uavs[1], uavs[3]))};
        uavToUav[2][1]={1,linkCapacity(distance(uavs[2], uavs[1]))};
        uavToUav[3][1]={1,linkCapacity(distance(uavs[3], uavs[1]))};
        
        //uav to user ki matrix bhari
        for(int j=0;j<users.size();j++){
            pair<int,int> mini={INT_MAX,-1};
            for(int i=0;i<uavs.size();i++){
                if(distance(uavs[i], users[j])<mini.first)
                    mini=make_pair(distance(uavs[i], users[j]),i);
            }
            uavToUser[mini.second][j]={1,linkCapacity(distance(uavs[mini.second], users[j]))};
        }
        
        uavToUav=uavToUav;
        uavToUser=uavToUser;
        
        //Placing random obstacles
//        srand(time(0));
        while(obstacles.size()<=20){
            int x=(rand()%30);
            int y=(rand()%30);
            
            obstacles.push_back({x,y});
        }
    }
    
    System(int numberOfUav,int numberOfUser,vector<vector<pair<int,int>>> &uavToUav, vector<vector<pair<int,int>>> &uavToUser){
        numOfUav = numberOfUav;
        numOfUser = numberOfUser;
        uavToUav=uavToUav;
        uavToUser=uavToUser;
        
        initialize(numOfUav, numOfUser, uavToUav, uavToUser);
        

        
        //map me coordinates bhare hai
        for(int i=0;i<uavToUav.size();i++){
            for(int j=0;j<uavToUav[i].size();j++){
                if(uavToUav[i][j].first==1)
                    m[i].push_back({Rv2v,uavs[j]});
            }

            for(int j=0;j<uavToUser[i].size();j++){
                if(uavToUser[i][j].first==1)
                    m[i].push_back({Rv2c,users[j]});
            }
        }
        
        gridTraversal();
        
                //Permissible coord output karan lai
                for(auto &x:permCords){
                    cout<<x.first<<":";
                    for(auto ele:x.second)
                        cout<<ele.first<<","<<ele.second<<" ";
                    cout<<endl;

                }
        
        
    }
};


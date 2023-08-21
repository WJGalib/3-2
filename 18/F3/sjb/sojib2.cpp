#include<bits/stdc++.h>
#include<cstdlib>
#include<ctime>
#include<fstream>
#include<algorithm>
#include "helper.h"

#define X_left 1
#define Y_right 2

int vertices , edges ; 
ofstream demo("demo2.txt") ; 
// Vector < edge , edge , edge > 
using namespace std ; 
vector<int>best_known{12078,12084,12077,0,0,0,0,0,0,0,627,621,645,3187,3169,3172,0,0,0,0,0,14123,14129,14131,0,0,0,0,0,0,0,1560,1537,1541,8000,7996,8009,0,0,0,0,0,7027,7022,7020,0,0,6000,6000,5988,0,0,0,0};
vector<vector<edge>>v; 
vector<vector<int>> adjacencyMatrix;
void print(string s)
{
   // cout<<s<<endl ; 
}
void vector_print(vector<int>prv)
{
    for(int i=0 ; i<prv.size() ; i++)
        print(to_string(prv[i]+1)+" ") ; 

}
void rcl_print(vector<int>prv)
{
        for(int i=0 ; i<prv.size() ; i++)
           cout<<prv[i]<<" " ; 
        cout<<endl ; 
}

void show_info(edge e)
{
    print(to_string(e.getSource().getid()+1)+" - "+to_string(e.getDestination().getid()+1)+" : "+to_string(e.getWeight())) ; 
}
void edgeVectorPrint(vector<vector<edge>>prv)
{
    for(int i=0 ; i<prv.size() ; i++)
    {
        for(int j=0 ; j<prv[i].size() ; j++)
            show_info(prv[i][j]) ; 
    }
       
}
bool BelongsToSide(const std::vector<vertex>& vec, int number) {
    for (vertex element : vec) {
        if (element.getid()== number) {
            return true; 
        }
    }
    return false; 
}
bool allowed(const vector<vertex>& left , const vector<vertex>& right , int num1 , int num2)
{
    if(BelongsToSide(left,num1) && BelongsToSide(right, num2)) return true ; 
    else if(BelongsToSide(left,num2) && BelongsToSide(right, num1)) return true ; 
    return false ; 
}

int calc_max(const vector<vertex>& left , const vector<vertex>& right)
{
    int weight =0 ; 
    for(int i=0  ; i<v.size() ; i++)
    {
        for(int j=0 ; j<v[i].size() ; j++)
        {
            if(allowed(left , right , v[i][j].getSource().getid() , v[i][j].getDestination().getid()))
                {
                   
                   show_info(v[i][j]);
                   weight+=v[i][j].getWeight() ; 
                }
        }
    }
    return weight  ; 
}
int delta2(int number,vector<vertex>side )
{
    int sum = 0 ; 
    for(int i=0; i<v[number].size();i++)
    {
        bool condition = false ; 
        if(v[number][i].getDestination().getid()==number)condition=BelongsToSide(side,v[number][i].getSource().getid()) ; 
        else if(v[number][i].getSource().getid()==number)condition=BelongsToSide(side,v[number][i].getDestination().getid()) ; 
        if(condition)
            sum+=v[number][i].getWeight() ; 
    }
    return sum ; 
}

int delta(int v, vector<vertex>vec){
        int sum=0;
        for(vertex u : vec){
            sum+=adjacencyMatrix[u.getid()][v];
        }
        return sum;
    }

int vect_min()
{
    int min_w = INT_MAX ; 
    for(int i = 0 ; i<v.size() ; i++)
    {
        for(int j=0 ; j<v[i].size() ; j++)
        {
            min_w = min(min_w,v[i][j].getWeight()) ; 
        }
    }
    return min_w ; 

}


int vect_max()
{
    int max_w = INT_MIN ; 
    for(int i = 0 ; i<v.size() ; i++)
    {
        for(int j=0 ; j<v[i].size() ; j++)
        {
             max_w = max(max_w,v[i][j].getWeight()) ; 
        }
    }
    return max_w ; 
}


int assigned = 0  ;

retval SEMI_GREEDY_APPROACH(double alpha)
{
    
    vector<vertex>nodes(vertices) ; 
    for(int i=0 ; i<vertices ; i++)
        nodes[i].setid(i) ; 
    
    int w_min = vect_min() ; 
    int w_max = vect_max() ; 

    int cut_off = w_min+alpha*(w_max - w_min) ; 
    demo<<cut_off<<endl ; 
    vector<edge>RCL ; 
     for(int i = 0 ; i<vertices ; i++)
    {
        for(int j = 0 ; j<v[i].size() ; j++)
        {
            if(v[i][j].getWeight()>=cut_off)
                RCL.push_back(v[i][j]) ; 
        }
    }

    edge selected ; 
    int random_number = rand() ; 
    int index = random_number%RCL.size() ; 
    selected = RCL[index]  ; 

    vector<vertex>left ; 
    vector<vertex>right ; 


    selected.getSource().setside(X_left) ; 
    selected.getDestination().setside(Y_right)  ; 

    //setting up in the base node class too . 
    for(int i=0 ; i<vertices ; i++)
    {
        if(nodes[i].getid()==selected.getSource().getid()) nodes[i].setside(X_left) ; 
        else if(nodes[i].getid()==selected.getDestination().getid()) nodes[i].setside(Y_right) ; 
    }

    left.push_back(selected.getSource()) ; 
    right.push_back(selected.getDestination())  ; 
    assigned+=2 ; 
    bool unfinished = true ; 
    while (unfinished)
    {
       vector<vertex>unassigned ; 
       for(int i=0 ; i<nodes.size() ; i++)
       {
            if(!nodes[i].istaken()) unassigned.push_back(nodes[i]) ; 
       }
       vector<int>xsigma(vertices,-1) ; 
       vector<int>ysigma(vertices,-1) ; 
       int max_xS = INT_MIN , min_xS=INT_MAX; 
       int max_yS = INT_MIN , min_yS=INT_MAX ; 

       for(vertex v : unassigned)
       {
        xsigma[v.getid()]=delta(v.getid(),right) ; 
        ysigma[v.getid()]=delta(v.getid(),left) ; 
        max_xS = max(max_xS,xsigma[v.getid()]) ; 
        max_yS = max(max_yS,ysigma[v.getid()]) ; 
        min_xS = min(min_xS,xsigma[v.getid()]) ; 
        min_yS = min(min_yS,ysigma[v.getid()]) ; 
        if(max_xS==INT_MIN)
        {
            demo<<"ok"<<endl ; 
            demo<<v.getid()<<"-------"<<xsigma[v.getid()]<<"  : "<<ysigma[v.getid()]<<endl ; 
        }
       }
       demo<<unassigned.size()<<endl ; 
       demo<<max_xS<<" "<<max_xS<<" "<<min_xS<<" "<<min_yS<<endl  ; 
       w_max = max(max_xS,max_xS) ; 
       w_min = min(min_xS,min_yS) ; 
       cut_off=w_min+alpha*(w_max-w_min) ;

       //demo<<w_max<<" "<<w_min<<endl  ; 
       //demo<<cut_off<<endl ; 
       
       vector<vertex>RCL_vertex ; 
       for(vertex v : unassigned)
       {
        if(max(xsigma[v.getid()] , ysigma[v.getid()]) >= cut_off)
            RCL_vertex.push_back(v) ; 
       }
       int v_rcl_random = rand() ; 
       if(RCL_vertex.size()==0)cout<<"baal kortesi ami"<<endl ; 
       int v_rcl_index = v_rcl_random%RCL_vertex.size() ; 
       vertex selected_vertex = RCL_vertex[v_rcl_index] ; 
       int choosen_side = -1 ; 
       if(xsigma[selected_vertex.getid()]>ysigma[selected_vertex.getid()])
        {
            selected_vertex.setside(X_left) ; 
            left.push_back(selected_vertex) ; 
            choosen_side=X_left  ; 
            assigned++ ; 
        }
        else 
        {
            selected_vertex.setside(Y_right) ; 
            right.push_back(selected_vertex) ; 
            choosen_side=Y_right  ; 
            assigned++ ; 
        }
        for(int i=0 ; i<vertices ; i++)
        {
            if(nodes[i].getid()==selected_vertex.getid())
                 nodes[i].setside(choosen_side) ; 
        }
        if(assigned==vertices)unfinished=false ; 

    }
    assigned=0 ; 
    int weight = calc_max(left,right) ; 
    



    retval r; 
    r.setleft(left) ; 
    r.setright(right);
    r.setmax(weight/2) ; 
  







    return r ; 
}







int local_count ;

retval LOCAL_SEARCH_ALGO(retval &r)
{
    vector<vertex> left = r.get_left(); 
    vector<vertex> right = r.get_right(); 
    int maxval = r.getmax() ; 


    bool change = true ; 
    local_count = 0 ; 
    while (change)
    {
        change = false ; 
        for(int i=0 ; i<left.size() ; i++)
        {
            if(delta(left[i].getid() , left) - delta(left[i].getid() , right)>0)
            {
                right.push_back(left[i]) ; 
                left.erase(left.begin()+i) ; 
                change=true ; 
                local_count++ ; 
                break; 
            }
        }
        if(change)continue;
        for(int i=0 ; i<right.size() ; i++)
        {
            if(delta(right[i].getid() , right) - delta(right[i].getid() , left)>0)
            {
                left.push_back(right[i]) ; 
                right.erase(right.begin()+i) ; 
                change=true ; 
                local_count++; 
                break;  
            }
        }
    }
    r.setleft(left) ; 
    r.setright(right) ; 
    r.setmax(calc_max(left,right)/2);
    return r ; 
    
}
double random_alpha()
{
    srand(static_cast<unsigned int>(time(0)));
    double random = (double)rand() / RAND_MAX; 
    return random;
}
 ofstream result("output2.csv") ; 


int GRASP(int max_iteration , int prob )
{
    double alpha = 0.0 ;
    int best_result=INT_MIN ; 
    int rand_best = 0 ; 
    int greedy_best = 0 ; 
    int semi_best = 0 ; 
    int local_best = 0 ; 
    int m_iter = max_iteration  ;
    int local_iter = 0 ; 
    while(max_iteration--)
    {
       retval r=SEMI_GREEDY_APPROACH(alpha) ; 
       rand_best += r.getmax() ; 

        cout<<"Rand_done"<<endl ; 
      

   
       

        alpha = random_alpha();
        r=SEMI_GREEDY_APPROACH(alpha) ; 
        semi_best += r.getmax() ; 

        cout<<"Semi_done"<<endl ; 

        alpha = 1.0 ;
        r=SEMI_GREEDY_APPROACH(alpha) ; 
        greedy_best += r.getmax() ; 
        cout<<"Greedy_done"<<endl ; 
       //r.show() ; 
       r=LOCAL_SEARCH_ALGO(r) ; 
       local_best+=r.getmax()  ; 
       local_iter+=local_count ; 
       cout<<"Local_done"<<endl ; 



       //r.show() ; 
       if(r.getmax()>best_result)best_result=r.getmax() ;
    }
    double l_c_best = local_best*1.0/m_iter ; 
    int l_c = l_c_best ; 
       double r_c_best = rand_best*1.0/m_iter ; 
    int r_c = r_c_best ; 
       double g_c_best = greedy_best*1.0/m_iter ; 
    int g_c = g_c_best ; 
        double s_c_best = semi_best*1.0/m_iter ; 
    int s_c = s_c_best ;
          double iter_best = local_iter*1.0/m_iter ; 
    int i_c = iter_best ;


    result<<to_string(r_c)<<"," ;
    result<<to_string(g_c)<<"," ;    
    result<<to_string(s_c)<<"," ;
    result<<to_string(i_c)<<"," ;
    result<<to_string(l_c)<<"," ;
    result<<to_string(m_iter)<<"," ; 
    result<<to_string(best_result)<<","; 
    if(best_known[prob]!=0)
    {
        result<<to_string(best_known[prob])<<","; 
        double accuracy = (double) best_result*1.0/best_known[prob] ; 
        result<<to_string( accuracy) ; 
    }
    else 
    {
        result<<"--"<<","; 
    
        result<<"--"; 
    }
    result<<endl ; 
    return best_result ; 
}
int main()
{
    int file_count = 49; 
   
    for(int j=41; j<=file_count ; j++)
    {
        result<<"g"+to_string(j)<<"," ;
        ifstream file("../set1/g"+to_string(j)+".rud") ; 


        file>>vertices ; 
        file>>edges ; 
        result<<to_string(vertices)<<","<<to_string(edges)<<"," ;
        v.clear() ; 
        v.resize(vertices);
        adjacencyMatrix.clear() ; 
        adjacencyMatrix.resize(vertices, vector<int>(vertices, 0));

        for(int i = 0 ; i<edges ; i++)
        {
            int a , b , c ;
            file>>a>>b>>c ; 
            vertex src , dest ; 


            dest.setid(b-1) ; 
            src.setid(a-1) ; 


            edge e1(dest,c) ;
            e1.setSource(src) ; 

            edge e2(src,c) ;
            e2.setSource(dest);

            v[a-1].push_back(e1) ;
            v[b-1].push_back(e2) ; 
            adjacencyMatrix[a-1][b-1] = c;
            adjacencyMatrix[b-1][a-1] = c;  
        }

        edgeVectorPrint(v) ; 
        int max_iteration = 100 ;  
       
    int max_cost = GRASP(max_iteration,j-1) ;
    cout<<max_cost<<endl ; 


    }
    
    
}

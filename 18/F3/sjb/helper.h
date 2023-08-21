

#include<bits/stdc++.h>
using namespace std;
class vertex 
{
int id ; 
int side ; 
public: 
vertex()
{
    id = -1 ; 
    side = -1 ; 

}
void setid(int id){this->id = id ; }
int getid(){return id ; }

void setside(int side){this->side = side ; }
int getside(){return side ; }

bool istaken()
{
    if(side==-1) return false ; 
    return true ; 
}



};


class retval
{
    vector<vertex> left ; 
    vector<vertex> right ;
    int max_value ; 
    public:
    retval()
    {

    } 
    void setleft(vector<vertex>argVector){left=argVector;}
    void setright(vector<vertex>argVector){right=argVector;}
    void setmax(int a){max_value=a;}
    vector<vertex> get_left()  {
        return left;
    }

    vector<vertex> get_right()  {
        return right;
    }
    int getmax()
    {
        return max_value;
    }
    void show()
    {
        cout<<"MAX VALUE : "<<max_value<<endl ; 
       // show_vec() ;  
        
    }
    void show_vec()
    {
            cout<<"LEFT : "<<endl ; 
        for(int i=0 ; i<left.size() ; i++)
            cout<<left[i].getid()+1<<" " ;
        cout<<endl ; 
        cout<<"RIGHT : "<<endl ; 
        for(int i=0 ; i<right.size() ; i++)
            cout<<right[i].getid()+1<<" " ;
        cout<<endl ; 
    }
};

class edge
{
    vertex source; 
    vertex destination ; 
    int weight ; 

    public:
    edge()
    {
        weight=INT_MIN ; 
    }
    edge(vertex b , int c)
    {
      
        destination = b ; 
        weight = c; 
 
    }

    vertex getSource() const {
        return source;
    }
    vertex getDestination() const {
        return destination;
    }
    int getWeight() const {
        return weight;
    }
    void setSource(vertex src) {
        source = src;
    }
    void setDestination(vertex dest) {
        destination = dest;
    }
    void setWeight(int w) {
        weight = w;
    }

};
#include<iostream>
#include<bits/stdc++.h>
#include<cmath>
#include<unordered_map>
#include<fstream>

using namespace std;



int main(){
    int count =0;

    ifstream fin("Error.txt");
    ifstream f1in("anslog.txt");
    string line1, line2;
    while(getline(fin,line1) and getline(f1in,line2)){
        if(line1 != line2){
            cout<<line1<<endl;
            cout<<line2<<endl;
            cout << count << endl;
            break;
        }
        count++;
    }
    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <map>
#include <vector>

using namespace std;

//realize four types of display mode using four different parameters
void display_hz(char mat[16][2], int italy = 0, int scale = 1, bool convert = false, bool mirror = false);

//find all possible related chinese charater due to the input pingying
void find_hz(string &input, map<string, string> &pingying, FILE *zkfile, FILE *zbfile);

//let the user to select display mode
void select_display_mode(char mat[16][2]);

int main()
{
    ifstream infile;
    FILE *zkfile, *zbfile;
    string temp;
    string input;
    char choice;
    map<string, string> pingying;
    infile.open("pingying");
    zkfile = fopen("Hzk16", "rb");
    zbfile = fopen("mhzk16", "rb");

    while(getline(infile, temp)){
        stringstream input(temp);
        string hz, py;
        input >> hz;
        hz.pop_back();
        input >> py;
        pingying.insert(pair<string, string>(py, hz));
    }
    while(true){
        cout << "English or pingying(E-English P-pingying)" << endl << ">>>";
        cin >> choice;
        if(choice == 'P' && choice == 'p'){
            cout << "Input pingying(input \"exit\" to quit)" << endl << ">>>";
            cin >> input;
            if(input == "exit")
                break;
            find_hz(input, pingying, zkfile, zbfile);
        }else{
            //The mode of english display directely check the charactor in the zbHzk
            unsigned char charactor;
            cout << "Input a charactor(input \"exit\" to quit)" << endl << ">>>";
            cin >> input;
            if(input == "exit")
                break;
            charactor = input[0];
            char mat[16][2];
            //The offset is charater * 32 because one charator has 32 byte
            unsigned long offset = charactor * 32;
            fseek(zbfile, offset, SEEK_SET);
            fread(mat, 32, 1, zbfile);
            select_display_mode(mat);
        }

    }
}

void find_hz(string &input, map<string, string> &pingying, FILE *zkfile, FILE *zbfile)
{
    map<string, string>::iterator it;
    vector<string> temp_result;
    size_t choice;
    size_t code;
    for(auto it : pingying){
        if(it.first.find(input) != string::npos)
            temp_result.push_back(it.second);
    }
    for(size_t i = 0; i < temp_result.size(); i++){
        cout << i << "." << temp_result[i];
        if((i + 1) % 10 == 0)
            cout << endl;
    }
    cout << endl << "Make your choice" << endl << ">>>";
    cin >> choice;

    cout << "Choose the encode you want to use" << endl;
    cout << "1. GB code" << endl;
    cout << "2. ZB code" << endl << ">>>";
    cin >> code;

    char mat[16][2];
    memset(mat, 0x00, sizeof(32));
    //use unsigned char type because the type has exactly 1 byte it won't take
    //unnessasary information
    unsigned char qh = temp_result[choice][0] - 0xa1;
    unsigned char wh = temp_result[choice][1] - 0xa1;
    //due to users selection find the HZ in different library
    if(code == 1){
        //in GB library
        unsigned long offset = (94 * qh + wh) * 32;
        fseek(zkfile, offset, SEEK_SET);
        fread(mat, 32, 1, zkfile);
    }else{
        //in ZB library
        unsigned long offset = (94 * qh + wh + 256) * 32;
        fseek(zbfile, offset, SEEK_SET);
        fread(mat, 32, 1, zbfile);
    }
    //as we have the matrix information give it to select function
    select_display_mode(mat);
    cout << "\n";
}

void display_hz(char mat[16][2], int italy, int scale, bool convert, bool mirror)
{
    string content;
    string space;
    //due to the scale mode user choose output different string
    for(int s = 0; s < scale; s++){
        content.append("**");
        space.append("  ");
    }
    //travel the matrix to out put the charater
    for(int j = 0; j < 16; j++){
        //the scale control
        for(int s = 0; s < scale; s++){
            //italy style control
            for(int p = 0; p < (16 - j) * italy; p++)
                cout << " ";
            //The charator
            for(int i = 0;i < 2; i++){
                for(int k = 0; k < 8; k++){
                    //due to different mode make different output

                    //convert
                    if(convert && !mirror && (mat[15 - j][i] & (0x80 >> k)))
                        cout << content;
                    //normal
                    else if(!convert && !mirror &&(mat[j][i] & (0x80 >> k)))
                        cout << content;
                    //mirror
                    else if(!convert && mirror && (mat[j][1 - i] & (0x80 >> (7 - k))))
                        cout << content;
                    //convert and mirror
                    else if(convert && mirror && (mat[15 - j][1 - i] & (0x80 >> (7 - k))))
                        cout << content;
                    //no content output space
                    else
                        cout << space;
                }
            }
            cout << endl;
        }
    }
}

void select_display_mode(char mat[16][2])
{
    char input;
    int italy = 0;
    int scale = 1;
    bool convert = false;
    bool mirror = false;
    cout << endl;
    //choose whether to be italy style
    cout << "italy style?(Y/n)" << endl << ">>>";
    cin >> input;
    if(input == 'y' || input == 'Y'){
        cout << "slop degree?(1-3 recommended)" << endl << ">>>";
        cin >> italy;
    }
    //choose whether to be scale
    cout << "Scale?(Y/n)" << endl << ">>>";
    cin >> input;
    if(input == 'y' || input == 'Y'){
        cout << "scale times?(1-3 recommended)" << endl << ">>>";
        cin >> scale;
    }
    //choose whether to scale
    cout << "Convert?(Y/n)" << endl << ">>>";
    cin >> input;
    if(input == 'y' || input == 'Y'){
        convert = true;
    }
    //choose whether to mirror
    cout << "Mirror?(Y/n)" << endl << ">>>";
    cin >> input;
    if(input == 'y' || input == 'Y'){
        mirror = true;
    }
    cout << endl;
    //due to the selection make display
    display_hz(mat, italy, scale, convert, mirror);
    cout << endl;
}

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <Vector>
#include <map>
#include<cctype>
#include <array>
#include <algorithm>
#include <unordered_set>

using namespace std;
string parsingtable[30][30];
int sizeVT, sizeVN;


vector<pair<string, string>> readText() {
    string grammar;
    ifstream file;
    vector<pair<string, string>> grammar_vector;

    file.open("grammar.txt");
    if (!file) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    while (getline(file, grammar)) {
        string LHS = grammar.substr(0, 1);
        string RHS = grammar.substr(3, grammar.length());
        if (islower(LHS[0]) || LHS[0] == '#') {
            cout << "wrong input";  exit(1);
        }

        grammar_vector.emplace_back(LHS, RHS);

    }

    file.close();
    return grammar_vector;
}
void printGrammar(vector<pair<string, string>> g)
{
    cout << "Grammar entered:" << endl;
    vector<pair<string, string>>::iterator i = g.begin();
    for (i = g.begin(); i != g.end(); i++)
        cout <<"\t\t"<< i->first << "->" << i->second << endl; ;
    cout << "\n\n\n";
}
string removeDuplicate(string s)
{
    string str = s;
    unordered_set<char> log;

    str.erase(std::remove_if(str.begin(), str.end(), [&](char const c) { return !(log.insert(c).second); }), str.end());
    return str;
}
map<string, string> findFirst(vector<pair<string, string>> g)
{
    map<string, string> first;
    std::map<string, string>::iterator it = first.begin();

    for (auto i = g.rbegin(); i != g.rend(); i++) {//for each grammar in the vector g
        int sections[100]; int section = 0;//array to keep track of the OR in the grammar
        string RHS = i->second;
        string LHS = i->first;
        for (int i = 0; i < RHS.length(); i++)//for each OR put the position in the section array
        {
            if (RHS[i] == '|') { sections[section] = i; section++; }
        }
        if (section == 0)//if no OR in the grammar
        {
            if (isupper(RHS[0]))//if its a Vn 
            {
                string first_collection = "";//collect all the first for the VN
                std::map<string, string>::iterator s = first.find(RHS.substr(0, 1));//search for the first of the VN
                if (s == first.end())
                {
                    cout << "wrong grammar";
                    exit(1);
                }
                if (s->second.find('#') == true)//rule of first number 5 if there is epsilon find the next first
                {
                    int j = 1;
                    while (true)//search for all the firsts
                    {
                        std::map<string, string>::iterator t = first.find(RHS.substr(j, j + 1));
                        if (t == first.end() || j > RHS.length())
                            break;
                        first_collection = first_collection + t->second;
                        j++;
                    }
                }
                first_collection = first_collection + s->second;
                first_collection = removeDuplicate(first_collection);
                first.insert(it, std::pair<string, string>(LHS, first_collection));
            }
            else
            {
                first.insert(it, std::pair<string, string>(LHS, RHS.substr(0, 1)));
            }
        }
        else {//if there is OR in the grammar

            sections[section] = RHS.length(); section++;
            string first_collection = "";
            int lastSection = 0;
            for (int i = 0; i < section; i++)//cut every string before the OR to get the first of each section
            {

                string x = RHS.substr(lastSection, sections[i]);
                lastSection = sections[i] + 1;
                if (isupper(x[0]))//if its a Vn
                {

                    std::map<string, string>::iterator s = first.find(x.substr(0, 1));
                    if (s == first.end())
                    {

                        cout << "wrong grammar";
                        exit(1);
                    }

                    if (s->second.find('#') == true)
                    {
                        int j = 1;
                        while (true)//search for all the firsts
                        {
                            std::map<string, string>::iterator t = first.find(x.substr(j, j + 1));
                            if (t == first.end() || j > x.length())
                                break;

                            first_collection = first_collection + t->second;
                            j++;
                        }
                    }
                    first_collection = first_collection + s->second;
                }
                else {

                    first_collection = first_collection + x.substr(0, 1);
                }
            }
            first_collection = removeDuplicate(first_collection);
            first.insert(it, std::pair<string, string>(LHS, first_collection));
        }

    }
    return first;
}
map<string, string> findFollow(vector<pair<string, string>> g, map<string, string> first)
{

    map<string, string> follow;
    std::map<string, string>::iterator it = first.begin();
    std::map<string, string>::iterator l = follow.begin();
    string follow_collection = "";
    string modify[100];
    int modify_counter = 0;
    for (auto i = g.begin(); i != g.end(); i++) //for each grammar in the vector g
    {

        string LHS = i->first, RHS = i->second;
        if (i == g.begin())
            follow.insert(l, std::pair<string, string>(i->first, "$"));//if its starting symbol then its follow is $

        for (int z = 0; z < RHS.length(); z++)
        {
            string follow_collection;
            if (RHS[z] == '|' || LHS == RHS.substr(z, z + 1) || RHS[z] == '#')continue;
            if (isupper(RHS[z]))//if its VT 
            {

                string Follow;
                Follow += RHS[z];
                if (RHS[z + 1] != '\0') //the first of the next is the follow
                {

                    string first_off;
                    first_off += RHS[z + 1];
                    if (isupper(RHS[z + 1]))//if the next is VN search for its first
                    {
                        std::map<string, string>::iterator x = first.find(first_off);

                        if (x == first.end())
                        {
                            cout << "wrong grammr1";
                            exit(1);
                        }
                        if (x->second.find('#') == true) {//if is the first of the next has an epsilon, the follow can reach the next of the next
                            int j = z + 2;
                            if (j >= RHS.length())//if the next is the last then we add the follow of the parent
                            {
                                std::map<string, string>::iterator w = follow.find(LHS);
                                if (w == follow.end())
                                {
                                    cout << "wrong grammar4";
                                    exit(1);
                                }
                                //to keep track of the changes that happen to follow of RHS then we must update follow
                                modify[modify_counter] = Follow + LHS; modify_counter++;
                                follow_collection += w->second;
                            }

                            for (j = z + 2; j < RHS.length(); j++)//for each first of the next 
                            {
                                if (LHS == RHS.substr(j, j + 1) || RHS[j] == '#')continue;
                                if (RHS[j] == '|' || RHS[j + 1] != '\0')//if the next is the last then we add the follow of the parent
                                {
                                    std::map<string, string>::iterator x = follow.find(LHS);
                                    if (x == follow.end())
                                    {
                                        cout << "wrong grammar5";
                                        exit(1);
                                    }
                                    //to keep track of the changes that happen to follow of RHS then we must update follow
                                    modify[modify_counter] = Follow + LHS; modify_counter++;
                                    follow_collection += x->second;
                                    break;
                                }
                                if (isupper(RHS[j]))//if the next is VN we get its first and if it has epsilon we contenue searching and adding firsts
                                {
                                    string first_off;
                                    first_off += RHS[j];
                                    std::map<string, string>::iterator r = first.find(first_off);
                                    follow_collection += r->second;

                                    if (r->second.find('#') == true && RHS[j + 1] == '\0') {
                                        std::map<string, string>::iterator n = follow.find(LHS);
                                        modify[modify_counter] = Follow + LHS; modify_counter++;
                                        follow_collection += n->second;

                                    }

                                    else break;
                                }
                                else//if the next is VT then add it
                                {
                                    follow_collection += RHS[j];
                                    break;
                                }


                            }
                        }
                        follow_collection += x->second;
                    }
                    else if (islower(RHS[z + 1]))//if the next is VT then its the follow
                        follow_collection += RHS.substr(z + 1, z + 1);
                    else if (RHS[z + 1] == '|')//it the next is OR contenue;
                        continue;
                    else//if its any char and not VN then add it
                        follow_collection += RHS[z + 1];


                }
                else {//if no next then the follow will be euql to its parent in the RHS

                    std::map<string, string>::iterator x = follow.find(LHS);
                    if (x == follow.end())
                    {
                        cout << "wrong grammar2";
                        exit(1);
                    }
                    //to keep track of the changes that happen to follow of RHS then we must update follow
                    modify[modify_counter] = Follow + LHS; modify_counter++;
                    follow_collection += x->second;

                }
                std::map<string, string>::iterator y = follow.find(Follow);
                if (y == follow.end())//if no follow for it insert
                {
                    follow.insert(l, std::pair<string, string>(Follow, follow_collection));
                }
                else//if it exist then add to it
                {
                    y->second = y->second + follow_collection;
                }


            }
        }

    }
    //some VN there follow are equl to its parent but sometimes the follow of thier parent is updated so we must update the follow of the VN
    for (int i = 0; i < modify_counter; i++)
    {
        string must_update, updated;
        must_update += modify[i][0];
        updated = modify[i][1];
        std::map<string, string>::iterator a = follow.find(updated);
        std::map<string, string>::iterator b = follow.find(must_update);
        b->second = b->second + a->second;
        b->second = removeDuplicate(b->second);//remove duplicates caused by the updating
    }
    //finally now we will  remove the epsilon since we are done finding follow and its incorrect to have it inside the follow
    map<string, string>::iterator i;
    for (i = follow.begin(); i != follow.end(); i++)
        i->second.erase(remove(i->second.begin(), i->second.end(), '#'), i->second.end()); //remove eplsion from string

    return follow;
}

void parsingTable(vector<pair<string, string>> g, map<string, string> first, map<string, string> follow)
{
    sizeVN = 1;//how many Vn to put in the parsing table    
    string VT;//VT values to put in the parsing table;
    vector<pair<string, string>>::iterator it = g.begin();
    for (it = g.begin(); it != g.end(); it++)//TO FILL THE Vns and take the VTs
    {
        VT += it->second;
        parsingtable[sizeVN][0] = it->first;
        sizeVN++;
    }
    VT.erase(remove(VT.begin(), VT.end(), '|'), VT.end());//remove ors
    VT.erase(remove(VT.begin(), VT.end(), '#'), VT.end());//remove epsiolon
    VT = removeDuplicate(VT);
    VT += '$';
    sizeVT = VT.length();
    int count = 1;
    for (int i = 1; i < sizeVT; i++)//to fill the  VTS
    {
        if (isupper(VT[i]) == true)
            continue;
        parsingtable[0][count++] = VT[i];
        
    }
    sizeVT = count;


    for (int i = 1; i < sizeVN; i++)//for each Vn = rows
    {
        map<string, string >::iterator f = first.find(parsingtable[i][0]);//get first
        if (f == first.end())
            cout << "first not found for" << parsingtable[i][0] << endl;
        string firstof = f->second;

        map<string, string >::iterator foll = follow.find(parsingtable[i][0]);//get follow
        if (foll == follow.end())
            cout << "follow not found for" << parsingtable[i][0] << endl;
        string followof = foll->second;
        if (followof == firstof)
        {
            cout << "left factoring not included" << endl;
            exit(1);
        }
        string grammarVT;//RHS of the grammar
        string grammarVn;//RHS of the grammar
        vector<pair<string, string>>::iterator it = g.begin();
        for (it = g.begin(); it != g.end(); it++)
        {
            if (it->first == parsingtable[i][0])//if i found grammar of the VN 
            {
                grammarVn = it->first;
                grammarVT = it->second;
                break;
            }
        }
        string partitions[50];//if the grammar has ors then we need to split the grammar for the filling of parsing table
        int part = 0;//partitions counter
        bool MTOS = false; //more than one section means we have OR in the grammar
        for (int m = 0; m < grammarVT.length(); m++)//for each element in the RHS of the grammar
        {
            if (grammarVT[m] == '|' || grammarVT[m] == '#')//if there is or then move one index in the partitions
            {
                MTOS = true;
                part++;
                continue;
            }
            partitions[part] += grammarVT[m];//fill the data in the array

        }
        for (int j = 1; j < sizeVT; j++)//for each VT= colmuns
        {
            if (firstof.find('#') != std::string::npos)//if there is an epsilon in the first then its goes to the RHS->epsilon
            {
                for (int k = 0; k < followof.length(); k++)//for each element in the follow put RHS->epsilon
                {
                    string temp;
                    temp += followof[k];
                    if (parsingtable[0][j] == temp)
                    {
                        parsingtable[i][j] = parsingtable[i][0] + "->#";
                        break;
                    }
                }
            }
            int finder = 0;//save the index 
            //if first !=follow then we put RHS->RHS in each element in the first with respect if there is OR then we check for them
            for (int a = 0; a <= part; a++)//for each partition
            {
                for (int x = 0; x < firstof.length(); x++) {//for each element in the first
                    finder = partitions[a].find(firstof[x]);
                    if (MTOS) {//if there is partitions = ORS
                        if (finder != std::string::npos && firstof[x] == parsingtable[0][j][0])
                        {
                            //if one of the first elements in the partition and and found that the first element equal the VT 
                            parsingtable[i][j] = grammarVn + "->" + partitions[a];
                            break;
                        }
                    }
                    else {//if there is no partitions
                        if (firstof[x] == parsingtable[0][j][0])//if the lement of the first matches the VT
                        {
                            parsingtable[i][j] = grammarVn + "->" + grammarVT;
                            break;
                        }
                    }
                }
                
            }

        }
    }
}
void reverseStr(string& str)
{
    int n = str.length();

    // Swap character starting from two
    // corners
    for (int i = 0; i < n / 2; i++)
        swap(str[i], str[n - i - 1]);
}

void StackBuffer()
{
    cout << "plz enter the language you want to test" << endl;
    string language;
    cin >> language;
    cout << "\n\n\n";
    string _Stack="$", _Buffer= language + "$", _Action= "push $ to stack";
    int step=1;
    if (language.empty()) { cout << "empty language"; exit(1); }
    cout << "\tstep\t\t" << "Stack\t\t" << "Buffer\t\t" << "Action" << endl;
    cout << "\t"<<step++<<"\t\t" << _Stack<<"\t\t" <<_Buffer<< "\t\t" <<"push $ to stack"<< endl;
    _Action = "push  " + parsingtable[1][0] + "  to stack";
    _Stack += parsingtable[1][0];
    cout << "\t" << step++ << "\t\t" << _Stack << "\t\t" << _Buffer << "\t\t" << _Action<< endl;
    bool accepted = true;
    bool found = false;
    //cout << _Stack.rbegin()[0] << endl;
    while (accepted)
    {
        
        for (int i = 1; i < sizeVN; i++)
        {
            
            if (_Stack[0] == '$' && _Buffer[0] == '$'&&_Stack.length()==1)//stack=buffer=$ accepted
            {
                cout << "ACCEPTED" << endl;
                cout << "would you like to try another language? Y | N" << endl;
                string userReply;
                cin >> userReply;
                if (userReply[0] == 'Y' || userReply[0] == 'y')
                    StackBuffer();

                exit(1);
            }
            if (_Stack.back() == _Buffer[0])//if last element in stack= first elemet in buffer we pop it
            {
                _Action = "pop ";
                _Action += _Stack.back();
                _Stack.pop_back();
                _Buffer = _Buffer.substr(1, _Buffer.length());
                cout << "\t" << step++ << "\t\t" << _Stack << "\t\t" << _Buffer << "\t\t" << _Action << endl;
            }
            else {
                if (_Stack.back() == parsingtable[i][0][0]) {//if last elelemnt in stack==the VN row
                    for (int j = 1; j < sizeVT; j++)
                    {
                        if (_Buffer[0] == parsingtable[0][j][0])//if first element in buffer == the VT column
                        {
                            found = true;
                            if (parsingtable[i][j].empty()) {
                                
                                cout << "syntax error"; exit(1);
                            }
                            string LHS = parsingtable[i][j].substr(0, 3);
                            string RHS = parsingtable[i][j].substr(3, parsingtable[i][j].size());
                            
                            if (RHS.length() == 1)//there are only 2 ways a RHS has one element that if it was epsilon or an VT that pops away
                            {
                                if (RHS[0] == '#') {
                                    _Action = "pop  " + LHS + RHS;
                                    _Stack.pop_back();
                                    cout << "\t" << step++ << "\t\t" << _Stack << "\t\t" << _Buffer << "\t\t" << _Action << endl;
                                   
                                }
                                else {
                                    if (RHS[0] != _Buffer[0]) { cout << "syntax error2"; exit(1); }
                                    _Action = "pop  " + LHS + RHS;
                                    _Stack.pop_back();
                                    _Buffer = _Buffer.substr(1, _Buffer.length());
                                    cout << "\t" << step++ << "\t\t" << _Stack << "\t\t" << _Buffer << "\t\t" << _Action << endl;
                                    
                                }
                                break;
                            }
                            else {//if the RHS does not have only one element we put the grammar on the stack and we contenue
                                _Action = LHS + RHS;
                                reverseStr(RHS);//reverse the string since we will put it into the stack
                                _Stack.pop_back();//pop LHS to insert RHS
                                _Stack += RHS;
                                cout << "\t" << step++ << "\t\t" << _Stack << "\t\t" << _Buffer << "\t\t" << _Action << endl;
                                break;
                            }

                        }
                    }
                    if (!found) { //if it didnt find the element then its rejected language
                        accepted = false; cout << "REJECTED" << endl; 
                       
                        cout << "would you like to try another language? Y | N" << endl;
                        string userReply;
                        cin >> userReply;
                        if (userReply[0] == 'Y' || userReply[0] == 'y')
                            StackBuffer();
                        break;
                        
                    }
                }
            }
        }
    }
}
void runProject() {

    vector<pair<string, string>> grammar_vector = readText();
    printGrammar(grammar_vector);
    cout << "------------------------------------------------------------------------------------------------------------------" << endl;
    map<string, string> first = findFirst(grammar_vector);
    map<string, string> follow = findFollow(grammar_vector, first);
    cout << "first & follow table:\n\n" << endl;
    cout << "\t\t" << "First" << "\t" << "Follow" << endl;
    for (auto i = first.begin(), j = follow.begin(); i != first.end(), j != follow.end(); i++, j++)//print first &follow
        cout <<"\t"<< i->first << "\t" << i->second  << "\t" << j->second  << endl;
    cout << "------------------------------------------------------------------------------------------------------------------" << endl;
    parsingTable(grammar_vector, first, follow);
  

    cout << "\n\n\t\t\t\t\t PARSING TABLE:\n\n" << endl;
    for (int i = 0; i < sizeVN; i++)
    {
        for (int j = 0; j < sizeVT; j++) {
            if (parsingtable[i][j].empty())cout << "empty" << "\t";
            else
                cout << parsingtable[i][j] << "\t";
        }
        cout << endl << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------" << endl;
    StackBuffer();
    cout << "------------------------------------------------------------------------------------------------------------------" << endl;
}
void main()
{
    runProject();
    
    
}
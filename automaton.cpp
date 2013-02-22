#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

void srInit(struct automaton &a);
void addChar(char c, struct automaton &a);

struct state {
    int len, link;
    map <char, int> segue;
};
typedef struct state state;

struct automaton {
    int last, size;
    vector <struct state> st;
};
typedef struct automaton automaton;

void stInit(automaton &a)
{
    a.last = a.size = 0;
    a.st[0].len = 0;
    a.st[0].link = -1;
    a.size++;
}

void addChar(char c, automaton &a)
{
    int cur = a.size++;
    int last = a.last;
    a.st[cur].len = a.st[last].len + 1;
    int p = last;
    for (; p != a.st[0].link && !a.st[p].segue.count(c); p = a.st[p].link)
	a.st[p].segue[c] = cur;
    if (p == -1)
	a.st[cur].link = 0;
    else {
	int old_segue = a.st[p].segue[c];
	if (a.st[p].len + 1 == a.st[old_segue].len)
	    a.st[cur].link = old_segue;
	else {
	    int clone = a.size ++;
	    a.st[clone].len = a.st[p].len + 1;
	    a.st[clone].segue = a.st[old_segue].segue;
	    a.st[clone].link = a.st[old_segue].link;
	    for (; p != a.st[0].link && a.st[p].segue[c] == old_segue; p = a.st[p].link)
		a.st[old_segue].link = a.st[cur].link = clone;
	}
    }
    a.last = cur;
}

bool ifSubStr(string s, automaton &a) 
{
    int i = 0;
    int cur = 0;
    bool b = true;
    while (b) {
	b = (a.st[cur].segue.count(s[i]) != 0);
	if (b) {
	    cur = a.st[cur].segue[s[i]];
	    i++;
	}
    }
    return (i == s.length());
}

void saveGraph(string filename, automaton &a)
{
    ofstream out;
    out.open(filename.c_str());
    out << "digraph A {" << endl;
    out << "rankdir=LR;" << endl;
    out << "rank=same;" << endl;
    out << "0 [shape=box];" << endl;
    string last = "0";
    for (int i = 1; i < a.size; i++) {
	map<char, int>::iterator it;
	for (it = a.st[i-1].segue.begin();
	     it != a.st[i-1].segue.end(); it ++) {
	    //out << "edge[constraint=true];" << endl;
	    out << last << " -> " << it->second << " [label=" << it->first << "];" << endl;
	}
	//out << "edge[constraint=false];" << endl;
	out << i << " -> " << a.st[i].link << " [style=dotted];" << endl;
	ostringstream s;
	s << it->second;
	last = s.str();
    }
    out << "}\n";
    out.close();
}

int main(int argc, char **argv)
{
    automaton a;
    a.st.resize(100);
    stInit(a);
    string s;
    cout << "Enter string: ";
    getline(cin, s);
    for (int i = 0; i < s.length(); i++)
	addChar(s[i], a);
    saveGraph("./graph.dot", a);
    return 0;
}


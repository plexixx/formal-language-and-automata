#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//转换函数δ
struct Delta
{
    string src;  //起始状态
    string edge; //接受字符
    string dst;  //接受状态
};

//NFA
class NFA
{
public:
    vector<string> NFA_Q;                   //状态集合
    vector<char> NFA_T;                     //输入字母表
    vector<Delta> NFA_Delta;                //转换函数表
    vector<char> NFA_q;                     //初始状态
    vector<string> NFA_F;                   //终止状态集
    bool if_has_epsilon = false;            //判断NFA是否含ε
    bool if_final(string str, int f) const; //判断是否为终止状态
    void input();
    void output();
};

//
bool check(string str, char ch)
{
    for (int i = 0; i < (int)str.length(); i++)
    {
        if (str[i] == ch)
            return true;
    }
    return false;
}

//计算闭包
string closure(string q, const NFA &NFA)
{
    for (int i = 0; i < (int)q.length(); i++)
    {
        for (int j = 0; j < (int)NFA.NFA_Delta.size(); j++)
        {
            //若当前状态[i]中有ε转换
            if (q[i] == NFA.NFA_Delta[j].src[0] && NFA.NFA_Delta[j].edge[0] == '&')
            {
                if (!check(q, NFA.NFA_Delta[j].dst[0]))
                {
                    //将其加入到当前状态中
                    q += NFA.NFA_Delta[j].dst[0];
                }
            }
        }
    }
    return q;
}

//计算从当前状态可达的其他子集状态
string move(string q, char ch, const NFA &NFA)
{
    string temp = "";
    for (int i = 0; i < (int)q.length(); i++)
    {
        for (int j = 0; j < (int)NFA.NFA_Delta.size(); j++)
        {
            if (q[i] == NFA.NFA_Delta[j].src[0] && NFA.NFA_Delta[j].edge[0] == ch)
            {
                if (!check(temp, NFA.NFA_Delta[j].dst[0]))
                {
                    temp += NFA.NFA_Delta[j].dst[0];
                }
            }
        }
    }
    return temp;
}

//检查当前状态是否在子集状态中
bool checkINrawDFA(vector<string> rawDFA, string u)
{
    for (int i = 0; i < (int)rawDFA.size(); i++)
    {
        if (rawDFA[i] == u)
            return true;
    }
    return false;
}

//返回未被标记的状态下标
int checkFlag(vector<bool> rawDFAflag)
{
    for (int i = 0; i < (int)rawDFAflag.size(); i++)
    {
        if (rawDFAflag[i] == false)
            return i;
    }
    return -1;
}

//输入，以*结尾
void NFA::input()
{
    int opt = 0;
    char ch;
    string str;
    Delta del;
    cout << "请输入您要进行的操作序号：" << endl;
    cout << "1.输入状态集合\n2.输入输入字母表\n3.输入转换函数表\n4.输入初始状态\n5.输入终止状态集\n6.结束输入" << endl;
    cin >> opt;
    while (1)
    {
        switch (opt)
        {
        case 1:
            cout << "请输入状态集合" << endl;
            while (cin >> str && str.compare("*"))
                NFA_Q.push_back(str);
            cout << "请继续操作" << endl;
            str = "";
            cin >> opt;
            break;
        case 2:
            cout << "请输入输入字母表" << endl;
            while (cin >> ch && ch != '*')
                NFA_T.push_back(ch);
            cout << "请继续操作" << endl;
            cin >> opt;
            ch = '\0';
            break;
        case 3:
            cout << "请输入转换函数表" << endl;
            while (cin >> del.src && del.src != "*" && cin >> del.edge >> del.dst)
            {
                NFA_Delta.push_back(del);
                if (!if_has_epsilon && del.edge == "&")
                    if_has_epsilon = true;
            }
            cout << "请继续操作" << endl;
            cin >> opt;
            break;
        case 4:
            cout << "请输入初始状态" << endl;
            while (cin >> ch && ch != '*')
                NFA_q.push_back(ch);
            cout << "请继续操作" << endl;
            ch = '\0';
            cin >> opt;
            break;
        case 5:
            cout << "请输入终止状态集" << endl;
            while (cin >> str && str != "*")
                NFA_F.push_back(str);
            cout << "请继续操作" << endl;
            str = "";
            cin >> opt;
            break;
        default:
            return;
            break;
        }
    }
}

//输出
void NFA::output()
{
    ofstream ofs;
    string str;
    cout << "请输入输出文件名（后缀为.dot）：";
    cin >> str;

    const char *pstr = str.data();
    while (strstr(pstr, ".dot") == NULL)
    {
        cout << "ERROR!" << endl
             << "请重新输入：";
        cin >> str;
        pstr = str.data();
    }

    ofs.open(str, ios::out);
    ofs << "digraph G{" << endl;

    //标记初始状态为菱形
    ofs << "\"";
    for (int i = 0; i < (int)NFA_q.size(); i++)
        ofs << NFA_q[i];
    ofs << "\"[shape=\"diamond\"];\n";
    //标记终止状态为矩形
    for (int i = 0; i < (int)NFA_F.size(); i++)
    {
        ofs << "\"" << NFA_F[i] << "\"";
        ofs << "[shape=\"box\"];\n";
    }

    for (int i = 0; i < (int)NFA_Delta.size(); i++)
    {
        if (NFA_Delta[i].src != "" && NFA_Delta[i].dst != "")
            ofs << "\"" << NFA_Delta[i].src << "\""
                << "->\"" << NFA_Delta[i].dst
                << "\"[label=\"{" << NFA_Delta[i].edge << "}\"];" << endl;
    }
    ofs << "}";
    return;
}

//标记终止状态
bool NFA::if_final(string str, int f) const
{
    const char *pstr = str.data();
    for (int i = 0; i < f; i++)
    {
        const char *pf = NFA_F[i].data();
        if (strstr(pstr, pf) != NULL)
            return true;
    }
    return false;
}

//规范状态名
string std_str(string str)
{
    int size = (int)str.size();
    if (size == 1)
        return str;
    for (int i = 1; i < size; i++)
        str.insert(2 * i - 1, 1, ',');
    return str;
}

//NFA -> DFA
NFA trans(const NFA &r_NFA)
{
    NFA DFA;
    string start(&r_NFA.NFA_q.front(), (int)r_NFA.NFA_q.size());
    vector<string> rawDFA;   //存入子集
    vector<bool> rawDFAflag; //存入子集是否完成的标志
    vector<string> rawDFA_F; //存入含终止状态的子集
    Delta del;
    int t = (int)r_NFA.NFA_T.size(); //字母表大小
    int f = (int)r_NFA.NFA_F.size(); //NFA中终止状态个数

    if (r_NFA.if_has_epsilon)
    {
        start = closure(start, r_NFA); //确定起始状态
    }
    sort(start.begin(), start.end());
    start = std_str(start);

    rawDFA.push_back(start);     //将起始状态加入到子集状态中
    rawDFAflag.push_back(false); //当前状态标记为false

    while (checkFlag(rawDFAflag) != -1) //子集状态中仍存在未被标记的状态
    {
        int m = checkFlag(rawDFAflag); //返回未被标记的状态下标
        rawDFAflag[m] = true;
        for (int i = 0; i < t; i++) //遍历字母表
        {
            string str = closure(move(rawDFA[m], r_NFA.NFA_T[i], r_NFA), r_NFA);
            sort(str.begin(), str.end());
            str = std_str(str);
            if (!checkINrawDFA(rawDFA, str)) //若当前状态不在子集状态中
            {
                rawDFA.push_back(str);       //将其加入子集状态中
                rawDFAflag.push_back(false); //并标记为false
            }
            if (r_NFA.if_final(str, f))
            {
                rawDFA_F.push_back(str);
            }
        }
    }

    DFA.NFA_Q = rawDFA;
    DFA.NFA_T = r_NFA.NFA_T;
    DFA.NFA_F = rawDFA_F;
    // for (int i = 0; i < (int)rawDFA.size(); i++)
    //     cout << DFA.NFA_F[i] << endl;
    DFA.NFA_q.assign(start.begin(), start.end());
    // for (int i = 0; i < (int)DFA.NFA_q.size(); i++)
    //     cout << DFA.NFA_q[i];

    for (int i = 0; i < (int)rawDFA.size(); i++)
    {
        for (int j = 0; j < t; j++)
        {
            del.src = rawDFA[i];
            del.edge = r_NFA.NFA_T[j];
            del.dst = closure(move(rawDFA[i], del.edge[0], r_NFA), r_NFA);
            sort(del.dst.begin(), del.dst.end());
            del.dst = std_str(del.dst);
            DFA.NFA_Delta.push_back(del);
            // cout << del.src << "\t" << del.edge << "\t" << del.dst << endl;
        }
    }
    return DFA;
}

int main()
{
    NFA *r_NFA = new NFA();
    r_NFA->input();
    NFA r_DFA = trans(*r_NFA);
    r_DFA.output();

    delete r_NFA;
    r_NFA = NULL;

    system("pause");
    return 0;
}

/*
1
0 1 2 3 4 5 6 7 *
2
a b *
3
0 & 1
1 a 1
1 b 1
1 & 2
2 a 3
2 b 4
3 a 5
4 b 5
5 & 6
6 a 6
6 b 6
6 & 7
*
4
0 *
5
5 7 *
6
*/

/*
3
0 & 1
1 a 2
1 a 4
1 b 3
2 a 5
4 b 5
3 a 5
*
1
0 1 2 3 4 5 *
2
a b *
4
0 *
5
3 5 *
6
*/
#include "helpers.h"

vector<int> helpers::parse_integers(const char* text)
{
    auto r = vector<int>();

    int n = 0;
    bool neg = false;
    for (auto p = text; *p != '\0'; ++p)
    {
        auto c = *p;
        if (c >= '0' && c <= '9')
        {
            n = n * 10 + (c - '0');
        }
        else if (c == '-')
        {
            neg = true;
        }
        else if (c == ',')
        {
            r.push_back(neg ? -n : n);
            n = 0;
            neg = false;
        }
        else if (c == ']')
        {
            r.push_back(neg ? -n : n);
            break;
        }
    }

    return r;
}

vector<string> helpers::parse_strings(const char* text)
{
    auto r = vector<string>();

    string n;
    bool a = false;
    for (auto p = text; *p != '\0'; ++p)
    {
        auto c = *p;
        if (c == '"')
        {
            if (a)
            {
                r.push_back(n);
                n.clear();
            }
            a = !a;
            continue;
        }
        if (a)
            n.push_back(c);
    }

    return r;
}

vector<TreeNode> helpers::parse_binary_tree(const char* text)
{
    vector<TreeNode> r;

    auto vs = helpers::parse_integers(text);

    vector<int> ns;
    bool n = false;
    for (auto p = text; *p != '\0'; ++p)
    {
        auto c = *p;
        if (c == ',' || c == ']')
        {
            ns.push_back(n);
            n = false;
        }
        else if (c == 'n')
        {
            n = true;
        }
    }
    
    if (ns.size() != vs.size() || ns.size() == 0)
        return r;

    for (int i = 0; i < ns.size(); ++i)
        if (!ns[i])
            r.push_back(TreeNode(vs[i]));

    {
        queue<int> q;
        q.push(0);
        int i = 1;
        int ri = 1;

        while (!q.empty() && i < ns.size())
        {
            int c = q.front();
            q.pop();
            auto& n = r[c];

            if (i < ns.size())
            {
                if (!ns[i])
                {
                    q.push(ri);
                    n.left = &r[ri];
                    ++ri;
                }
                ++i;
            }

            if (i < ns.size())
            {
                if (!ns[i])
                {
                    q.push(ri);
                    n.right = &r[ri];
                    ++ri;
                }
                ++i;
            }
        }
    }

    return r;
}

vector<vector<int>> helpers::parse_matrix(const char* text)
{
    auto r = vector<vector<int>>();

    for (auto p = text + 1; *p != 0; ++p)
        if (*p == '[')
            r.push_back(parse_integers(p));

    return r;
}

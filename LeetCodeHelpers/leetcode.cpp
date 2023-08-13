#include "leetcode.h"
#include <stdexcept>
#include <functional>

// MiniParser for parsing functions

class MiniParser
{
private:
    const char* mPtr;
public:
    MiniParser(const char* text) : mPtr(text)
    {
    }
    MiniParser(const MiniParser&) = delete;
public:
    bool EndOfFile() const
    {
        return *mPtr == '\0';
    }
    char PeekChar() const
    {
        return *mPtr;
    }
    void MoveForward(int steps = 1)
    {
        mPtr += steps;
    }
    char ReadChar()
    {
        return *mPtr++;
    }
    void SkipWhitespaces()
    {
        while (!this->EndOfFile())
        {
            auto ch = this->PeekChar();
            if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\n')
            {
                this->MoveForward();
                continue;
            }
            else
            {
                break;
            }
        }
    }
    bool Expect(char ch)
    {
        if (!this->EndOfFile() && this->PeekChar() == ch)
        {
            this->MoveForward();
            return true;
        }
        return false;
    }

    int ReadInt()
    {
        int res = 0;
        bool neg = false;

        if (!this->EndOfFile() && this->PeekChar() == '-')
        {
            neg = true;
            this->MoveForward();
        }

        while (!this->EndOfFile())
        {
            auto ch = this->PeekChar();
            if (ch >= '0' && ch <= '9')
            {
                res = res * 10 + (ch - '0');
                this->MoveForward();
            }
            else
            {
                break;
            }
        }

        return neg ? -res : res;
    }
    std::string ReadString()
    {
        std::string res;

        if (!this->Expect('"'))
            throw std::runtime_error(__func__);

        while (!this->EndOfFile() && this->PeekChar() != '"')
            res.push_back(this->ReadChar());

        if (!this->Expect('"'))
            throw std::runtime_error(__func__);

        return res;
    }
    template <class T>
    std::vector<T> ReadSome(std::function<T(MiniParser*)> readFn)
    {
        std::vector<T> res;

        this->SkipWhitespaces();
        if (!this->Expect('['))
            throw std::runtime_error(__func__);

        while (!this->EndOfFile())
        {
            this->SkipWhitespaces();
            res.push_back(readFn(this));
            this->SkipWhitespaces();

            if (!this->Expect(','))
                break;
        }

        this->SkipWhitespaces();
        if (!this->Expect(']'))
            throw std::runtime_error(__func__);

        return res;
    }
};

std::vector<int> parse_integers(const char* text)
{
    auto parser = MiniParser(text);
    return parser.ReadSome<int>(&MiniParser::ReadInt);
}

std::vector<std::string> parse_strings(const char* text)
{
    auto parser = MiniParser(text);
    return parser.ReadSome<std::string>(&MiniParser::ReadString);
}

std::vector<std::vector<int>> parse_matrix(const char* text)
{
    auto parser = MiniParser(text);
    auto readFn = [](MiniParser* parser) {
        std::vector<int> res;
        return parser->ReadSome<int>(&MiniParser::ReadInt);
    };
    return parser.ReadSome<std::vector<int>>(readFn);
}

std::vector<TreeNode> parse_binary_tree(const char* text)
{
    typedef std::pair<bool, int> TreeElem;
    auto readFn = [](MiniParser* parser) {
        if (parser->Expect('n'))
        {
            parser->MoveForward(3);
            return std::make_pair(false, 0);
        }
        else
        {
            return std::make_pair(true, parser->ReadInt());
        }
    };

    std::vector<TreeNode> res;
    auto parser = MiniParser(text);
    auto treeElems = parser.ReadSome<TreeElem>(readFn);

    if (!treeElems.empty())
    {
        for (auto& elem : treeElems)
            if (elem.first)
                res.emplace_back(elem.second);

        decltype(res.size()) parentPos = 0, childPos = 1;
        auto childIter = treeElems.begin() + 1;
        while (childIter != treeElems.end())
        {
            if (childIter->first)
                res[parentPos].left = &res[childPos++];
            ++childIter;
            if (childIter != treeElems.end() && childIter->first)
                res[parentPos].right = &res[childPos++];
            if (childIter != treeElems.end())
                ++childIter;

            ++parentPos;
        }
    }

    return res;
}

std::vector<ListNode> parse_linked_list(const char* text)
{
    std::vector<ListNode> res;
    auto nums = parse_integers(text);
    for (auto num : nums)
        res.emplace_back(num);
    for (decltype(nums.size()) i = 1; i < nums.size(); ++i)
        res[i - 1].next = &res[i];
    return res;
}

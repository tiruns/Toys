#pragma once

#include <format>
#include <string>
#include <vector>

// Structures

struct ListNode
{
public:
    int val;
    ListNode* next;
public:
    ListNode() : val(0), next(nullptr)
    {
    }

    ListNode(int x) : val(x), next(nullptr)
    {
    }

    ListNode(int x, ListNode* next) : val(x), next(next)
    {
    }
};

struct TreeNode
{
public:
    int val;
    TreeNode* left;
    TreeNode* right;
public:
    TreeNode() : val(0), left(nullptr), right(nullptr)
    {
    }

    TreeNode(int x) : val(x), left(nullptr), right(nullptr)
    {
    }

    TreeNode(int x, TreeNode* left, TreeNode* right)
        : val(x), left(left), right(right)
    {
    }
};

// Parsing Functions

std::vector<int> parse_integers(const char* text);

std::vector<std::string> parse_strings(const char* text);

std::vector<std::vector<int>> parse_matrix(const char* text);

std::vector<TreeNode> parse_binary_tree(const char* text);

std::vector<ListNode> parse_linked_list(const char* text);

// Formatters

namespace details
{
    template <class T, class CharT>
    class trivial_fomatter
    {
    public:
        using parse_contex_type = std::basic_format_parse_context<CharT>;
    public:
        auto parse(const parse_contex_type& context)
        {
            return context.end();
        }
    };
}

template <class CharT>
class std::formatter<const ListNode*, CharT>
    : public details::trivial_fomatter<const ListNode*, CharT>
{
public:
    template <class FormatContext>
    auto format(const ListNode* node, FormatContext& context)
    {
        auto out = context.out();
        while (node)
        {
            out = std::format_to(out, "{}", node->val);
            node = node->next;
            if (node)
                out = std::format_to(out, " -> ");
        }
        return out;
    }
};

template <class CharT>
class std::formatter<ListNode*, CharT>
    : public std::formatter<const ListNode*, CharT>
{
};

template <class CharT>
class std::formatter<ListNode, CharT>
    : public details::trivial_fomatter<ListNode, CharT>
{
public:
    template <class FormatContext>
    auto format(const ListNode& node, FormatContext& context)
    {
        return std::format_to(context.out(), "{}", &node);
    }
};

template <class CharT>
class std::formatter<const TreeNode*, CharT>
    : public details::trivial_fomatter<const TreeNode*, CharT>
{
public:
    auto format_node(const TreeNode* node, auto it) -> decltype(it)
    {
        if (node)
        {
            it = std::format_to(it, "{}", node->val);
            if (node->left || node->right)
            {
                it = std::format_to(it, " (");
                it = format_node(node->left, it);
                it = std::format_to(it, ", ");
                it = format_node(node->right, it);
                it = std::format_to(it, ")");
            }
        }
        else
        {
            it = std::format_to(it, "null");
        }
        return it;
    }

    template <class FormatContext>
    auto format(const TreeNode* node, FormatContext& context)
    {
        // TODO: format("{} ({}, {})", node->val, node->left, node->right); ?
        return this->format_node(node, context.out());
    }
};

template <class CharT>
class std::formatter<TreeNode*, CharT>
    : public std::formatter<const TreeNode*, CharT>
{
};

template <class CharT>
class std::formatter<TreeNode, CharT>
    : public details::trivial_fomatter<const TreeNode, CharT>
{
public:
    template <class FormatContext>
    auto format(const TreeNode& node, FormatContext& context)
    {
        return std::format_to(context.out(), "{}", &node);
    }
};

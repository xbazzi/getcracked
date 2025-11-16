/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
struct ListNode
{
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

#include <cstdint>

class Solution {
   public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if (!head->next) return nullptr;
        ListNode* first  = head;
        ListNode* second = head;
        std::size_t count{ 0 };

        while (count < static_cast<std::size_t>(n) && first != nullptr)
        {
            first = first->next;
            ++count;
        }

        // If first is nullptr, we need to remove the head node
        if (first == nullptr)
        {
            ListNode* newHead = head->next;
            delete head;
            return newHead;
        }

        while (first->next != nullptr)
        {
            first  = first->next;
            second = second->next;
        }

        // Now second points to the node before the one we want to delete
        ListNode* toDelete = second->next;
        second->next       = second->next->next;
        delete toDelete;

        return head;
    }
};

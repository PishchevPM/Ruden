#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>

namespace nsp
{
    /**
     * @brief Stack with priorities
     * @tparam KeyT type of priority, Compare - comparator for them, ValueT - Value type
     */
    template <typename ValueT, typename KeyT, class Compare = std::less<KeyT>>
    class MyStack final
    {
    private:
        Compare cmp;

        struct InterNode
        {
            ValueT value_;
            struct InterNode *left_ = nullptr, *right_ = nullptr;
        };

        struct PriorNode
        { 
            KeyT key_;
            struct PriorNode *left_ = nullptr, *right_ = nullptr;
            struct InterNode *beg_ = nullptr, *end_ = nullptr;
            unsigned long size_;
        };

    public:
        /**
         * @brief Iterator for tree
         */
        class Iterator final
        {
        private:
            InterNode* node_ = nullptr;
            PriorNode* prnode_ = nullptr;
        public:
            Iterator(InterNode* node, PriorNode* prnode) noexcept : node_(node), prnode_(prnode) {}; 

            /**
             * @brief get element index
             * @return index of element 
             */

            bool operator== (const Iterator& it) const noexcept
            {
                return node_ == it.node_;
            }

            bool operator!= (const Iterator& it) const noexcept
            {
                return node_ != it.node_;
            }

            Iterator& operator++() noexcept
            {
                if (node_->right_ != nullptr)
                    node_ = node_->right_;
                else if (prnode_->right_ != nullptr)
                {
                    prnode_ = prnode_->right_;
                    node_ = prnode_->beg_;
                }
                else
                {
                    node_ = nullptr;
                    prnode_ = nullptr;
                }

                return *this;
            }

            Iterator& operator--() noexcept
            {
                if (node_->left_ != nullptr)
                    node_ = node_->left_;
                else if (prnode_->left_ != nullptr)
                    prnode_ = prnode_->left_;
                else
                {
                    node_ = nullptr;
                    prnode_ = nullptr;
                }

                return *this;
            }

            KeyT get_priority () const noexcept
            {
                return prnode_->key_;
            }

            ValueT& operator* () noexcept
            {
                return node_->value_;
            }

            ValueT* operator-> () noexcept
            {
                return node_;
            }
        };

    private:
        PriorNode* top_ = nullptr;
        PriorNode* back_ = nullptr;
        unsigned long long total_size_ = 0;
        unsigned long long priors_size_ = 0;
        
        MyStack (PriorNode* top) noexcept : top_(top) {};

    public:
        MyStack () {};
        /**
         * @brief Copy constructor
         */
        MyStack (const MyStack& tr)
        {
            if (tr.top_ == nullptr)
            {
                top_ = nullptr;
                total_size_ = 0;
                priors_size_ = 0;
            }

            total_size_ = tr.total_size_;
            priors_size_ = tr.priors_size_;
            top_ = new PriorNode();
            PriorNode* nd1 = top_;
            PriorNode* nd2 = tr.top_;
            while (true)
            {
                (*nd1) = (*nd2);
                if (nd2->right_ != nullptr)
                {
                    nd1->right_ = new PriorNode();
                    nd1->right_->left_ = nd1;
                }
                else
                {
                    nd1->right_ = nullptr;
                    back_ = nd1;
                    break;
                }
                nd1->beg_ = new InterNode();
                InterNode* ndi1 = nd1->beg_;
                InterNode* ndi2 = nd2->beg_;
                while (true)
                {
                    (*ndi1) = (*ndi2);
                    if (ndi2->right_ != nullptr)
                    {
                        ndi1->right_ = new InterNode();
                        ndi1->right_->left_ = ndi1;
                    }
                    else
                    {
                        ndi1->right_ = nullptr;
                        nd1->end_ = ndi1;
                        break;
                    }
                    ndi1 = ndi1->right_;
                    ndi2 = ndi2->left_;
                }
                nd1 = nd1->right_;
                nd2 = nd2->right_;
            }
        }

        /**
         * @brief Move constructor
         */
        MyStack (MyStack&& tr)
        {
            std::swap (back_, tr.back_);
            std::swap (top_, tr.top_);
            priors_size_ = tr.priors_size_;
            total_size_ = tr.total_size_;
        }

        /**
         * @brief Assign operator
         */
        MyStack& operator= (const MyStack& tr)
        {
            if (&tr == this)
                return *this;
            MyStack* temp = new MyStack (tr);

            std::swap (temp->top_, top_);
            std::swap (temp->back_, back_);
            std::swap (temp->priors_size_, priors_size_);
            std::swap (temp->total_size_, total_size_);
            delete temp;
        }

        MyStack& operator= (MyStack&& tr)
        {
            std::swap (top_, tr.top_);
            std::swap (back_, back_.top_);
            priors_size_ = tr.priors_size_;
            total_size_ = tr.total_size_;
        }

        /**
         * @brief Destructor
         */
        ~MyStack ()
        {
            while (top_ != nullptr)
            {
                InterNode* nd = top_->beg_;
                while (nd != nullptr)
                {
                    InterNode* tmpn = nd->right_;
                    delete nd;
                    nd = tmpn;
                }
                PriorNode* tmpn = top_->right_;
                delete top_;
                top_ = tmpn;
            }
        }

        /**
         * @brief iterator to element after last
         */
        Iterator end () const noexcept
        {
            return Iterator(nullptr, nullptr);
        }


        /**
         * @brief print total count of elements
         */
        unsigned long long total_size() const noexcept
        {
            return total_size_;
        }

        /**
         * @brief print count of priorities
         */
        unsigned long long priors_size() const noexcept
        {
            return priors_size_;
        }

        /**
         * @brief find iterator to elem by its index number
         * @param k index number
         * @return Iterator to elem or end (), if not founded
         */
        Iterator find_pos (unsigned long long k) noexcept
        {
            unsigned long long sum = 0;
            PriorNode* pr = top_;

            while (pr != nullptr)
            {
                if (sum + pr->size_ > k)
                {
                    InterNode* tmp = pr->beg_;
                    for (unsigned long long i = 1; i < k - sum + 1; i++)
                        tmp = tmp->right_;
                    return Iterator (tmp, pr);
                }
                sum += pr->size_;
                pr = pr->right_;
            }

            return end();
        }

        Iterator operator[] (unsigned long long k) noexcept
        {
            return find_pos (k);
        }

        /**
         * @brief push value
         * @param value value to push
         * @param priority element priority
         */
        void push_back (ValueT value, KeyT priority)
        {
            total_size_++;
            PriorNode* nd = top_;
            PriorNode* pr = nullptr;
            while (nd != nullptr && cmp (nd->key_, priority))
            {
                pr = nd;
                nd = nd->right_;
            }

            if (top_ == nullptr)
            {
                priors_size_++;
            }
            else if (nd == nullptr)
            {
                pr->right_ = new PriorNode ();
                pr->right_->left_ = pr;
                pr->right_->right_ = nullptr;
                pr->right_->key_ = priority;
                pr->right_->size_ = 0;
                pr = pr->right_;
                priors_size_++;
                back_ = pr;
            }
            else if (cmp (priority, nd->key_))
            {
                nd->left_ = new PriorNode ();
                nd->left_->right_ = nd;
                nd->left_->left_ = pr;
                nd->left_->key_ = priority;
                nd->left_->size_ = 0;
                if (pr != nullptr)
                    pr->right_ = nd->left_;
                else
                    top_ = nd->left_;
                pr = nd->left_;
                priors_size_++;
            }
            else
            {
                pr = nd;
            }

            if (pr == nullptr)
            {
                top_ = new PriorNode ();
                top_->key_ = priority;
                top_->size_ = 0;
                pr = top_;
                back_ = top_;
            }
            if (pr->beg_ == nullptr)
            {
                pr->beg_ = new InterNode ();
                pr->beg_->value_ = value;
                pr->end_ = pr->beg_;
                pr->size_ = 0;
            }
            else
            {
                pr->end_->right_ = new InterNode ();
                pr->end_->right_->left_ = pr->end_;
                pr->end_ = pr->end_->right_;
                pr->end_->value_ = value;
            }

            pr->size_++;
        };

        /**
         * @brief delete last element with maximal priority
         */
        void pop_back ()
        {
            if (back_ == nullptr)
                return;
            if (back_->end_->left_ == nullptr)
            {
                delete back_->beg_;
                if (back_->left_ != nullptr)
                {
                    auto tmp = back_->left_;
                    delete tmp->right_;
                    tmp->right_ = nullptr;
                    priors_size_--;
                    total_size_--;
                    back_ = tmp;
                }
                else
                {
                    delete top_;
                    top_ = nullptr;
                    back_ = nullptr;
                    priors_size_ = 0;
                    total_size_ = 0;
                }
            }
            else
            {
                auto tmp = back_->end_->left_;
                delete tmp->right_;
                tmp->right_ = nullptr;
                back_->end_ = tmp;
                back_->size_--;
                total_size_--;
            }

        }
    };

    template <typename ValueT, typename KeyT, class Compare = std::less<KeyT>>
    std::ostream& operator<< (std::ostream& stream, MyStack<ValueT, KeyT, Compare>& stack)
    {
        auto it = stack.find_pos (0);
        while (it != stack.end ())
        {
            stream << "Priority " << it.get_priority() << ": " << *it << std::endl;
            ++it;
        }

        return stream;
    }
};
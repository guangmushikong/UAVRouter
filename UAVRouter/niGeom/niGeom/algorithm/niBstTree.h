//! \file
// \brief
// The threading binary sort tree template class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niBstTree_H
#define niBstTree_H

#ifdef _DEBUG
#include <assert.h>
#include <stack>
#endif

#include <stack>
#include <iostream>

namespace ni
{
    namespace algorithm
    {
        // Compare function type
        typedef int (*_FnCompare_)(const void *p1, const void *p2);

        template<typename _T> class niBstTree;

        /**
         * \brief bst node
         *
         */
        template<typename _T>
        class niBstNode
        {
        public:
            niBstNode               ()
            {
                l_child     = NULL;
                r_child     = NULL;
                l_link      = NULL;
                r_link      = NULL;
            }

            ~niBstNode              ()
            {
            }

            const _T                Get                     () const
            {
                return m_val;
            }

            void                    Get                     (_T& _val) const
            {
                _val = m_val;
            }

            void                    Set                     (const _T& _val)
            {
                m_val = _val;
            }

            void                    Reset                   ()
            {
                l_child     = NULL;
                r_child     = NULL;
                l_link      = NULL;
                r_link      = NULL;
            }


            friend                  class niBstTree<_T>;

        private:
            _T                      m_val;
            niBstNode<_T>*          l_child;
            niBstNode<_T>*          r_child;
            niBstNode<_T>*          l_link;     // Used to traversed the sort tree.
            niBstNode<_T>*          r_link;     // Used to traversed the sort tree.
        };

        /**
         * \brief binary sorting tree
         *
         */
        template<typename _T>
        class niBstTree
        {
        public:

            /**
             * \brief iterator used to visit bst tree
             *
             */
            class _iterator
            {
            public:
                _iterator           (){}
                _iterator           (niBstNode<_T>* _l)
                {
                    _link = _l;
                }
                _iterator           (const _iterator &_it)
                {
                    _link = _it._link;
                }

                _iterator&          operator++()
                {
                    _link = _link->r_link;
                    return *this;
                }

                _iterator           operator++(int)
                {
                    niBstNode<_T>* _l = _link;
                    _link = _link->r_link;
                    return _iterator(_l);
                }
                _T&                 operator*()
                {
                    return _link->m_val;
                }
                _T                  operator*() const
                {
                    return _link->m_val;
                }

                bool                operator==(const _iterator &_it) const
                {
                    return _link == _it._link;
                }

                bool                operator!=(const _iterator &_it) const
                {
                    return _link != _it._link;
                }

            protected:
                niBstNode<_T>*      _link;
            };

        public:
            niBstTree               (_FnCompare_ _fn_)
            {
                m_root = NULL;
                m_head = NULL;
                m_tail = NULL;
                m_iter = NULL;
                m_num_nodes = 0;
                _fn_comp_ = _fn_;
            }

            ~niBstTree              ()
            {
                Destory();
            }

            _iterator               _begin                  () const
            {
                return _iterator(m_head);
            }
            _iterator               _tail                   () const
            {
                return _iterator(m_tail);
            }
            _iterator               _end                    () const
            {
                return _iterator(NULL);
            }


            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Quickly build a balanced binary sort tree.
            *
            * @param       _sorted:     pointer of sorted datas
            * @param       _num:        num of sorted datas
            * @return      true on success, false on failure
            */
            bool                    Build                    (_T *_sorted, int _num)
            {
                Destory();
                if (NULL == _sorted)
                    return false;

                niBstNode<_T> *last = NULL;
                niBstNode<_T> *pNew = NULL;
                niBstNode<_T> ** buffer = new niBstNode<_T>*[_num];

                // Create tree node, and connect l_link, r_link
                for (int i = 0; i < _num; ++i)
                {
                    pNew = new niBstNode<_T>;
                    if (NULL != last)
                        last->r_link = pNew;
                    pNew->m_val = _sorted[i];
                    pNew->l_link = last;
                    buffer[i] = pNew;
                    last = pNew;
                }

                // Create a tree iteratively.
                m_root = _Build(buffer, 0, _num-1);
                m_head = buffer[0];
                m_tail = buffer[_num-1];
                m_num_nodes = _num;
                delete []buffer;
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Destory
            //-----------------------------------------------------------------------------
            /**
            * Destory bst tree.
            *
            */
            void                    Destory                 ()
            {
                niBstNode<_T>* iter = m_head;
                while (NULL != iter)
                {
                    niBstNode<_T>* last = iter;
                    iter = iter->r_link;
                    delete last;
                }
                m_root = NULL;
                m_head = NULL;
                m_tail = NULL;
                m_iter = NULL;
                m_num_nodes = 0;
            }


            //-----------------------------------------------------------------------------
            // FUNCTION Delete
            //-----------------------------------------------------------------------------
            /**
            * Delete a value from bst tree.
            *
            * @param       _val:        the value need delete
            */
            void                    Delete                  (const _T& _val)
            {
                niBstNode<_T>* iter = m_root;
                niBstNode<_T>* father = NULL;

                int _comp_sign_, direct = 0;

                while (NULL != iter)
                {
                    _comp_sign_ = _fn_comp_(&_val, &iter->m_val);
                    if (-1 == _comp_sign_)
                    {
                        direct = -1;
                        father = iter;
                        iter = iter->l_child;
                    }
                    else if (1 == _comp_sign_)
                    {
                        direct = 1;
                        father = iter;
                        iter = iter->r_child;
                    }
                    else
                    {
                        _Delete(iter, father, direct);
                        break;
                    }
                }
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Find
            //-----------------------------------------------------------------------------
            /**
            * Find _val in bst tree
            *
            * @param       _val:        find this value
            * @return      if found, return the pointer of the node, or return NULL
            */
            niBstNode<_T>*          Find                    (const _T& _val)
            {
                niBstNode<_T>* iter = m_root;
                int _comp_sign_;
                while (NULL != iter)
                {
                    _comp_sign_ = _fn_comp_(&_val, &iter->m_val);
                    if (-1 == _comp_sign_)
                        iter = iter->r_child;
                    else if (1 == _comp_sign_)
                        iter = iter->l_child;
                    else
                        return iter;
                }
                return NULL;
            }
            //-----------------------------------------------------------------------------
            // FUNCTION Insert
            //-----------------------------------------------------------------------------
            /**
            * Insert a value into bst tree
            *
            * @param       _val:        insert the value into bst tree
            * @return      if found the value, return the pointer of the node
            *              if not found the value, insert it into bst tree,
            *                and return the pointer of the node,
            *              if failed, return NULL
            */
            niBstNode<_T>*          Insert                  (const _T& _val)
            {
                niBstNode<_T>* pNew = NULL;
                niBstNode<_T>* iter = NULL;

                pNew = new niBstNode<_T>;
                pNew->Set(_val);

                // If root is NULL, create root, and return
                if (NULL == m_root)
                {
                    m_root = pNew;
                    m_head = pNew;
                    m_tail = pNew;
                    ++m_num_nodes;
                    return pNew;
                }
                else
                {
                    int _comp_sign_;
                    iter = m_root;
                    while (NULL != iter)
                    {
                        _comp_sign_ = _fn_comp_(&_val, &iter->m_val);
                        if (1 == _comp_sign_)           // If lager than iter->val, enter the right subtree.
                        {
                            if (NULL == iter->r_child)  // If right child is NULL, insert the value,
                            {                           // and maintain the threading.
                                iter->r_child = pNew;
                                pNew->l_link = iter;
                                pNew->r_link = iter->r_link;
                                if (NULL != iter->r_link)
                                    iter->r_link->l_link = pNew;
                                iter->r_link = pNew;
                                if (iter == m_tail)
                                    m_tail = pNew;
                                ++m_num_nodes;
                                return pNew;
                            }
                            else
                            {
                                iter = iter->r_child;
                            }
                        }
                        else if (-1 == _comp_sign_)     // If lager than iter->val, enter the left subtree.
                        {
                            if (NULL == iter->l_child)  // If right child is NULL, insert the value,
                            {                           // and maintain the threading.
                                iter->l_child = pNew;
                                pNew->l_link = iter->l_link;
                                pNew->r_link = iter;
                                if (NULL != iter->l_link)
                                    iter->l_link->r_link = pNew;
                                iter->l_link = pNew;
                                if (iter == m_head)
                                    m_head = pNew;
                                ++m_num_nodes;
                                return pNew;
                            }
                            else
                            {
                                iter = iter->l_child;
                            }
                        }
                        else
                        {
                            delete pNew;
                            return iter;
                        }
                    }
                }
                return NULL;
            }

            int                     NumNodes                () const
            {
                return m_num_nodes;
            }


        private:
            //-----------------------------------------------------------------------------
            // FUNCTION _Build
            //-----------------------------------------------------------------------------
            /**
            * Quickly build iteration.
            *
            * @param       buffer:          pointer of nodes, used to build subtree
            * @param       l:               start index of nodes for subtree
            * @param       r:               finish index of nodes for subtree
            * @return      if success, return the root of subtree,
            *                or return NULL
            */
            niBstNode<_T>*          _Build                  (niBstNode<_T> **buffer, int l, int r)
            {
                int m = (l + r) / 2;
                niBstNode<_T> *father = buffer[m];
                if (l == m-1)
                {
                    father->l_child = buffer[l];
                }
                else if (l < m-1)
                {
                    father->l_child = _Build(buffer, l, m-1);
                }

                if (m+1 == r)
                {
                    father->r_child = buffer[r];
                }
                else if (m+1 < r)
                {
                    father->r_child = _Build(buffer, m+1, r);
                }
                return father;
            }
            //-----------------------------------------------------------------------------
            // FUNCTION _Delete
            //-----------------------------------------------------------------------------
            /**
            * Delete node
            *
            * @param       iter:            the node need to be deleted
            * @param       father:          the father of iter
            * @param       direct:          0 indicate iter is the left child of father,
            *                               1 indicate iter is the right child of father
            */
            void                    _Delete                 (
                                                            niBstNode<_T>* iter,
                                                            niBstNode<_T>* father,
                                                            int direct)
            {
                if (NULL == iter)
                    return;

                niBstNode<_T>* l_child = iter->l_child;
                niBstNode<_T>* r_child = iter->r_child;
                if (NULL == l_child)
                {
                    l_child = r_child;
                }
                else
                {
                    iter->l_link->r_child = r_child;
                }

                // Maintain threading
                if (NULL != iter->l_link)
                {
                    iter->l_link->r_link = iter->r_link;
                }
                if (NULL != iter->r_link)
                {
                    iter->r_link->l_link = iter->l_link;
                }

                if (NULL != father)
                {
                    if (-1 == direct)
                        father->l_child = l_child;
                    else if (1 == direct)
                        father->r_child = l_child;
                }
                if (iter == m_root)
                {
                    m_root = l_child;
                }
                if (iter == m_head)
                {
                    m_head = iter->r_link;
                }
                if (iter == m_tail)
                {
                    m_tail = iter->l_link;
                }
                delete iter;
                --m_num_nodes;
            }

#ifdef _DEBUG

            void                    __DEBUG__CHECK__(const _T& _val)
            {
                return;
                niBstNode<_T>* iter = m_head;
                while (NULL != iter)
                {
                    int _comp_sign_ = _fn_comp_(&_val, &iter->m_val);
                    if (_comp_sign_ == 0)
                        assert(0);
                    iter = iter->r_link;
                }
            }

            void                    __DEBUG__CHECK__()
            {
                return;
                niBstNode<_T>* iter;

                int counter = 0;
                std::stack<niBstNode<_T>*> stack;
                stack.push(m_root);
                while (!stack.empty())
                {
                    iter = stack.top();
                    stack.pop();

                    while (iter != NULL)
                    {
                        counter++;
                        if (iter->r_child != NULL)
                        {
                            stack.push(iter->r_child);
                        }
                        iter = iter->l_child;
                    }
                }
                if (counter != m_num_nodes)
                    assert(0);

                counter = 0;
                iter = m_head;
                while (iter != NULL)
                {
                    ++counter;
                    iter = iter->r_link;
                }
                if (counter != m_num_nodes)
                    assert(0);
            }
#endif

        private:
            niBstNode<_T>*          m_root;
            niBstNode<_T>*          m_head;
            niBstNode<_T>*          m_tail;
            niBstNode<_T>*          m_iter;

            int                     m_num_nodes;

            _FnCompare_             _fn_comp_;
        };
    }
}
#endif

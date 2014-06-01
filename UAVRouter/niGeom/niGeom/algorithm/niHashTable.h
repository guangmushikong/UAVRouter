#ifndef niHashTable_H
#define niHashTable_H

namespace ni
{
    namespace algorithm
    {
        /**
         * \brief hash table
         *
         */
        template<typename _T>
        class niHashTable
        {
        private:
            /**
             * \brief  node of hash table
             *
             */
            class _dsNode
            {
            public:
                _dsNode                 ()
                {
                    m_link = NULL;
                    m_list = NULL;
                }

                _dsNode                 (const _T &_val) : m_val(_val)
                {
                    m_link = NULL;
                }
                ~_dsNode                ()
                {
                }

                bool                    operator==              (const _dsNode &bucket)
                {
                    return (m_val == bucket.m_val);
                }

            public:
                _T                      m_val;
                unsigned long           m_key;
                _dsNode*                m_link;
                _dsNode*                m_list;
            };
        public:
            niHashTable                 ()
            {
                m_entry_list = NULL;
                m_buckets = NULL;
            }

            ~niHashTable                ()
            {
                Clear();
            }

            void                        Clear                   ()
            {
                _dsNode *iter;
                while (NULL != m_entry_list)
                {
                    iter = m_entry_list->m_list;
                    delete m_entry_list;
                    m_entry_list = iter;
                }

                if (NULL != m_buckets)
                {
                    delete []m_buckets;
                    m_buckets = NULL;
                }
                m_bucket_size = m_num_nodes = 0;
            }

            bool                        Create                  (const unsigned long _size = 521, bool force = false)
            {
                Clear();
                if (force)
                {
                    m_bucket_size = _size;
                }
                else
                {
                    unsigned long i;
                    for (i = 0; i < 26; i++)
                    {
                        if (_size < sBucketSizeArray[i])
                        {
                            m_bucket_size = sBucketSizeArray[i];
                            break;
                        }
                    }

                    if (0 == m_bucket_size)
                    {
                        m_bucket_size = _size;
                    }
                }

                try
                {
                    m_buckets = new _dsNode*[m_bucket_size];
                }
                catch (std::bad_alloc)
                {
                    m_buckets = NULL;
                    m_bucket_size = 0;
                    return false;
                }

                memset( (void*)m_buckets, 0, sizeof(_dsNode*) * m_bucket_size );

                return true;
            }

            bool                        Find                    (const _T &_val, unsigned long _key)
            {
                unsigned long hash = HashKey(_key);
                if (-1 == hash)
                    return false;

                _dsNode* iter = m_buckets[hash];
                while (NULL != iter)
                {
                    if (_key == iter->m_key && _val == iter->m_val)
                    {
                        return true;
                    }
                    iter = iter->m_link;
                }
                return false;
            }

            bool                        Insert                  (const _T &_val, unsigned long _key)
            {
                unsigned long hash = HashKey(_key);
                if (-1 == hash)
                    return false;

                _dsNode *node = new _dsNode;
                node->m_val = _val;
                node->m_key = _key;

                //insert to entry
                node->m_list = m_entry_list;
                m_entry_list = node;

                node->m_link = m_buckets[hash];
                m_buckets[hash] = node;
                m_num_nodes++;

                if (m_num_nodes > m_bucket_size * 2)
                {
                    unsigned long i;
                    int new_bucket_size = 0;
                    for (i = 0; i < 26; i++)
                    {
                        if (m_num_nodes < sBucketSizeArray[i])
                        {
                            new_bucket_size = sBucketSizeArray[i];
                            break;
                        }
                    }

                    if (0 == new_bucket_size)
                        return false;

                    m_bucket_size = new_bucket_size;

                    if (NULL != m_buckets)
                    {
                        delete []m_buckets;
                        m_buckets = NULL;
                    }

                    try
                    {
                        m_buckets = new _dsNode*[m_bucket_size];
                    }
                    catch (std::bad_alloc)
                    {
                        return false;
                    }

                    memset( (void*)m_buckets, 0, sizeof(_dsNode*) * m_bucket_size );
                    _dsNode* iter = m_entry_list;
                    while (NULL != iter)
                    {
                        iter->m_link = NULL;
                        hash = HashKey(iter->m_key); 
                        iter->m_link = m_buckets[hash];
                        m_buckets[hash] = iter;
                        iter = iter->m_list;
                    }
                }
                return true;
            }

        protected:
            unsigned long               HashKey                 (unsigned long _key)
            {
                if (m_bucket_size > 0)
                    return _key % m_bucket_size;
                else
                    return -1;
            }


        private:
            static unsigned long        sBucketSizeArray[26];
            _dsNode**                   m_buckets;
            _dsNode*                    m_entry_list;
            unsigned long               m_bucket_size;
            unsigned long               m_num_nodes;
        };


        template<typename _T>
        unsigned long niHashTable<_T>::sBucketSizeArray[] =
        {
            11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209,
            16411, 32771, 65537, 131101, 262147, 524309, 1048583, 2097169,
            4194319, 8388617, 16777259, 33554467, 67108879, 134217757, 268435459
        };
    }
}

#endif
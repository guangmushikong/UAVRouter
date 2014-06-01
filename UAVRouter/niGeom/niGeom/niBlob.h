//! \file
// \brief
// Define class niBlob
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niBlob_H
#define niBlob_H

#include <string>
#include <memory.h>

namespace ni
{
    /**
     * \brief blob
     *
     */
    class niBlob
    {
    public:
        niBlob                      () : m_blob(NULL), m_blobSize(0), m_memSize(0)
        {
        }

        niBlob                      (const niBlob &other)
        {
            if (NULL == other.m_blob)
            {
                m_blob          = NULL;
                m_blobSize      = 0;
                m_memSize       = 0;
            }
            else
            {
                m_blobSize      = other.m_blobSize;
                m_memSize       = m_blobSize + 1;
                m_blob          = new char[m_memSize];
                memcpy( (void*) m_blob, (const void*)other.m_blob, sizeof(char) * m_memSize );
            }
        }

        niBlob                      (const char* blob, size_t blobSize)
        {
            if (NULL == blob || 0 == blobSize)
            {
                m_blob          = NULL;
                m_blobSize        = 0;
                m_memSize   = 0;
            }
            else
            {
                m_blobSize      = blobSize;
                m_memSize       = m_blobSize + 1;
                m_blob          = new char[m_memSize];
                memcpy( (void*) m_blob, (const void*)blob, sizeof(char) * m_blobSize );
                m_blob[m_blobSize] = 0;
            }
        }

        ~niBlob                     ()
        {
            if (NULL != m_blob)
            {
                delete []m_blob;
            }
        }

        char*                       Blob                        () const
        {
            return m_blob;
        }

        size_t                      BlobSize                    () const
        {
            return m_blobSize;
        }

        bool                        AllocMem                    (size_t blobSize)
        {
            if (m_memSize <= blobSize)
            {
                if (NULL != m_blob)
                {
                    delete []m_blob;
                    m_blob = NULL;
                }
            }
            if (NULL == m_blob)
            {
                m_memSize   = blobSize + 1;
                m_blob      = new char[m_memSize];
            }
            return NULL != m_blob;
        }

        bool                        Make                        (const char* blob, size_t blobSize)
        {
            if (NULL == blob || 0 == blobSize)
            {
                m_blobSize = 0;
                return false;
            }

            bool bStat = AllocMem(blobSize);
            if (!bStat)
            {
                return false;
            }

            m_blobSize = blobSize;

            memcpy( (void*) m_blob, (const void*)blob, sizeof(char) * m_blobSize );
            m_blob[m_blobSize] = 0;

            return true;
        }

        niBlob&                     operator=                   (const niBlob &other)
        {
            if (this != &other)
            {
                Make(other.m_blob, other.m_blobSize);
            }
            return *this;
        }

    private:
        char*                       m_blob;
        size_t                      m_blobSize;
        size_t                      m_memSize;
    };
}
#endif
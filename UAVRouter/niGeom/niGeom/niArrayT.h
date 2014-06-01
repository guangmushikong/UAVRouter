//! \file
// \brief
// Define enum types of geometry
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niArrayT_H
#define niArrayT_H

#include <niGeom/niBlob.h>
#include <vector>

namespace ni
{
    /**
     * \brief array
     *
     */
    template<typename Type>
    class niArrayT : public std::vector<Type>
    {
        /*
        public:
        bool            convertToBlob       (niBlob &blob) const
        {
        size_t blobSize = sizeof(Type) * this->size();
        if (blobSize < 1)
        {
        return false;
        }

        const char * buffer = (char*)(&(*this)[0]);

        return blob.Make(buffer, blobSize);
        }

        bool            convertToBlob       (size_t &blobSize, char **blob) const
        {
        if (NULL == blob)
        return false;
        if (this->size() < 1)
        {
        return false;
        }

        blobSize = sizeof(Type) * this->size();
        if (*blob)
        {
        delete [](*blob);
        }
        (*blob) = new char[blobSize+1];
        (*blob)[blobSize] = 0;
        memcpy( *blob, (void*)(&(*this)[0]), blobSize );
        return true;
        }

        bool            convertFromBlob     (const niBlob &blob)
        {
        return convertFromBlob( blob.BlobSize(), blob.Blob() );
        }

        bool            convertFromBlob     (size_t blobSize, const char *blob)
        {
        if (blobSize < 1 || NULL == blob)
        {
        return false;
        }
        size_t num = blobSize / sizeof(Type);
        if (num * sizeof(Type) != blobSize)
        {
        return false;
        }
        this->resize(num);
        const Type* data = (Type*)blob;
        for (size_t i = 0; i < num; ++i)
        {
        (*this)[i] = data[i];
        }

        return true;
        }
        //*/
    };

}

#endif

#ifndef niSmartRefT_H
#define niSmartRefT_H

#include <niGeom/niRefCounter.h>

namespace ni
{
    /**
     * \brief smart pointer
     *
     */
    template<typename Type>
    class niSmartRefT
    {
    public:
        niSmartRefT                 () : m_ptr(NULL)
        {
        }

        niSmartRefT                 (Type* ptr) : m_ptr(ptr)
        {
            if (m_ptr)
            {
                niRef(m_ptr);
            }
        }

        niSmartRefT                 (const niSmartRefT& rp) : m_ptr(rp.m_ptr)
        {
            if (m_ptr)
            {
                niRef(m_ptr);
            }
        }

        template<typename NewType>
        niSmartRefT                 (const niSmartRefT<NewType>& rp) : m_ptr(rp.m_ptr)
        {
            if (m_ptr)
            {
                niRef(m_ptr);
            }
        }

        ~niSmartRefT                ()
        {
            if (m_ptr)
            {
                niUnRef(m_ptr);
                m_ptr = 0;
            }
        }

        niSmartRefT&                operator=                   (const niSmartRefT& other)
        {
            assign(other);
            return *this;
        }

        template<typename NewType>
        niSmartRefT&                operator=                   (const niSmartRefT<NewType>& other)
        {
            assign(other);
            return *this;
        }

        inline niSmartRefT&         operator=                   (Type* ptr)
        {
            if (m_ptr==ptr)
            {
                return *this;
            }

            Type* tmp_ptr = m_ptr;
            m_ptr = ptr;
            if (m_ptr)
            {
                niRef(m_ptr);
            }

            if (tmp_ptr)
            {
                niUnRef(tmp_ptr);
            }
            return *this;
        }

        bool                        operator==                  (const niSmartRefT& rp) const
        {
            return (m_ptr == rp.m_ptr);
        }

        bool                        operator==                  (const Type* ptr) const
        {
            return (m_ptr == ptr);
        }

        friend bool                 operator==                  (const Type* ptr, const niSmartRefT& rp)
        {
            return (ptr == rp.m_ptr);
        }

        bool                        operator!=                  (const niSmartRefT& rp) const
        {
            return (m_ptr != rp.m_ptr);
        }

        bool                        operator!=                  (const Type* ptr) const
        {
            return (m_ptr != ptr);
        }

        friend bool                 operator!=                  (const Type* ptr, const niSmartRefT<Type>& rp)
        {
            return (ptr != rp.m_ptr);
        }

        bool                        operator<                   (const niSmartRefT& rp) const
        {
            return (m_ptr < rp.m_ptr);
        }

    public:
        Type&                       operator*                   () const
        {
            return *m_ptr;
        }

        Type*                       operator->                  () const
        {
            return m_ptr;
        }

        Type*                       get                         () const
        {
            return m_ptr;
        }

        bool                        operator!                   () const
        {
            return m_ptr==0;
        }

        bool                        valid                       () const
        {
            return m_ptr!=0;
        }

        void                        swap                        (niSmartRefT& rp)
        {
            Type* tmp = m_ptr;
            m_ptr = rp.m_ptr;
            rp.m_ptr = tmp;
        }

    private:

        template<typename NewType> void assign                  (const niSmartRefT<NewType>& rp)
        {
            if (m_ptr == rp.m_ptr)
            {
                return;
            }
            Type* tmp_ptr = m_ptr;
            m_ptr = rp.m_ptr;
            if (m_ptr)
            {
                niRef(m_ptr);
            }

            if (tmp_ptr)
            {
                niUnRef(tmp_ptr);
            }
        }

        Type*                       m_ptr;
    };


    template<typename Type>
    inline void swap(niSmartRefT<Type>& rp1, niSmartRefT<Type>& rp2)
    {
        rp1.swap(rp2);
    }

    template<typename Type>
    inline Type* get_pointer(const niSmartRefT<Type>& rp)
    {
        return rp.get();
    }

    template<typename NewType, typename OldType>
    inline niSmartRefT<NewType> static_pointer_cast(const niSmartRefT<OldType>& rp)
    {
        return static_cast<NewType*>(rp.get());
    }

    template<typename NewType, typename OldType>
    inline niSmartRefT<NewType> dynamic_pointer_cast(const niSmartRefT<OldType>& rp)
    {
        return dynamic_cast<NewType*>(rp.get());
    }

    template<typename NewType, typename OldType>
    inline niSmartRefT<NewType> const_pointer_cast(const niSmartRefT<OldType>& rp)
    {
        return const_cast<NewType*>(rp.get());
    }
}
#endif

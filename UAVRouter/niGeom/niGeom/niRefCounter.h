#ifndef niRefCounter_H
#define niRefCounter_H

namespace ni
{
    /**
     * \brief ref counter of smart ref
     *
     */
    class niRefCounter
    {
    protected:
        niRefCounter            () : m_ref_counter(0)
        {
        }

        virtual ~niRefCounter   ()
        {
        }

    public:
        void                    ref                     () const
        {
            ++m_ref_counter;
        }

        void                    unref                   () const
        {
            --m_ref_counter;
            if (m_ref_counter <= 0)
            {
                m_ref_counter = 0;
                delete this;
            }
        }

    protected:
        mutable int             m_ref_counter;
    };
    void                        niRef                   (const niRefCounter* refCounter);
    void                        niUnRef                 (const niRefCounter* refCounter);
}
#endif


#include <niGeom/niRefCounter.h>

namespace ni
{
    void niRef(const niRefCounter* refCounter)
    {
        if (refCounter)
            refCounter->ref();
    }

    void niUnRef(const niRefCounter* refCounter)
    {
        if (refCounter)
            refCounter->unref();
    }
}
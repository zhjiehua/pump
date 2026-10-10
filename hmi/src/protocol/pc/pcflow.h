#ifndef PROTOCOL_PC_PCFLOW_H
#define PROTOCOL_PC_PCFLOW_H

#include <QtGlobal>

/** weiduodianzi processCmd4Pc / processCmd4PcClarity flow scaling. */
namespace PcFlow {

inline double fromArg(int pumpType, quint32 arg, bool clarity)
{
    if (pumpType <= 1)
        return arg / 1000.0;
    if (clarity)
    {
        if (pumpType <= 6)
            return arg / 100.0;
        return arg / 10.0;
    }
    return arg / 10.0;
}

inline double cxthPercent(quint32 add)
{
    return add == 0 ? 100.0 : double(add - 1u);
}

} // namespace PcFlow

#endif

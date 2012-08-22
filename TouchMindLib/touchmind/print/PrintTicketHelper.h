#ifndef TOUCHMIND_PRINT_PRINTTICKETHELPER_H_
#define TOUCHMIND_PRINT_PRINTTICKETHELPER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace print
{

class PrintTicketHelper
{
private:
    PrintTicketHelper() {}

public:
    static void CreatePrintTicket(
        const std::wstring &deviceName,
        LPPRINTDLGEX pPDX,
        IXpsPrintJobStream *printTicketStream);
    static void WritePrintTicket(IStream *pStream, IXpsPrintJobStream *pPrintStream);
};

} // print
} // touchmind

#endif // TOUCHMIND_PRINT_PRINTTICKETHELPER_H_
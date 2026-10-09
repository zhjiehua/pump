#ifndef APP_CMDSOURCE_H
#define APP_CMDSOURCE_H

/** Origin of a pump command. CDS is Remote; panel keys/pages are Local. */
namespace CmdSource {
enum Value {
    Local = 0,
    Remote = 1
};
}

#endif

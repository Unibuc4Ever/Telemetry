#ifndef UNIVERSAL_TYPE_
#define UNIVERSAL_TYPE_

/**
 * Tip universal, utilizat pentru a salva informatii
 * de tip si lungime arbitrara.
 * Este utilizat pentru structuri care nu au nevoie sa stie
 * ce anume salveaza, in mod black-box.
 */

struct UniversalType {
    void* data;
    int size;
};

#endif // UNIVERSAL_TYPE_

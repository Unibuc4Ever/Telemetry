#ifndef UNIVERSAL_TYPE_
#define UNIVERSAL_TYPE_

/**
 * Tip universal, utilizat pentru a salva informatii
 * de tip si lungime arbitrara.
 * Este utilizat pentru structuri care nu au nevoie sa stie
 * ce anume salveaza, in mod black-box.
 */
typedef struct {
    void* data;
    int size;
} UniversalType;

/**
 * Allocates a new UniversalType, with a given
 * data and length.
 */
int CreateUniversalType(UniversalType** t, void* data, int size);

/**
 * Frees the alocated universaltype.
 */
int DeleteUniversalType(UniversalType** t);

#endif // UNIVERSAL_TYPE_

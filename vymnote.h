#ifndef VYMNOTE_H
#define VYMNOTE_H

#include "vymtext.h"

/*! \brief The text note belonging to one OrnamentedObj */


class VymNote:public VymText
{
public:
    QString saveToDir();
};
#endif

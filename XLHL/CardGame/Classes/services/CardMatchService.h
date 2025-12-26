#ifndef __CARD_MATCH_SERVICE_H__
#define __CARD_MATCH_SERVICE_H__

#include <cstdlib>

/**
 * @brief Stateless service for card matching rules.
 */
class CardMatchService
{
public:
    static bool canMatchByFaceDiff1(int topFace1To13, int otherFace1To13)
    {
        return std::abs(topFace1To13 - otherFace1To13) == 1;
    }
};

#endif



#ifndef __CARD_TYPES_H__
#define __CARD_TYPES_H__

/**
 * @brief Card suit type.
 * Matches the enum definition in requirements doc.
 */
enum class CardSuitType
{
    CST_NONE = -1,
    CST_CLUBS = 0,
    CST_DIAMONDS,
    CST_HEARTS,
    CST_SPADES,
    CST_NUM_CARD_SUIT_TYPES
};

/**
 * @brief Card face type.
 * Matches the enum definition in requirements doc.
 */
enum class CardFaceType
{
    CFT_NONE = -1,
    CFT_ACE = 0,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

#endif


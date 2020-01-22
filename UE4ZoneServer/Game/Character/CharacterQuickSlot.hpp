#pragma once


class Character;

class CharacterQuickSlot
{
public:
    CharacterQuickSlot(Character& chr);

private:
    Character& base_;
};
#pragma once


class Character;

class CharacterSkill
{
public:
    CharacterSkill(Character& base);
private:
    Character& base_;
};
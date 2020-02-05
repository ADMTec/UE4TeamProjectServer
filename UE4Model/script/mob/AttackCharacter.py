import UE4Model
import random

def Start(chr, mob, attackid):
    attack = 10#(random.uniform(mob.attack_max, mob.attack_min) - chr.defence)
    chr.hp -= attack
    if chr.hp < 0:
        chr.hp = 0

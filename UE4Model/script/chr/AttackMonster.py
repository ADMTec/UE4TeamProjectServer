import UE4Model
import random

def Start(chr, mob, attackid):
    attack = 1000#(random.uniform(chr.attack_max, chr.attack_min) - mob.defence)
    mob.hp -= attack
    if mob.hp < 0:
        mob.hp = 0
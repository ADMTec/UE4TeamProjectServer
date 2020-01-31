import UE4Model

def Start(chr): 
    print("StatUpdate Python Script Start")
    chr.hp = 500.0
    chr.max_hp = 500.0
    chr.attack_min = 100.0
    chr.Attack_max = 200.0
    chr.Attack_range = 30.0
    chr.Attack_speed = 10.0
    chr.defence = 100.0
    chr.speed = 300.0
    # chr.stamina = 100.0
    # chr.max_stamina = 100.0
    print("StatUpdate Python Script End")
extends Node2D

# A water cooler that provides cups of water.

func plan(person: Person, action: Action) -> void:
	if action == %GetWater:
		# This person wants water
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(2.0, 3.0)
		person.do_gain("res://objects/Water.tscn")
		person.do_stop_use(self)
		
		# Loiter for a bit to encourage a natural 'water cooler moment'
		person.do_loiter(3.2)

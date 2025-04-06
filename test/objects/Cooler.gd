extends Node2D


func plan(person: Person, action: Action) -> void:
	if action == %GetWater:
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(2.0, 3.0)
		person.do_gain("res://objects/Water.tscn")
		person.do_stop_use(self)
		person.do_loiter(1.5)

extends Node2D


func plan(person: Person, action: Action) -> void:
	if action == %GetSnacks:
		person.do_walk_to(global_position)
		person.do_start_use(self)
		person.do_wait(2.0, 3.0)
		person.do_gain("res://objects/Snack.tscn")
		person.do_loiter(0.5)
		person.do_stop_use(self)
		person.do_loiter(1.5)
